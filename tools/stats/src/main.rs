#[macro_use]
extern crate clap;
extern crate time;
extern crate libtest;

use std::path::Path;
use std::process::Command;
use std::fs;

use libtest::*;

use clap::{Arg, App};

// TODO: make threaded

macro_rules! dir_iter {
    ($input_dir:expr) => (
        WalkDir::new($input_dir).into_iter().filter_entry(|x| is_svg(x)).map(|x| x.unwrap())
    )
}

struct Data<'a> {
    work_dir: &'a str,
    render_path: Option<&'a str>,
    threshold: u8,
    cache: TestCache,
}

// TODO: min/max time
struct TotalStats {
    title: String,
    cleaned_with_errors: u64,
    unchanged: u64,
    total_input_size: u64,
    total_output_size: u64,
    files_count: u64,
    total_time: f64,
}

impl Default for TotalStats {
    fn default() -> TotalStats {
        TotalStats {
            title: String::new(),
            cleaned_with_errors: 0,
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
    orig_file_size: u64,
    new_file_size: u64,
    elapsed_time: f64,
}

impl Default for FileStats {
    fn default() -> FileStats {
        FileStats {
            is_successful: false,
            orig_file_size: 0,
            new_file_size: 0,
            elapsed_time: 0.0,
        }
    }
}

enum Cleaner<'a> {
    New(&'a str),
    Old(&'a str),
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
        .arg(Arg::with_name("render")
            .long("render").help("Sets path to SVG render")
            .value_name("PATH")
            .required_unless("skip-errors-check"))
        .arg(Arg::with_name("type")
            .long("type").help("Sets type of SVG cleaning program.")
            .value_name("NAME")
            .possible_values(&["svgcleaner", "svgcleaner-old", "svgo"])
            .required(true))
        .arg(Arg::with_name("cleaner")
            .long("cleaner").help("Sets path to cleaning software")
            .value_name("PATH")
            .required(true))
        .arg(Arg::with_name("threshold")
            .long("threshold").help("Sets AE threshold in percent")
            .value_name("PERCENT")
            .default_value("0"))
        .arg(Arg::with_name("cache-db")
            .long("cache-db").help("Sets path to the test cache db.")
            .value_name("PATH")
            .required_unless("skip-errors-check"))
        .arg(Arg::with_name("skip-errors-check")
            .long("skip-errors-check").help("Skips raster images compare, which is much faster"))
        .get_matches();

    let input_dir = m.value_of("input-data").unwrap();

    let render;
    if !m.is_present("skip-errors-check") {
        render = Some(m.value_of("render").unwrap());
    } else {
        render = None;
    }

    // TODO: no need for cache in skip-errors-check mode

    let data = Data {
        work_dir: m.value_of("workdir").unwrap(),
        render_path: render,
        threshold: value_t!(m, "threshold", u8).unwrap(),
        cache: TestCache::init(m.value_of("cache-db").unwrap()),
    };

    create_dir(&data.work_dir);

    let cleaner_path = m.value_of("cleaner").unwrap();

    let stats = match m.value_of("type").unwrap() {
        "svgcleaner" => collect_stats(&data, input_dir, Cleaner::New(cleaner_path)),
        "svgcleaner-old" => collect_stats(&data, input_dir, Cleaner::Old(cleaner_path)),
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
    total_stats.files_count = total as u64;

    total_stats.title = match &cleaner {
        &Cleaner::New(path) => "svgcleaner ".to_owned() + &detect_new_cleaner_ver(path).unwrap(),
        &Cleaner::Old(path) => "svgcleaner ".to_owned() + &detect_old_cleaner_ver(path).unwrap(),
        &Cleaner::Svgo(path) => "svgo ".to_owned() + &detect_svgo_ver(path).unwrap(),
    };

    let mut idx = 1;
    for entry in dir_iter!(input_dir) {
        if entry.file_type().is_dir() {
            continue;
        }

        let svg_path = entry.path();
        let prefix = svg_path.strip_prefix(input_dir).unwrap();
        println!("Processing {} of {}: {}", idx, total, prefix.to_str().unwrap());
        let stats = file_stats(data, svg_path, &cleaner);

        total_stats.total_input_size += stats.orig_file_size;
        total_stats.total_output_size += stats.new_file_size;
        total_stats.total_time += stats.elapsed_time;

        if stats.orig_file_size == stats.new_file_size {
            total_stats.unchanged += 1;
        }

        if !stats.is_successful {
            total_stats.cleaned_with_errors += 1;
        }

        idx += 1;

        // break;
    }

    total_stats
}

fn file_stats(data: &Data, svg_path: &Path, cleaner: &Cleaner) -> FileStats {
    let svg_path_str = svg_path.to_str().unwrap();

    let mut render_imgs = data.render_path.is_some();

    let use_cache = match *cleaner {
        Cleaner::New(_) => true,
        _ => false,
    };

    let mut stats = FileStats::default();

    let new_svg_path;
    {
        let file_name = svg_path.file_name().unwrap();
        new_svg_path = Path::new(data.work_dir).join(file_name);
    }
    let new_svg_path_str = new_svg_path.to_str().unwrap();;
    let svg_suffix = new_svg_path.strip_prefix(data.work_dir).unwrap().to_str().unwrap();

    let start = time::precise_time_ns();
    let res = match *cleaner {
        Cleaner::New(path) => clean_with_new_cleaner(path, svg_path_str, new_svg_path_str),
        Cleaner::Old(path) => clean_with_old_cleaner(path, svg_path_str, new_svg_path_str),
        Cleaner::Svgo(path) => clean_with_svgo(path, svg_path_str, new_svg_path_str),
    };
    let end = time::precise_time_ns();
    if !res || !new_svg_path.exists() {
        return stats;
    }

    stats.orig_file_size = Path::new(svg_path).metadata().unwrap().len();
    stats.new_file_size = new_svg_path.metadata().unwrap().len();
    stats.elapsed_time = (end - start) as f64 / 1000000.0;

    let cache_id = data.cache.cache_id(svg_suffix);

    if render_imgs && use_cache {
        match cache_id {
            Some(id) => {
                let new_hash = file_md5sum(new_svg_path_str);
                if data.cache.get_hash(id) == new_hash {
                    // if md5 is same as in cache - no need to render and compare images
                    render_imgs = false;
                }
            }
            None => {}
        }
    }

    if !render_imgs {
        stats.is_successful = true;
        fs::remove_file(&new_svg_path).unwrap();
        return stats;
    }

    // render original file
    let orig_png_path;
    {
        let file_name = svg_path.file_stem().unwrap().to_str().unwrap().to_owned();
        let file_name = file_name + "_orig.png";
        orig_png_path = Path::new(data.work_dir).join(file_name);
    }
    let orig_png_path_str = orig_png_path.to_str().unwrap();
    if !render_svg(&data.render_path.unwrap(), svg_path_str, orig_png_path_str) {
        fs::remove_file(&new_svg_path).unwrap();
        return stats;
    }

    // render cleaned file
    let new_png_path;
    {
        let file_name = svg_path.file_stem().unwrap().to_str().unwrap().to_owned();
        let file_name = file_name + "_new.png";
        new_png_path = Path::new(data.work_dir).join(file_name);
    }
    let new_png_path_str = new_png_path.to_str().unwrap();
    if !render_svg(&data.render_path.unwrap(), new_svg_path_str, new_png_path_str) {
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
            } else {
                // error less than threshold% of image - is ok
                let (w, h) = get_img_size(new_png_path_str);
                let n = (v as f64 / (w * h) as f64) * 100.0;
                is_successful = (n as u32) < (data.threshold as u32);
                if !is_successful {
                    println!("AE: {:.6}%", n);
                }
            }
        }
        None => is_successful = false,
    }

    if is_successful && use_cache {
        match cache_id {
            Some(id) => data.cache.update_hash(id, &file_md5sum(&new_svg_path_str)),
            None => data.cache.append_hash(svg_suffix, &file_md5sum(&new_svg_path_str)),
        }
    }

    stats.is_successful = is_successful;

    fs::remove_file(&new_svg_path).unwrap();
    fs::remove_file(&new_png_path).unwrap();
    fs::remove_file(&orig_png_path).unwrap();
    fs::remove_file(&diff_path).unwrap();

    stats
}

fn print_total_stats(stats: &TotalStats) {
    println!("Results for: {}", stats.title);
    println!("Files count: {}", stats.files_count);
    println!("Files cleaned with errors: {}", stats.cleaned_with_errors);
    println!("Unchanged files: {}", stats.unchanged);
    println!("Size after/before: {}/{}",
             stats.total_output_size, stats.total_input_size);
    println!("Cleaning ratio: {:.2}%",
             100.0 - (stats.total_output_size as f64 / stats.total_input_size as f64 * 100.0));
    println!("Cleaning time: {:.1}ms total, {:.4}ms avg",
             stats.total_time, stats.total_time / stats.files_count as f64);
}

fn clean_with_new_cleaner(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    let res = Command::new(exe_path)
                .arg(in_path)
                .arg(out_path)
                .arg("--copy-on-error=true")
                .arg("--quiet=true")
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
            println!("My err: {:?}", e);
            return false;
        }
    }
}

fn clean_with_old_cleaner(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    // Old cleaner did not support default options, so we need to set all of them.
    // This list is equal to preset=complete, but precision set to 6
    // and viewbox convertions is disabled.
    let res = Command::new(exe_path)
                .arg(in_path)
                .arg(out_path)
                .arg("--remove-prolog")
                .arg("--remove-comments")
                .arg("--remove-proc-instr")
                .arg("--remove-unused-defs")
                .arg("--remove-nonsvg-elts")
                .arg("--remove-metadata-elts")
                .arg("--remove-inkscape-elts")
                .arg("--remove-sodipodi-elts")
                .arg("--remove-ai-elts")
                .arg("--remove-corel-elts")
                .arg("--remove-msvisio-elts")
                .arg("--remove-sketch-elts")
                .arg("--remove-invisible-elts")
                .arg("--remove-empty-containers")
                .arg("--remove-duplicated-defs")
                .arg("--remove-outside-elts")
                .arg("--equal-elts-to-use")
                .arg("--ungroup-containers")
                .arg("--merge-gradients")
                .arg("--remove-gaussian-blur=0.1")
                .arg("--remove-version")
                .arg("--remove-unreferenced-ids")
                .arg("--trim-ids")
                .arg("--remove-notappl-atts")
                .arg("--remove-default-atts")
                .arg("--remove-inkscape-atts")
                .arg("--remove-sodipodi-atts")
                .arg("--remove-ai-atts")
                .arg("--remove-corel-atts")
                .arg("--remove-msvisio-atts")
                .arg("--remove-sketch-atts")
                .arg("--remove-stroke-props")
                .arg("--remove-fill-props")
                .arg("--remove-unused-xlinks")
                .arg("--group-elts-by-styles")
                .arg("--simplify-transform-matrix")
                .arg("--apply-transforms-to-defs")
                .arg("--apply-transforms-to-shapes")
                .arg("--convert-to-relative")
                .arg("--remove-unneeded-symbols")
                .arg("--remove-tiny-segments")
                .arg("--convert-segments")
                .arg("--colors-to-rrggbb")
                .arg("--rrggbb-to-rgb")
                .arg("--convert-basic-shapes")
                .arg("--apply-transforms-to-defs")
                .arg("--compact-output")
                .arg("--transform-precision=6")
                .arg("--coordinates-precision=6")
                .arg("--attributes-precision=6")
                .arg("--sort-defs")
                .output();

    match res {
        Ok(_) => {
            // let so = String::from_utf8_lossy(&o.stdout);
            // if !so.is_empty() {
            //     println!("{}", so);
            //     // return false;
            // }

            // let se = String::from_utf8_lossy(&o.stderr);
            // if !se.is_empty() {
            //     println!("{}", se);
            //     // return false;
            // }

            return true;
        }
        Err(e) => {
            println!("My err: {:?}", e);
            return false;
        }
    }
}

fn clean_with_svgo(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    let res = Command::new(exe_path)
                .arg("--quiet")
                .arg("--precision=6")
                .arg(in_path)
                .arg(out_path)
                .output();

    match res {
        Ok(o) => {
            let s = String::from_utf8_lossy(&o.stdout);
            if !s.is_empty() {
                println!("{}", s);
                return false;
            }
            return true;
        }
        Err(e) => {
            println!("My err: {:?}", e);
            return false;
        }
    }
}

fn detect_new_cleaner_ver(exe_path: &str) -> Option<String> {
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

fn detect_old_cleaner_ver(exe_path: &str) -> Option<String> {
    let res = Command::new(exe_path).arg("-v").output();
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
