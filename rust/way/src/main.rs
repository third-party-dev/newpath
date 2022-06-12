/*
    Copyright (c) 2022 Vincent Agriesti

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

use std::env;
use std::process;
use atty::Stream;
use std::io::{self, /*Write, Read, print,*/ stdin, Read};

// stdin().read_to_string(String) -> Result<usize>
//print!()
//eprint!()

fn main() {

    let isatty = atty::is(Stream::Stdin);
    println!("isatty(STDIN): {}", isatty);
    if !isatty {
        println!("STDIN HERE");
        let mut inp = String::from("");
        let sz = stdin().read_to_string(&mut inp).unwrap();
        println!("STDIN: {}", inp);
    }

    let path = env::var("PATH").unwrap();
    println!("PATH: {}", path);

    for c in path.chars(){
        println!("PATH[0]: {}",c);
        break;
    }

    let args: Vec<String> = env::args().collect();
    println!("ARGS: {:?}", args[0]);

    println!("Is help? {}", args[1] == "--help");

    //let str: String = String::from(path);
    //let byts: &[u8] = str.as_bytes();
    //if io::stdout().write_all(byts).is_err() {
    //    println!("Failed.");
    //}
    print!("{}", path);

    process::exit(1);
}
