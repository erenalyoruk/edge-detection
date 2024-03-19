# Edge Detection

## Build

### Requirements

- gcc and mpicc
- make

```sh
make all
```

## Run

```sh
# parallel program
mpiexec -n 4 ./bin/mpi assets/aybu.png image_out.png

# sequential program
./bin/seq assets/aybu.png image_out.png
```