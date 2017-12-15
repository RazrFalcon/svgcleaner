// svgcleaner could help you to clean up your SVG files
// from unnecessary data.
// Copyright (C) 2012-2017 Evgeniy Reizner
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

extern crate svgcleaner;
extern crate log;
extern crate fern;

use std::fmt;
use std::fs;
use std::path::Path;
use std::io::{
    stderr,
    Write,
};

use svgcleaner::cli::{
    self,
    InputFrom,
    OutputTo,
    Key,
    KEYS,
};
use svgcleaner::{
    cleaner,
    ChainedErrorExt,
};

macro_rules! try_msg {
    ($e:expr) => ({
        match $e {
            Ok(o) => o,
            Err(e) => {
                writeln!(stderr(), "Error: {}.", e).unwrap();
                return;
            }
        }
    })
}

fn main() {
    fern::Dispatch::new()
        .format(log_format)
        .level(log::LogLevelFilter::Warn)
        .chain(std::io::stderr())
        .apply().unwrap();

    let app = cli::prepare_app();
    let args = match app.get_matches_safe() {
        Ok(a) => a,
        Err(mut e) => {
            // Change case before printing an error to match svgcleaner's format.
            if e.message.starts_with("error:") {
                e.message = e.message.replace("error:", "Error:");
            }
            e.exit();
        }
    };

    if !cli::check_values(&args) { return; }
    let parse_opt = cli::gen_parse_options(&args);
    let write_opt = cli::gen_write_options(&args);
    let cleaning_opt = cli::gen_cleaning_options(&args);

    let input = cli::input(&args);
    let output = cli::output(&args);

    if let InputFrom::File(path) = input {
        if !Path::new(path).exists() {
            writeln!(stderr(), "Error: input file does not exist.").unwrap();
            return;
        }
    }

    // Load data.
    let raw = match input {
        InputFrom::Stdin => try_msg!(cleaner::load_stdin()),
        InputFrom::File(path) => try_msg!(cleaner::load_file(path)),
    };

    let on_err = || {
        // Copy original file to destination
        // only when both files are specified.
        let in_file  = if let InputFrom::File(s) = input  { Some(s) } else { None };
        let out_file = if let OutputTo::File(s)  = output { Some(s) } else { None };

        if     in_file.is_some()
            && out_file.is_some()
            && args.is_present(KEYS[Key::CopyOnError])
        {
            let inf = in_file.unwrap();
            let outf = out_file.unwrap();
            // Copy a file only when paths are different.
            if inf != outf {
                try_msg!(fs::copy(inf, outf));
            }
        }

        std::process::exit(0);
    };

    // Parse it.
    let mut doc = match cleaner::parse_data(&raw[..], &parse_opt) {
        Ok(d) => d,
        Err(e) => {
            writeln!(stderr(), "{}.", e.full_chain()).unwrap();
            on_err();
            return;
        }
    };

    // Allocate a buffer for the output data.
    let capacity = (raw.len() as f64 * 0.8) as usize;
    let mut buf = Vec::with_capacity(capacity);
    let mut prev_size = 0;

    loop {
        // Clear buffer.
        buf.clear();

        // Clean document.
        match cleaner::clean_doc(&mut doc, &cleaning_opt, &write_opt) {
            Ok(_) => {}
            Err(e) => {
                writeln!(stderr(), "{}.", e.full_chain()).unwrap();
                on_err();
                break;
            }
        }

        // Write buffer.
        cleaner::write_buffer(&doc, &write_opt, &mut buf);

        if !args.is_present(KEYS[Key::Multipass]) {
            // Do not repeat without '--multipass'.
            break;
        }

        // If the size is unchaged - exit from the loop.
        if prev_size == buf.len() {
            break;
        }

        prev_size = buf.len();
    }

    // Check that cleaned file is smaller.
    if !args.is_present(KEYS[Key::AllowBiggerFile]) {
        if buf.len() > raw.len() {
            writeln!(stderr(), "Error: cleaned file is bigger than original.").unwrap();
            on_err();
            return;
        }
    }

    // Save buffer.
    match output {
        OutputTo::Stdout => try_msg!(cleaner::write_stdout(&buf[..])),
        OutputTo::File(path) => try_msg!(cleaner::save_file(&buf[..], path)),
    }

    if !args.is_present(KEYS[Key::Quiet]) {
        let ratio = 100.0 - (buf.len() as f64) / (raw.len() as f64) * 100.0;
        writeln!(stderr(), "Your image is {:.2}% smaller now.", ratio).unwrap();
    }
}

fn log_format(out: fern::FormatCallback, message: &fmt::Arguments, record: &log::LogRecord) {
    use log::LogLevel;

    let lvl = match record.level() {
        LogLevel::Error => "Error",
        LogLevel::Warn => "Warning",
        LogLevel::Info => "Info",
        LogLevel::Debug => "Debug",
        LogLevel::Trace => "Trace",
    };

    out.finish(format_args!(
        "{}: {}",
        lvl,
        message
    ));
}
