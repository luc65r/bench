# bench

Benchmarks functions from a shared library.

Supports only functions of type `void ()(uint64_t)` for now
(I think the return type doesn't matter as long it is returned in RAX).

## Compilation

Just `make`.

The code isn't meant to be portable.
It should only compile on x86_64-linux-gnu. (maybe musl too)

## Usage

Run `bench --help` for help.

Example: `bench fib/libfib_*.so -e "fib" -r 100 -n 20`

## License

GNU General Public License v3.0 or later
