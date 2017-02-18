extern crate svgcleaner;

use std::env;

use svgcleaner::cleaner;
use svgcleaner::{ParseOptions, WriteOptions, CleaningOptions};

// svgcleaner can be used as a library
fn main() {
    let args: Vec<_> = env::args().collect();

    if args.len() != 3 {
        println!("Usage:\n\tuse_as_lib in.svg out.svg");
        return;
    }

    let in_file  = &args[1];
    let out_file = &args[2];

    // init options
    let parse_opt = ParseOptions::default();
    let write_opt = WriteOptions::default();
    let cleaning_opt = CleaningOptions::default();

    // load file
    let raw = cleaner::load_file(in_file).unwrap();

    // parse file's content
    let doc = cleaner::parse_data(&raw[..], &parse_opt).unwrap();

    // clean document
    cleaner::clean_doc(&doc, &cleaning_opt, &write_opt).unwrap();

    // allocate a buffer for the output data
    let mut buf = Vec::with_capacity(raw.len());

    // write document to buffer
    cleaner::write_buffer(&doc, &write_opt, &mut buf);

    // save buffer
    cleaner::save_file(&buf[..], out_file).unwrap();
}
