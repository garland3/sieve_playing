#include <iostream>
#include <cmath>
#include <chrono>
#include <cuda.h>
#include <cuda_runtime.h>
using namespace std;
using namespace std::chrono;

__global__ void sieve_kernel(bool* is_prime, int start, int end) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int step = blockDim.x * gridDim.x;
    for (int i = 2 + tid; i * i <= end; i += step) {
        for (int j = max(2, (start + i - 1) / i) * i; j <= end; j += i) {
            is_prime[j - start] = false;
        }
    }
}

int* sieve(int n, int& num_primes) {
    int chunk_size = 1000000;
    int num_chunks = (n + chunk_size - 1) / chunk_size;
    int* primes = new int[n];
    num_primes = 0;
    for (int i = 0; i < num_chunks; i++) {
        int start = i * chunk_size + 2;
        int end = min(start + chunk_size - 1, n);
        int size = end - start + 1;
        bool* is_prime = new bool[size];
        for (int j = 0; j < size; j++) {
            is_prime[j] = true;
        }

        bool* d_is_prime;
        cudaMalloc(&d_is_prime, size * sizeof(bool));
        cudaMemcpy(d_is_prime, is_prime, size * sizeof(bool), cudaMemcpyHostToDevice);

        int threads_per_block = 256;
        int blocks_per_grid = (size + threads_per_block - 1) / threads_per_block;
        sieve_kernel<<<blocks_per_grid, threads_per_block>>>(d_is_prime, start, end);

        cudaMemcpy(is_prime, d_is_prime, size * sizeof(bool), cudaMemcpyDeviceToHost);
        cudaFree(d_is_prime);

        for (int j = 0; j < size; j++) {
            if (is_prime[j]) {
                primes[num_primes] = start + j;
                num_primes++;
            }
        }
        delete[] is_prime;
    }
    return primes;
}

int main() {
    int n = 100000000;

    auto start_time = high_resolution_clock::now();
    int num_primes;
    int* primes = sieve(n, num_primes);
    auto end_time = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end_time - start_time);
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    cout << "Number of primes found: " << num_primes << endl;

    delete[] primes;

    return 0;
}
