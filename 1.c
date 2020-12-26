
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define n 25

int a[n][n], adash[n][n];

void generateMatrix() //used to fill the matrix with random values and write the matix in a file
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        a[i][i] = 0;
        for (j = i + 1; j < n; j++)
        {
            a[i][j] = rand() % 2;
            a[j][i] = a[i][j];
            adash[i][j] = a[i][j];
            adash[j][i] = a[i][j];
        }
    }
}

int main(int argc, char **argv)
{
    int s = 1, d = 5, count = 1;
    int rank;
    int np;
    int c[n][n];
    int i, j, k, sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    generateMatrix();

    if (rank == 0)
    {
        if (s == d)
        {
            printf("Path length: 0\n");
            printf("Time: 0");
            exit(1);
        }
    }
    
    double t1 = clock();
    int n1 = (n * n) / np;
    int start = (n * rank) / np;
    int end = start + (n/np);

    MPI_Barrier(MPI_COMM_WORLD);

    while (a[s - 1][d - 1] == 0 && count <= n)
    {
        if (rank == 0)
            MPI_Scatter(&a[0][0], n1, MPI_INT, MPI_IN_PLACE, n1, MPI_INT, 0, MPI_COMM_WORLD);
        else
            MPI_Scatter(&a[0][0], n1, MPI_INT, &a[start][0], n1, MPI_INT, 0, MPI_COMM_WORLD);
        
        MPI_Bcast(b, n * n, MPI_INT, 0, MPI_COMM_WORLD);
        
        for (i = start; i < end; i++)
        {
            for (j = 0; j < n; j++)
            {
                sum = 0;
                for (k = 0; k < n; k++)
                    sum += (a[i][k] * adash[k][j]);
                c[i][j] = sum;
            }
        }

        if (rank == 0)
            MPI_Gather(MPI_IN_PLACE, n1, MPI_INT, &c[0][0], n1, MPI_INT, 0, MPI_COMM_WORLD);
        else
            MPI_Gather(&c[start][0], n1, MPI_INT, &c[0][0], n1, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                a[i][j] = c[i][j];
        
        MPI_Bcast(a, n * n, MPI_INT, 0, MPI_COMM_WORLD);
        
        count++;
    }

    t1 = clock() - t1;
    if (rank == 0)
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
                printf("%d ", b[i][j]);
            printf("\n");
        }

        if (count >= n)
            printf("Path length: 0\n");
        else
            printf("Path length:%d\n", count);

        printf("Time: %f\n",((double)t1) / CLOCKS_PER_SEC);
    }
    MPI_Finalize();
}