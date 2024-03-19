CC := gcc
MPICC := mpicc

CFLAGS := -Wall -Wextra -isystem include -Wno-unused-parameter
LDFLAGS :=
LDLIBS := -lm

all: build

build: bin/mpi bin/seq

bin/mpi: out/mpi_main.o out/sobel.o
	@mkdir -p bin
	$(MPICC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

bin/seq: out/seq_main.o out/sobel.o
	@mkdir -p bin
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

out/sobel.o: src/sobel.c
	@mkdir -p out
	$(CC) $(CFLAGS) -c $^ -o $@

out/mpi_main.o: src/mpi_main.c
	@mkdir -p out
	$(MPICC) $(CFLAGS) -c $^ -o $@

out/seq_main.o: src/seq_main.c
	@mkdir -p out
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf bin out