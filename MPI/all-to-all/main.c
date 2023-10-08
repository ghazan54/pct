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

void all_to_all(char* sbuf, char* rbuf, int msg_size, int rank, int commsize,
                MPI_Datatype datatype) {
    MPI_Request requests[2 * commsize];
    MPI_Status statuses[2 * commsize];

    for (int i = 0; i < commsize; i++) {
        MPI_Isend(sbuf + i * msg_size, msg_size, datatype, i, 0, MPI_COMM_WORLD,
                  &requests[i]);
        MPI_Irecv(rbuf + i * msg_size, msg_size, datatype, i, 0, MPI_COMM_WORLD,
                  &requests[commsize + i]);
    }

    MPI_Waitall(2 * commsize, requests, statuses);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, commsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    int msg_size = 4;
    MPI_Datatype datatype = MPI_CHAR;

    char* sbuf = (char*)xmalloc(msg_size * commsize * sizeof(*sbuf) + 1);
    char* rbuf = (char*)xmalloc(msg_size * commsize * sizeof(*rbuf) + 1);

    memset(sbuf, rank + 'a', msg_size * commsize);
    memset(rbuf, 0, msg_size * commsize);

    all_to_all(sbuf, rbuf, msg_size, rank, commsize, datatype);

    // printf("[%d] %s\n", rank, sbuf);
    printf("[%d] %s\n", rank, rbuf);

    free(sbuf);
    free(rbuf);

    MPI_Finalize();
    return 0;
}
