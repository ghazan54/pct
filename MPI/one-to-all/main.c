#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) exit(EXIT_FAILURE);
    memset(ptr, 0, size);
    return ptr;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, commsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    int sizes[2] = {1024, 1024 * 1024};  // 1 Kb и 1 Mb

    for (int i = 0; i < NELEMS(sizes); i++) {
        int size = sizes[i];
        char* sbuf = (char*)xmalloc(size);
        char* rbuf = (char*)xmalloc(size);

        if (rank == 0) {
            double t = MPI_Wtime();
            MPI_Bcast(sbuf, size, MPI_CHAR, 0, MPI_COMM_WORLD);
            t = MPI_Wtime() - t;

            printf("Size = %d, Time = %f\n", size, t);
        } else {
            MPI_Bcast(rbuf, size, MPI_CHAR, 0, MPI_COMM_WORLD);
        }

        free(sbuf);
        free(rbuf);
    }

    MPI_Finalize();
    return 0;
}
