#[macro_use]
extern crate clap;
extern crate time;
extern crate libtest;

use std::path::{Path, PathBuf};
use std::process::Command;
use std::fs;

use libtest::*;

use clap::{Arg, App};

macro_rules! dir_iter {
    ($input_dir:expr) => (
        WalkDir::new($input_dir).into_iter().filter_entry(|x| is_svg(x)).map(|x| x.unwrap())
    )
}

struct Data<'a> {
    work_dir: &'a str,
    threshold: u8,
    input_dir: &'a str,
    orig_pngs_dir: &'a str,
    broken_imgs_dir: &'a str,
    skip_errors_check: bool,
}

struct TotalStats {
    title: String,
    cleaned_with_errors: Vec<String>,
    // amount of files that has at least one changed pixel
    cleaned_with_errors_all: u32,
    unchanged: u32,
    total_input_size: u64,
    total_output_size: u64,
    files_count: u32,
    total_time: f64,
}

impl Default for TotalStats {
    fn default() -> TotalStats {
        TotalStats {
            title: String::new(),
            cleaned_with_errors: Vec::new(),
            cleaned_with_errors_all: 0,
            unchanged: 0,
            total_input_size: 0,
            total_output_size: 0,
            files_count: 0,
            total_time: 0.0,
        }
    }
}

struct FileStats {
    is_successful: bool,
    // AE == 0
    is_fully_successful: bool,
    orig_file_size: u64,
    new_file_size: u64,
    elapsed_time: f64,
}

impl Default for FileStats {
    fn default() -> FileStats {
        FileStats {
            is_successful: false,
            is_fully_successful: false,
            orig_file_size: 0,
            new_file_size: 0,
            elapsed_time: 0.0,
        }
    }
}

enum Cleaner<'a> {
    SvgCleaner(&'a str),
    Scour(&'a str),
    Svgo(&'a str),
}

fn main() {
    let m = App::new("stats")
        .version("0.1.0")
        .arg(Arg::with_name("workdir")
            .long("workdir").help("Sets path to work dir")
            .value_name("DIR")
            .required(true))
        .arg(Arg::with_name("input-data")
            .long("input-data").help("Sets path to SVG files dir")
            .value_name("DIR")
            .required(true))
        .arg(Arg::with_name("type")
            .long("type").help("Sets type of SVG cleaning program.")
            .value_name("NAME")
            .possible_values(&["svgcleaner", "scour", "svgo"])
            .required(true))
        .arg(Arg::with_name("cleaner")
            .long("cleaner").help("Sets path to cleaning software")
            .value_name("PATH")
            .required(true))
        .arg(Arg::with_name("threshold")
            .long("threshold").help("Sets AE threshold in percent")
            .value_name("PERCENT")
            .default_value("0"))
        .arg(Arg::with_name("skip-errors-check")
            .long("skip-errors-check").help("Skips raster images compare, which is much faster"))
        .get_matches();

    let orig_pngs = Path::new(m.value_of("workdir").unwrap()).join("orig_pngs");
    let broken_imgs = Path::new(m.value_of("workdir").unwrap()).join("broken_imgs");
    let input_dir = m.value_of("input-data").unwrap();

    let data = Data {
        work_dir: m.value_of("workdir").unwrap(),
        threshold: value_t!(m, "threshold", u8).unwrap(),
        input_dir: input_dir,
        orig_pngs_dir: orig_pngs.to_str().unwrap(),
        broken_imgs_dir: broken_imgs.to_str().unwrap(),
        skip_errors_check: m.is_present("skip-errors-check"),
    };

    create_dir(&data.work_dir);
    create_dir(&data.orig_pngs_dir);
    create_dir(&data.broken_imgs_dir);

    let cleaner_path = m.value_of("cleaner").unwrap();

    let stats = match m.value_of("type").unwrap() {
        "svgcleaner" => collect_stats(&data, input_dir, Cleaner::SvgCleaner(cleaner_path)),
        "scour" => collect_stats(&data, input_dir, Cleaner::Scour(cleaner_path)),
        "svgo" => collect_stats(&data, input_dir, Cleaner::Svgo(cleaner_path)),
        _ => return,
    };

    print_total_stats(&stats)
}

fn collect_stats(data: &Data, input_dir: &str, cleaner: Cleaner) -> TotalStats {
    let mut total = 0;
    for entry in dir_iter!(input_dir) {
        if entry.file_type().is_file() {
            total += 1;
        }
    }

    let mut total_stats = TotalStats::default();
    total_stats.files_count = total;

    total_stats.title = match &cleaner {
        &Cleaner::SvgCleaner(path) => "svgcleaner ".to_owned() + &detect_svgcleaner_ver(path).unwrap(),
        &Cleaner::Scour(path) => "scour ".to_owned() + &detect_scour_ver(path).unwrap(),
        &Cleaner::Svgo(path) => "svgo ".to_owned() + &detect_svgo_ver(path).unwrap(),
    };

    let mut idx = 1;
    for entry in dir_iter!(input_dir) {
        if entry.file_type().is_dir() {
            create_dir(gen_orig_png_dir(data, entry.path()));
            continue;
        }

        if !entry.file_type().is_file() {
            continue;
        }

        let svg_path = entry.path();
        let path_suffix = svg_path.strip_prefix(input_dir).unwrap();
        let path_suffix_str = path_suffix.to_str().unwrap();
        println!("Processing {} of {}: {}", idx, total, path_suffix_str);
        let stats = file_stats(data, svg_path, &cleaner);

        total_stats.total_input_size += stats.orig_file_size;
        total_stats.total_output_size += stats.new_file_size;
        total_stats.total_time += stats.elapsed_time;

        if stats.orig_file_size == stats.new_file_size {
            total_stats.unchanged += 1;
        }

        if !stats.is_successful {
            total_stats.cleaned_with_errors.push(path_suffix_str.to_string());
        }

        if !stats.is_fully_successful {
            total_stats.cleaned_with_errors_all += 1;
        }

        idx += 1;
    }

    total_stats
}

fn gen_orig_png_dir(data: &Data, svg_path: &Path) -> PathBuf {
    let sub_path = svg_path.strip_prefix(data.input_dir).unwrap();
    Path::new(data.orig_pngs_dir).join(sub_path)
}

fn file_stats(data: &Data, svg_path: &Path, cleaner: &Cleaner) -> FileStats {
    let svg_path_str = svg_path.to_str().unwrap();

    let mut stats = FileStats::default();

    let new_svg_path;
    {
        let file_name = svg_path.file_name().unwrap();
        new_svg_path = Path::new(data.work_dir).join(file_name);
    }
    let new_svg_path_str = new_svg_path.to_str().unwrap();

    let start = time::precise_time_ns();
    let res = match *cleaner {
        Cleaner::SvgCleaner(path) => clean_with_svgcleaner(path, svg_path_str, new_svg_path_str),
        Cleaner::Scour(path) => clean_with_scour(path, svg_path_str, new_svg_path_str),
        Cleaner::Svgo(path) => clean_with_svgo(path, svg_path_str, new_svg_path_str),
    };
    let end = time::precise_time_ns();
    if !res || !new_svg_path.exists() {
        return stats;
    }

    stats.orig_file_size = Path::new(svg_path).metadata().unwrap().len();
    stats.new_file_size = new_svg_path.metadata().unwrap().len();
    stats.elapsed_time = (end - start) as f64 / 1000000.0;

    if data.skip_errors_check {
        stats.is_successful = true;
        stats.is_fully_successful = true;
        fs::remove_file(&new_svg_path).unwrap();
        return stats;
    }

    // render original svg
    let orig_png_path = gen_orig_png_dir(data, Path::new(svg_path)).with_extension("png");
    let orig_png_path_str = orig_png_path.to_str().unwrap();
    if !orig_png_path.exists() {
        if !render_svg(svg_path_str, orig_png_path_str) {
            fs::remove_file(&new_svg_path).unwrap();
            return stats;
        }
    }

    // render cleaned file
    let new_png_path;
    {
        let file_name = svg_path.file_stem().unwrap().to_str().unwrap().to_owned();
        let file_name = file_name + "_new.png";
        new_png_path = Path::new(data.work_dir).join(file_name);
    }
    let new_png_path_str = new_png_path.to_str().unwrap();
    if !render_svg(new_svg_path_str, new_png_path_str) {
        fs::remove_file(&new_svg_path).unwrap();
        fs::remove_file(&orig_png_path).unwrap();
        return stats;
    }

    let diff_path = Path::new(data.work_dir).join("diff.png");
    let diff_path_str = diff_path.to_str().unwrap();

    let diff = compare_imgs(&data.work_dir, new_png_path_str, orig_png_path_str, diff_path_str);
    let is_successful;
    match diff {
        Some(v) => {
            if v == 0 {
                is_successful = true;
                stats.is_fully_successful = true;
            } else {
                // error less than threshold% of image - is ok
                let (w, h) = get_img_size(new_png_path_str);
                let n = (v as f64 / (w * h) as f64) * 100.0;
                is_successful = (n as u32) < (data.threshold as u32);
            }
        }
        None => is_successful = false,
    }

    if !is_successful {
        let file_name = svg_path.file_stem().unwrap().to_str().unwrap().to_owned();
        let file_name = file_name + ".png";
        let join_img_path = Path::new(data.broken_imgs_dir).join(file_name);
        let join_img_path_str = join_img_path.to_str().unwrap();
        join_images(orig_png_path_str, new_png_path_str, diff_path_str, join_img_path_str);
    }

    stats.is_successful = is_successful;

    fs::remove_file(&new_svg_path).unwrap();
    fs::remove_file(&new_png_path).unwrap();
    fs::remove_file(&diff_path).unwrap();

    stats
}

fn print_total_stats(stats: &TotalStats) {
    println!("Results for: {}", stats.title);
    println!("Files count: {}", stats.files_count);
    println!("Files cleaned with serious errors: {}", stats.cleaned_with_errors.len());
    println!("Files cleaned with any errors: {}", stats.cleaned_with_errors_all);
    println!("Unchanged files: {}", stats.unchanged);
    println!("Size after/before: {}/{}",
             stats.total_output_size, stats.total_input_size);
    println!("Cleaning ratio: {:.2}%",
             100.0 - (stats.total_output_size as f64 / stats.total_input_size as f64 * 100.0));
    println!("Cleaning time: {:.1}ms total, {:.4}ms avg",
             stats.total_time, stats.total_time / stats.files_count as f64);
    println!("Broken files: {:?}", stats.cleaned_with_errors);
}

fn clean_with_svgcleaner(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    let res = Command::new(exe_path)
                .arg(in_path)
                .arg(out_path)
                .arg("--copy-on-error")
                .arg("--quiet")
                .output();

    match res {
        Ok(_) => {
            // let s = String::from_utf8_lossy(&o.stdout);
            // if !s.is_empty() {
            //     print!("{}", s);
            // }
            return true;
        }
        Err(e) => {
            println!("Error: {:?}", e);
            return false;
        }
    }
}

fn clean_with_scour(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    let res = Command::new(exe_path)
        .arg(in_path)
        .arg(out_path)
        .arg("--enable-id-stripping")
        .arg("--enable-comment-stripping")
        .arg("--shorten-ids")
        .arg("--indent=none")
        .arg("--no-line-breaks")
        .arg("--strip-xml-prolog")
        .arg("--remove-descriptive-elements")
        .arg("--set-precision=8")
        .arg("--set-c-precision=8")
        .arg("--create-groups")
        .arg("--remove-titles")
        .arg("--remove-descriptions")
        .arg("--remove-metadata")
        .output();

    match res {
        Ok(o) => {
            // let so = String::from_utf8_lossy(&o.stdout);
            // if !so.is_empty() {
            //     println!("{}", so);
            //     return false;
            // }

            let se = String::from_utf8_lossy(&o.stderr);
            if !se.is_empty() {
                println!("{}", se);
                return false;
            }

            return true;
        }
        Err(e) => {
            println!("Error: {:?}", e);
            return false;
        }
    }
}

fn clean_with_svgo(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    let res = Command::new(exe_path)
        .arg("--quiet")
        .arg("--precision=8")
        .arg(in_path)
        .arg("-o")
        .arg(out_path)
        .output();

    match res {
        Ok(o) => {
            let s = String::from_utf8_lossy(&o.stderr);
            if s.contains("Error") {
                println!("{}", s);
                return false;
            }
            return true;
        }
        Err(e) => {
            println!("Error: {:?}", e);
            return false;
        }
    }
}

fn detect_svgcleaner_ver(exe_path: &str) -> Option<String> {
    let res = Command::new(exe_path).arg("-V").output();
    match res {
        Ok(o) => {
            let s = String::from_utf8_lossy(&o.stdout);
            let s = s.into_owned().trim().to_owned();
            let s = s.replace("svgcleaner ", "");
            return Some(s);
        }
        Err(_) => {
            return None;
        }
    }
}

fn detect_scour_ver(exe_path: &str) -> Option<String> {
    let res = Command::new(exe_path).arg("--version").output();
    match res {
        Ok(o) => {
            let s = String::from_utf8_lossy(&o.stderr);
            return Some(s.into_owned().trim().to_owned());
        }
        Err(_) => {
            return None;
        }
    }
}

fn detect_svgo_ver(exe_path: &str) -> Option<String> {
    let res = Command::new(exe_path).arg("-v").output();
    match res {
        Ok(o) => {
            let s = String::from_utf8_lossy(&o.stdout);
            return Some(s.into_owned().trim().to_owned());
        }
        Err(_) => {
            return None;
        }
    }
}

fn join_images(orig_png: &str, cleaned_png: &str, diff_png: &str, out_png: &str) {
    // convert orig.png cleaned.png diff.png +append out.png
    Command::new("convert")
        .arg(orig_png)
        .arg(cleaned_png)
        .arg(diff_png)
        .arg("+append")
        .arg(out_png)
        .status().unwrap();
}
