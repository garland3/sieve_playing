#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<int> sieve(int start, int end) {
    int size = end - start + 1;
    vector<bool> is_prime(size, true);
    for (int i = 2; i * i <= end; i++) {
        for (int j = max(2, (start + i - 1) / i) * i; j <= end; j += i) {
            is_prime[j - start] = false;
        }
    }
    vector<int> primes;
    for (int i = 0; i < size; i++) {
        if (is_prime[i]) {
            primes.push_back(start + i);
        }
    }
    return primes;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100000000;
    int chunk_size = (n + size - 1) / size;
    int start = rank * chunk_size + 2;
    int end = min(start + chunk_size - 1, n);
    if (rank == 0) {
        start = 2;
    }

    auto start_time = high_resolution_clock::now();
    vector<int> local_primes = sieve(start, end);
    auto end_time = high_resolution_clock::now();

    if (rank != 0) {
        int count = local_primes.size();
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&local_primes[0], count, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        vector<int> primes = local_primes;
        for (int i = 1; i < size; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<int> buffer(count);
            MPI_Recv(&buffer[0], count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            primes.insert(primes.end(), buffer.begin(), buffer.end());
        }
        auto duration = duration_cast<milliseconds>(end_time - start_time);
        cout << "Execution time: " << duration.count() << " milliseconds" << endl;
        cout << "Number of primes found: " << primes.size() << endl;
    }

    MPI_Finalize();
    return 0;
}
// compile with mpic++ -std=c++11 -O3 mpi_sieve.cpp -o mpi_sieve
// run with mpirun -np 8 ./mpi_sieve