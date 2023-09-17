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

    int sizes[] = {1024, 1024 * 1024};  // 1 KB и 1 MB

    for (int i = 0; i < NELEMS(sizes); i++) {
        int size = sizes[i];
        char* sbuf = (char*)xmalloc(size);
        char* rbuf = NULL;

        if (rank == 0) {
            rbuf = (char*)xmalloc(size * commsize);
        }

        double t = MPI_Wtime();

        MPI_Gather(sbuf, size, MPI_CHAR, rbuf, size, MPI_CHAR, 0,
                   MPI_COMM_WORLD);

        t = MPI_Wtime() - t;

        double total_t;
        MPI_Reduce(&t, &total_t, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("Size = %d, Time = %f\n", size, total_t);
        }

        free(sbuf);
        if (rbuf) {
            free(rbuf);
        }
    }

    MPI_Finalize();

    return 0;
}
