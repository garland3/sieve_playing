# mkdir out if it doesn't exist
mkdir -p out
# compile the program
g++ -std=c++11 -O3 basic_sieve.cpp -o  out/basic_sieve
mpic++ -std=c++11 -O3 mpi_sieve.cpp -o out/mpi_sieve
nvcc -O3 cuda_sieve.cu -o out/cuda_sieve
rustc src/rust_sieve.rs -o out/rust_sieve

# run the programs
n=10000000000
echo "Running basic sieve"
./out/basic_sieve $n

cores=$(( $(nproc)-1))
echo "Running MPI sieve on $cores cores"
mpirun -np $cores ./out/mpi_sieve $n

echo "Running CUDA sieve"
./out/cuda_sieve $n

echo "Running Rust sieve"
./out/rust_sieve $n