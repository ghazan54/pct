#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMS (15)

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

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int root = 0;
    char* sbuf = (char*)calloc(ELEMS, sizeof(*sbuf));
    char* rbuf = NULL;

    if (world_rank == root) {
        rbuf = (char*)calloc(ELEMS, sizeof(*rbuf) * world_size + 1);
    }

    memset(sbuf, 'a' + world_rank, ELEMS * sizeof(*sbuf));

    gather(sbuf, rbuf, root, world_size, world_rank);

    if (world_rank == root) {
        printf("[%d] %s\n", world_rank, rbuf);
    }

    printf("[%d] %s\n", world_rank, sbuf);

    free(sbuf);
    if (rbuf) free(rbuf);

    MPI_Finalize();
}
