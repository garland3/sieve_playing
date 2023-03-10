use std::env;

fn sieve(n: usize) -> Vec<usize> {
    let mut is_prime = vec![true; n+1];
    let mut primes = Vec::new();
    for i in 2..=n {
        if is_prime[i] {
            primes.push(i);
            for j in (i..=n).step_by(i) {
                is_prime[j] = false;
            }
        }
    }
    primes
}


fn main() {
    let args: Vec<String> = env::args().collect();
    let n: usize = match args.get(1) {
        Some(arg) => arg.parse().unwrap_or(1000000),
        None => 1000000,
    };
    let start_time = std::time::Instant::now();
    let primes = sieve(n);
    let end_time = std::time::Instant::now();
    let duration = end_time - start_time;
    println!("Execution time: {} milliseconds", duration.as_millis());
    println!("Number of primes found: {}", primes.len());
}

// compile: rustc rust_sieve.rs -O 