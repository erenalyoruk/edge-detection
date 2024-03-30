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
mpiexec -n 4 ./bin/mpi assets/aybu.jpg image_out.jpg

# sequential program
./bin/seq assets/aybu.jpg image_out.jpg
```
