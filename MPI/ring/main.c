#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
// #define SIZE (1)

void xtime_to_file(double t, int size) {
    char filename[MPI_MAX_PROCESSOR_NAME];
    snprintf(filename, sizeof(filename), "%d.dat", size);

    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "File '%s' is not open.\n", filename);
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
    int commsize, rank, len;

    int tag1 = 0, tag2 = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int sizes[] = {1, 1024, 1024 * 1024};
    for (int i = 0; i < NELEMS(sizes); ++i) {
        int size = sizes[i];
        // char inbuf_prev[size], inbuf_next[size], outbuf_prev[size],
        //     outbuf_next[size];

        char* inbuf_prev = (char*)xmalloc(size);
        char* inbuf_next = (char*)xmalloc(size);
        char* outbuf_prev = (char*)xmalloc(size);
        char* outbuf_next = (char*)xmalloc(size);

        MPI_Request reqs[4];
        MPI_Status stats[4];
        int prev = (rank + commsize - 1) % commsize;
        int next = (rank + 1) % commsize;

        init_msg(outbuf_prev, size);
        init_msg(outbuf_next, size);

        double t = MPI_Wtime();

        MPI_Isend(outbuf_prev, size, MPI_CHAR, prev, tag1, MPI_COMM_WORLD,
                  &reqs[0]);
        MPI_Isend(outbuf_next, size, MPI_CHAR, next, tag2, MPI_COMM_WORLD,
                  &reqs[1]);
        MPI_Irecv(inbuf_prev, size, MPI_CHAR, prev, tag2, MPI_COMM_WORLD,
                  &reqs[2]);
        MPI_Irecv(inbuf_next, size, MPI_CHAR, next, tag1, MPI_COMM_WORLD,
                  &reqs[3]);
        MPI_Waitall(4, reqs, stats);

        t = MPI_Wtime() - t;
        // printf("Time = %lf\n", t);

        double total_t;

        MPI_Reduce(&t, &total_t, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("Size = %d, Time = %f\n", size, total_t);
            xtime_to_file(total_t, size);
        }

        free(inbuf_prev);
        free(inbuf_next);
        free(outbuf_prev);
        free(outbuf_next);
    }

    MPI_Finalize();

    return 0;
}
