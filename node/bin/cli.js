#!/usr/bin/env node

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