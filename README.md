# bench

Benchmarks functions from a shared library.

It isn't functionnal yet.

## Compilation

Just `make`.

The code isn't meant to be portable.
This should only compile on x86_64-linux-gnu. (maybe musl too)

## Usage

Run `bench --help` for help.

Example: `bench -e "^fib" libfib1.so libfib2.so`

## License

GNU General Public License v3.0 or later
