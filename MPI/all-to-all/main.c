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

    int msg_size = 1024;
    MPI_Datatype datatype = MPI_CHAR;

    char* sbuf = (char*)malloc(msg_size * commsize * sizeof(char));
    char* rbuf = (char*)malloc(msg_size * commsize * sizeof(char));

    memset(sbuf, rank + 'a', msg_size * commsize);

    MPI_Request send_request[commsize];
    MPI_Request recv_request[commsize];
    MPI_Status status[commsize];

    double t = MPI_Wtime();

    for (int i = 0; i < commsize; i++) {
        MPI_Isend(sbuf + i * msg_size, msg_size, datatype, i, 0, MPI_COMM_WORLD,
                  &send_request[i]);
        MPI_Irecv(rbuf + i * msg_size, msg_size, datatype, i, 0, MPI_COMM_WORLD,
                  &recv_request[i]);
    }

    MPI_Waitall(commsize, send_request, status);
    MPI_Waitall(commsize, recv_request, status);

    t = MPI_Wtime() - t;

    double total_t;
    MPI_Reduce(&t, &total_t, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Size = %d, Time = %f\n", msg_size, total_t);
    }

    // printf("[%d] %s\n", rank, sbuf);
    // printf("[%d] %s\n", rank, rbuf);

    free(sbuf);
    free(rbuf);

    MPI_Finalize();
    return 0;
}
