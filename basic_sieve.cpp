#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<int> sieve(int n) {
    vector<bool> is_prime(n + 1, true);
    vector<int> primes;
    for (int i = 2; i <= n; i++) {
        if (is_prime[i]) {
            primes.push_back(i);
            for (int j = i * 2; j <= n; j += i) {
                is_prime[j] = false;
            }
        }
    }
    return primes;
}

int main(int argc, char** argv) {
    // read n from command line
    int n;
    if (argc > 1) {
        n = atoi(argv[1]);
    } else {
        n = 100000000;
    }

    auto start_time = high_resolution_clock::now();
    vector<int> primes = sieve(n);
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    cout << "Number of primes found: " << primes.size() << endl;
    return 0;
}

// compile with g++ -std=c++11 -O3 basic_sieve.cpp -o basic_sieve