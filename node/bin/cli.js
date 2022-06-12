#!/usr/bin/env node

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

var tty = require('tty');
var fs = require('fs');

const isatty = tty.isatty(process.stdin.fd);
const { PATH } = process.env;

let buffer = new Buffer.from('A simple buffer.\n');
function writeHandler(err, bytes) {
  if(err) console.log('Can not write to file.');
  else console.log('Wrote ' + bytes + ' bytes.');
}
fs.write(process.stdout.fd, buffer, 0, buffer.length, null, writeHandler);
fs.write(process.stdout.fd, "A simple string.\n", writeHandler);
process.stdout.write("A more simple string.\n");

console.log("isatty: ", isatty);
if (!isatty) {
  process.stdin.on('readable', () => {
    let chunk;
    while ((chunk = process.stdin.read()) !== null) {
        process.stdout.write('we have a chunk ('+chunk.length+' bytes): ');
        process.stdout.write(chunk);
    }
  });
}


console.log("PATH: ", PATH );
console.log("argv: ", process.argv);