#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMS (128)

void bcast(char* buf, int root, int world_size, int world_rank) {
    if (world_rank == root) {
        for (int i = 0; i < world_size; ++i) {
            if (i == root) {
                continue;
            }
            MPI_Send(buf, ELEMS, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(buf, ELEMS, MPI_CHAR, root, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int root = 0;
    char* buf = (char*)calloc(ELEMS, sizeof(*buf) + 1);
    // memset(buf, 'a' + world_rank, ELEMS * sizeof(*buf));
    strcpy(buf, "test msg");

    bcast(buf, root, world_size, world_rank);

    printf("[%d] %s\n", world_rank, buf);

    free(buf);

    MPI_Finalize();
}
