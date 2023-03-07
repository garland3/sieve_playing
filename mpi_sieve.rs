use mpi::traits::*;
use mpi::topology::Communicator;
use std::time::Instant;

fn sieve(start: usize, end: usize, comm: &Communicator) -> Vec<usize> {
    let mut is_prime = vec![true; end-start+1];
    let mut primes = Vec::new();
    for i in 2..=end {
        if is_prime[i-start] {
            primes.push(i);
            for j in (i..=end).step_by(i) {
                is_prime[j-start] = false;
            }
        }
    }
    let global_primes: Vec<Vec<usize>> = comm.all_gather(&primes);
    let mut merged_primes = Vec::new();
    for local_primes in global_primes {
        merged_primes.extend(local_primes);
    }
    merged_primes.sort();
    merged_primes.dedup();
    merged_primes
}

fn main() {
    let universe = mpi::initialize().unwrap();
    let world = universe.world();
    let rank = world.rank();
    let size = world.size();

    let n = 100000000;
    let chunk_size = (n + size - 1) / size;
    let start = rank * chunk_size + 2;
    let end = (rank + 1) * chunk_size;
    let start_time = Instant::now();
    let primes = sieve(start, end, &world);
    let end_time = Instant::now();
    let duration = end_time - start_time;

    if rank == 0 {
        println!("Execution time: {} milliseconds", duration.as_millis());
        println!("Number of primes found: {}", primes.len());
    }
}

// compile: rustc mpi_sieve.rs -O
