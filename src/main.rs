/****************************************************************************
**
** svgcleaner could help you to clean up your SVG files
** from unnecessary data.
** Copyright (C) 2012-2016 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

// TODO: reexport clap from lib
#[macro_use] extern crate clap;
#[macro_use] extern crate svgcleaner;

use std::fs;
use std::path::Path;

use svgcleaner::cli::*;
use svgcleaner::cleaner::*;
use svgcleaner::error::CleanerError;

macro_rules! try_msg {
    ($e:expr) => ({
        match $e {
            Ok(o) => o,
            Err(e) => {
                println!("Error: {:?}.", e);
                return;
            }
        }
    })
}

fn main() {
    let app = prepare_app();
    let args = match app.get_matches_safe() {
        Ok(a) => a,
        Err(mut e) => {
            // change case before printing an error to match svgcleaner's format
            if e.message.starts_with("error:") {
                e.message = e.message.replace("error:", "Error:");
            }
            e.exit();
        }
    };

    if !check_values(&args) { return; }
    let parse_opt = gen_parse_options(&args);
    let write_opt = gen_write_options(&args);

    let in_file  = args.value_of("in-file").unwrap();
    let out_file = args.value_of("out-file").unwrap();

    if !Path::new(in_file).exists() {
        println!("Error: {:?}.", CleanerError::InputFileDoesNotExist);
        std::process::exit(0);
    }

    // load file
    let raw = try_msg!(load_file(in_file));

    let on_err = || {
        // copy original file to destination
        if get_flag!(args, Key::CopyOnError) {
            // copy a file only when paths are different
            if in_file != out_file {
                try_msg!(fs::copy(in_file, out_file));
            }
        }

        std::process::exit(0);
    };

    // parse it
    let doc = match parse_data(&raw[..], &parse_opt) {
        Ok(d) => d,
        Err(e) => {
            println!("Error: {:?}.", e);
            on_err();
            return;
        }
    };

    // allocate a buffer for the output data
    let capacity = (raw.len() as f64 * 0.8) as usize;
    let mut buf = Vec::with_capacity(capacity);
    let mut prev_size = 0;

    loop {
        // clear buffer
        buf.clear();

        // clean it
        match clean_doc(&doc, &args, &write_opt) {
            Ok(_) => {}
            Err(e) => {
                println!("Error: {:?}.", e);
                on_err();
            }
        }

        // write it
        write_buffer(&doc, &write_opt, &mut buf);

        if !get_flag!(args, Key::Multipass) {
            // do not repeat without '--multipass'
            break;
        }

        // if a size is unchaged - stop multipass
        if prev_size == buf.len() {
            break;
        }

        prev_size = buf.len();
    }

    // check that cleaned file is smaller
    if buf.len() > raw.len() {
        println!("Error: {:?}.", CleanerError::BiggerFile);
        on_err();
    }

    // save it
    try_msg!(save_file(&buf[..], out_file));

    if get_flag!(args, Key::Quiet) {
        return;
    }

    // unwrap is safe, because 'save_file' will fail on write error,
    // so file is totally exist
    let out_size = fs::File::open(out_file).unwrap().metadata().unwrap().len() as f64;
    let ratio = 100.0 - out_size / (raw.len() as f64) * 100.0;
    println!("Your image is {:.2}% smaller now.", ratio);
}
