#[macro_use]
extern crate libtest;
extern crate clap;
extern crate toml;
extern crate time;

use std::path::{Path, PathBuf};
use std::process::Command;
use std::io::prelude::*;
use std::fs;
use std::fs::File;
use std::collections::HashMap;

use libtest::*;

use clap::{Arg, App};

macro_rules! dir_iter {
    ($input_dir:expr) => (
        WalkDir::new($input_dir).into_iter().filter_entry(|x| is_svg(x)).map(|x| x.unwrap())
    )
}

struct Config {
    valid_ae_map: HashMap<String, u32>,
    min_valid_ae: u32,
    ignore_list: Vec<String>,
}

struct Data<'a> {
    work_dir: &'a str,
    svgcleaner: &'a str,
    render: &'a str,
    input_dir: &'a str,
    orig_pngs_dir: &'a str,
    config: Config,
}

// TODO: use std::env::temp_dir()

fn main() {
    let m = App::new("files-testing")
        .arg(Arg::with_name("workdir")
            .long("workdir").help("Sets path to the work dir")
            .value_name("DIR")
            .required(true))
        .arg(Arg::with_name("input-data")
            .long("input-data").help("Sets path to the SVG files dir")
            .value_name("DIR")
            .required(true))
        .arg(Arg::with_name("svgcleaner")
            .long("svgcleaner").help("Sets path to the current version of SVG Cleaner")
            .value_name("PATH")
            .required(true))
        .arg(Arg::with_name("render")
            .long("render").help("Sets path to the SVG render")
            .value_name("PATH")
            .required(true))
        .arg(Arg::with_name("cache-db")
            .long("cache-db").help("Sets path to the test cache db.")
            .value_name("PATH")
            .required(true))
        .arg(Arg::with_name("input-data-config")
            .long("input-data-config").help("Sets path to the input data config.")
            .value_name("PATH"))
        .arg(Arg::with_name("continue")
            .long("continue").help("Continue testing from the last position."))
        // .arg(Arg::with_name("threshold")
        //     .long("threshold").help("Sets AE threshold in percent")
        //     .value_name("PERCENT")
        //     .default_value("2"))
        .get_matches();

    // TODO: make --input-data-config optional
    // TODO: autodetect render
    // TODO: add --start-pos=n
    // TODO: add --verbose
    // TODO: add --threshold which conflict with --input-data-config


    let config;
    if m.is_present("input-data-config") {
        let toml = load_config(m.value_of("input-data-config").unwrap()).unwrap();
        let min_ae = toml.get("global").unwrap().lookup("min_valid_ae")
                        .unwrap().as_integer().unwrap() as u32;

        config = Config {
            valid_ae_map: get_valid_ae_list(&toml),
            min_valid_ae: min_ae,
            ignore_list: get_ignore_list(&toml),
        };
    } else {
        config = Config {
            valid_ae_map: HashMap::new(),
            min_valid_ae: 0,
            ignore_list: Vec::new(),
        };
    }

    let orig_pngs = Path::new(m.value_of("workdir").unwrap()).join("orig_pngs");
    let input_dir = m.value_of("input-data").unwrap();

    let data = Data {
        work_dir: m.value_of("workdir").unwrap(),
        svgcleaner: m.value_of("svgcleaner").unwrap(),
        render: m.value_of("render").unwrap(),
        input_dir: input_dir,
        orig_pngs_dir: orig_pngs.to_str().unwrap(),
        config: config,
    };

    create_dir(&data.work_dir);
    create_dir(&data.orig_pngs_dir);

    let pos;
    if m.is_present("continue") {
        pos = load_last_pos(&data.work_dir);
    } else {
        pos = 0;
    }

    let cache = TestCache::init(m.value_of("cache-db").unwrap());

    // File::create("out.txt").unwrap();

    let start = time::precise_time_ns();
    run_tests(&data, input_dir, pos, &cache);
    let end = time::precise_time_ns();
    println!("Elapsed: {}s", ((end - start) as f64 / 1000000.0) as u64 / 1000);
}

fn load_last_pos(work_dir: &str) -> usize {
    let mut f = match File::open(Path::new(work_dir).join("pos.txt")) {
        Ok(f) => f,
        Err(_) => return 0,
    };

    let mut s = String::new();
    f.read_to_string(&mut s).unwrap();
    s.trim().parse::<usize>().unwrap()
}

fn save_curr_pos(work_dir: &str, pos: usize) {
    let mut f = fs::File::create(Path::new(work_dir).join("pos.txt")).unwrap();
    f.write_all(pos.to_string().as_bytes()).unwrap();
}

fn load_config(path: &str) -> Option<toml::Table> {
    let mut f = File::open(path).unwrap();
    let mut s = String::new();
    f.read_to_string(&mut s).unwrap();

    let mut parser = toml::Parser::new(&s);
    match parser.parse() {
        Some(toml) => Some(toml),
        None => {
            for err in &parser.errors {
                let (loline, locol) = parser.to_linecol(err.lo);
                let (hiline, hicol) = parser.to_linecol(err.hi);
                println!("{}:{}-{}:{} error: {}",
                         loline, locol, hiline, hicol, err.desc);
            }
            None
        }
    }
}

fn get_valid_ae_list(toml: &toml::Table) -> HashMap<String, u32> {
    let mut map = HashMap::new();

    let items = match toml.get("item") {
        Some(i) => i,
        None => return map,
    };

    // TODO: check for duplicates

    for table in items.as_slice() {
        for i in table {
            let name = i.as_table().unwrap().get("name").unwrap().as_str().unwrap().to_string();
            match i.as_table().unwrap().get("valid_ae") {
                Some(v) => {
                    match map.insert(name.clone(), v.as_integer().unwrap() as u32) {
                        Some(_) => {
                            panic!("Error: {} already exist in list.", name);
                        }
                        None => {}
                    }
                }
                None => {},
            }
        }
    }

    map
}

fn get_ignore_list(toml: &toml::Table) -> Vec<String> {
    let mut vec = Vec::new();

    let items = match toml.get("item") {
        Some(i) => i,
        None => return vec,
    };

    // TODO: check for duplicates

    for table in items.as_slice() {
        for i in table {
            let name = i.as_table().unwrap().get("name").unwrap().as_str().unwrap().to_string();
            match i.as_table().unwrap().get("ignore") {
                Some(v) => {
                    if v.as_bool().unwrap() {
                        vec.push(name)
                    }
                }
                None => {},
            }
        }
    }

    vec
}

fn gen_orig_png_dir(data: &Data, svg_path: &Path) -> PathBuf {
    let sub_path = svg_path.strip_prefix(data.input_dir).unwrap();
    Path::new(data.orig_pngs_dir).join(sub_path)
}

fn run_tests(data: &Data, input_dir: &str, start_pos: usize, cache: &TestCache) {
    let mut total = 0;
    for entry in dir_iter!(input_dir) {
        if entry.file_type().is_file() {
            total += 1;
        }
    }

    let mut idx = 1;
    for entry in dir_iter!(input_dir) {
        if entry.file_type().is_dir() {
            create_dir(gen_orig_png_dir(data, entry.path()));
            continue;
        }

        if idx < start_pos {
            idx += 1;
            continue;
        }

        let file_path = entry.path();
        let sub_path = file_path.strip_prefix(data.input_dir).unwrap();

        println!("Test {} of {}: {}", idx, total, sub_path.display());

        if data.config.ignore_list.iter().any(|ref s| *s == sub_path.to_str().unwrap()) {
            println!("Test skipped.");
            idx += 1;
            continue;
        }

        if !run_test(data, &file_path, cache) {
            println!("Test failed.");
            save_curr_pos(&data.work_dir, idx);
            break;
        } else {
            println!("Test passed.");
        }
        // break;

        idx += 1;

        if idx == total {
            let pos_path = Path::new(&data.work_dir).join("pos.txt");
            if pos_path.exists() {
                fs::remove_file(pos_path).unwrap();
            }
        }
    }
}

fn run_test(data: &Data, svg_path: &Path, cache: &TestCache) -> bool {
    let svg_path_str = svg_path.to_str().unwrap();
    let orig_file_name = svg_path.file_name().unwrap().to_str().unwrap();
    let new_svg_path = Path::new(data.work_dir).join(orig_file_name);
    let out_path = new_svg_path.to_str().unwrap();
    let svg_suffix = svg_path.strip_prefix(data.input_dir).unwrap().to_str().unwrap();

    if new_svg_path.exists() {
        fs::remove_file(&new_svg_path).unwrap();
    }

    if !clean_svg(data.svgcleaner, svg_path_str, out_path) {
        return false;
    }

    let cache_id = cache.cache_id(svg_suffix);
    match cache_id {
        Some(id) => {
            let new_hash = file_md5sum(out_path);
            if cache.get_hash(id) == new_hash {
                // if md5 is same as in cache - no need to render and compare images
                fs::remove_file(&new_svg_path).unwrap();
                return true;
            }
        }
        None => {}
    }

    let new_png_path = gen_png_path(&out_path, "_new");
    let diff_path = gen_png_path(&out_path, "_diff");

    // render original svg
    let orig_png_path = gen_orig_png_dir(data, Path::new(svg_path)).with_extension("png");
    let orig_png_path_str = orig_png_path.to_str().unwrap();
    if !orig_png_path.exists() {
        if !render_svg(&data.render, svg_path_str, orig_png_path_str) {
            return false;
        }
    }

    if !render_svg(&data.render, &out_path, &new_png_path) {
        return false;
    }

    let diff = match compare_imgs(&data.work_dir, &new_png_path, orig_png_path_str, &diff_path) {
        Some(d) => d,
        None => return false,
    };

    let valid_ae = match data.config.valid_ae_map.get(svg_suffix) {
        Some(v) => *v,
        None => data.config.min_valid_ae,
    };

    if diff <= valid_ae {
        match cache_id {
            Some(id) => cache.update_hash(id, &file_md5sum(&out_path)),
            None => cache.append_hash(svg_suffix, &file_md5sum(&out_path)),
        }

        fs::remove_file(&new_svg_path).unwrap();
        fs::remove_file(new_png_path).unwrap();
        fs::remove_file(diff_path).unwrap();

        return true;
    } else {
        // fs::copy(orig_png_path, gen_png_path(&out_path, "_orig")).unwrap();
        println!("AE: {:?} of {:?}", diff, valid_ae);

        // let file = fs::OpenOptions::new().append(true).open("out.txt").unwrap();
        // let mut out_buf = io::BufWriter::new(file);
        // out_buf.write(b"[[item]]\n").unwrap();
        // write!(out_buf, "name = \"{}\"\n", svg_suffix).unwrap();
        // write!(out_buf, "valid_ae = {}\n\n", diff).unwrap();

        return false;
    }
}

fn clean_svg(exe_path: &str, in_path: &str, out_path: &str) -> bool {
    let res = Command::new(exe_path)
                .arg(in_path)
                .arg(out_path)
                .arg("--indent=2")
                .arg("--copy-on-error=true")
                .arg("--quiet=true")
                .arg("--remove-unreferenced-ids=false")
                .arg("--trim-ids=false")
                .output();

    match res {
        Ok(o) => {
            let mut s: String = String::from_utf8_lossy(&o.stdout).into_owned();
            s = s.trim().to_owned();

            if !s.is_empty() {
                println!("{}", s);
            }

            if s.find("Error").is_some() {
                // list of "not errors"
                if    s.find("Error: Scripting is not supported.").is_some()
                   || s.find("Error: Animation is not supported.").is_some()
                   || s.find("Error: Valid FuncIRI").is_some()
                   || s.find("Error: Broken FuncIRI").is_some()
                   || s.find("Error: Unsupported CSS at").is_some()
                   || s.find("Error: Element crosslink").is_some()
                   || s.find("Error: Unsupported ENTITY").is_some() {
                    return true;
                }
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
