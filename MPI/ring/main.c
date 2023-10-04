#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

void* xmalloc(size_t size) {
    void* ptr = calloc(size + 1, sizeof(char));
    if (!ptr) exit(EXIT_FAILURE);
    return ptr;
}

int main(int argc, char** argv) {
    int commsize, rank;

    int tag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // int sizes[] = {1, 1024, 1024 * 1024};
    int sizes[] = {2};
    for (int i = 0; i < NELEMS(sizes); ++i) {
        int size = sizes[i];

        char* buf = (char*)xmalloc(size * commsize);

        memset(buf + rank * size, 'a' + rank, size);

        MPI_Request reqs[2];
        MPI_Status stats[2];

        int prev = (rank - 1 + commsize) % commsize;
        int next = (rank + 1) % commsize;

        for (int step = 0; step < commsize - 1; ++step) {
            // printf("[%d] R%d, S%d\n", rank, recv_rank, send_rank);
            int sendblock = (rank - step + commsize) % commsize;
            int recvblock = (rank - step - 1 + commsize) % commsize;
            MPI_Isend(buf + sendblock * size, size, MPI_CHAR, next, tag, MPI_COMM_WORLD,
                      &reqs[0]);
            MPI_Irecv(buf + recvblock * size, size, MPI_CHAR, prev, tag, MPI_COMM_WORLD,
                      &reqs[1]);
            MPI_Waitall(2, reqs, stats);
        }

        printf("[%d] %s\n", rank, buf);

        free(buf);
    }

    MPI_Finalize();

    return 0;
}
