#include <math.h>
#include <mpi.h>
#include <stdio.h>

double func(double x) { return sin(x + 2) / (0.4 + cos(x)); }

int main(int argc, char **argv) {
    double start_time, end_time;

    int commsize, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double eps = 1E-6;
    int n0 = 1000000;
    int n = n0, k;
    double sq[2] = {0.0, 0.0}, delta = 1;
    double a = -1.0, b = 1.0;

    start_time = MPI_Wtime();

    for (k = 0; delta > eps; n *= 2, k ^= 1) {
        int points_per_proc = n / commsize;
        int lb = rank * points_per_proc;
        int ub = (rank == commsize - 1) ? (n - 1) : (lb + points_per_proc - 1);
        double h = (b - a) / n;
        double s = 0.0;

        for (int i = lb; i <= ub; i++) {
            s += func(a + h * (i + 0.5));
        }

        MPI_Allreduce(&s, &sq[k], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        sq[k] *= h;
        if (n > n0) {
            delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
        }
    }

    end_time = MPI_Wtime();

    double elapsed_time = end_time - start_time;
    double max_elapsed_time;

    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Result: %.12f; Runge rule: EPS %e, n %d\n", sq[k], eps, n / 2);
        printf("Max elapsed time: %f seconds\n", max_elapsed_time);
    }

    MPI_Finalize();
    return 0;
}
