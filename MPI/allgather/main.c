#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMS (1)

void gather(char* sbuf, char* rbuf, int root, int world_size, int world_rank) {
    if (world_rank == root) {
        for (int i = 0; i < world_size; ++i) {
            if (i == root) {
                memcpy(rbuf + ELEMS * i, sbuf, ELEMS * sizeof(char));
            } else {
                MPI_Recv(rbuf + ELEMS * i, ELEMS, MPI_CHAR, i, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    } else {
        MPI_Send(sbuf, ELEMS, MPI_CHAR, root, 0, MPI_COMM_WORLD);
    }
}

void allgather(char* sbuf, char* rbuf, int world_size, int world_rank) {
    for (int i = 0; i < world_size; ++i) {
        gather(sbuf, rbuf, i, world_size, world_rank);
    }
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char* sbuf = (char*)malloc(sizeof(*sbuf) * ELEMS);
    char* rbuf = (char*)malloc(sizeof(*rbuf) * ELEMS * world_size);

    memset(sbuf, 'a' + world_rank, ELEMS * sizeof(*sbuf));

    double t = MPI_Wtime();
    allgather(sbuf, rbuf, world_size, world_rank);
    t = MPI_Wtime() - t;

    double total_t;
    MPI_Reduce(&t, &total_t, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        printf("Time = %f\n", total_t);
    }

    printf("[%d] %s\n", world_rank, rbuf);

    free(sbuf);
    free(rbuf);

    MPI_Finalize();
}
