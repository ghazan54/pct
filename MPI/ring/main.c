#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

void xtime_to_file(double t, int size) {
    char filename[MPI_MAX_PROCESSOR_NAME];
    snprintf(filename, sizeof(filename), "%d.dat", size);

    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Файл '%s' не удалось открыть.\n", filename);
    }
    fprintf(f, "%f\n", t);
    fclose(f);
}

void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) exit(EXIT_FAILURE);
    return ptr;
}

void init_msg(char* msg, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        msg[i] = 'a';
    }
}

int main(int argc, char** argv) {
    int commsize, rank;

    int tag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int sizes[] = {1, 1024, 1024 * 1024};
    // int sizes[] = {1};
    for (int i = 0; i < NELEMS(sizes); ++i) {
        int size = sizes[i];

        char* inbuf = (char*)xmalloc(size);
        char* outbuf = (char*)xmalloc(size);

        MPI_Request reqs[2];
        MPI_Status stats[2];
        int prev = (rank + commsize - 1) % commsize;
        int next = (rank + 1) % commsize;

        init_msg(outbuf, size);

        double t = MPI_Wtime();

        for (int step = 0; step < commsize - 1; ++step) {
            // printf("P%d, N%d\n", prev, next);
            MPI_Isend(outbuf, size, MPI_CHAR, next, tag, MPI_COMM_WORLD,
                      &reqs[0]);
            MPI_Irecv(inbuf, size, MPI_CHAR, prev, tag, MPI_COMM_WORLD,
                      &reqs[1]);
            MPI_Waitall(2, reqs, stats);

            char* temp = outbuf;
            outbuf = inbuf;
            inbuf = temp;
        }

        t = MPI_Wtime() - t;

        double total_t;
        MPI_Reduce(&t, &total_t, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("Size = %d, Time = %f\n", size, total_t);
            xtime_to_file(total_t, size);
        }

        free(inbuf);
        free(outbuf);
    }

    MPI_Finalize();

    return 0;
}
