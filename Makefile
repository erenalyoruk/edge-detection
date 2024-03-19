CC := gcc
MPICC := mpicc

CFLAGS := -Wall -Wextra -isystem include
LDFLAGS :=
LDLIBS := -lm

all: build
	cp assets/* bin/

build: bin/mpi bin/seq

bin/mpi: out/mpi_main.o
	@mkdir -p bin
	$(MPICC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

bin/seq: out/seq_main.o
	@mkdir -p bin
	$(MPICC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

out/mpi_main.o: src/mpi_main.c
	@mkdir -p out
	$(MPICC) $(CFLAGS) -c $^ -o $@

out/seq_main.o: src/seq_main.c
	@mkdir -p out
	$(MPICC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf bin out