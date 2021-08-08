#[no_mangle]
pub extern "C" fn fib_recursive(n: u64) -> u64 {
    match n {
        0 => 0,
        1 => 1,
        _ => fib_recursive(n - 2) + fib_recursive(n - 1),
    }
}

#[no_mangle]
pub extern "C" fn fib_tail_recursive(n: u64) -> u64 {
    tcfib(n, 0, 1)
}

fn tcfib(n: u64, a: u64, b: u64) -> u64 {
    match n {
        0 => a,
        1 => b,
        _ => tcfib(n - 1, b, a + b),
    }
}

#[no_mangle]
pub extern "C" fn fib_loop(n: u64) -> u64 {
    let mut a = 0;
    let mut b = 1;

    for _ in 0..n {
        let c = a + b;
        a = b;
        b = c;
    }

    a
}
