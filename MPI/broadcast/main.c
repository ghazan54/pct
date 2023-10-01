#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMS (1024)

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
    char* buf = (char*)malloc(sizeof(*buf) * ELEMS);
    strcpy(buf, "test msg");

    double t = MPI_Wtime();
    bcast(buf, root, world_size, world_rank);
    t = MPI_Wtime() - t;

    double total_t;
    MPI_Reduce(&t, &total_t, 1, MPI_DOUBLE, MPI_MAX, root, MPI_COMM_WORLD);

    if (world_rank == root) {
        printf("Time = %f\n", total_t);
    }

    printf("[%d] %s\n", world_rank, buf);

    free(buf);

    MPI_Finalize();
}
