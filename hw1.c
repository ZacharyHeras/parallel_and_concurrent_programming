/*
Name: Zachary Heras
ID: 916316192
Homework #: 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double get_time(void) {
    struct timeval tval;

    gettimeofday(&tval, NULL);

    return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

int **alloc_board(int P, int Q) {
    int i;
    int *p, **a;

    p = (int *)malloc(P*Q*sizeof(int));
    a = (int **)malloc(P*sizeof(int*));

    if (p == NULL || a == NULL) 
        printf("Error allocating memory\n");

    /* for row major storage */
    for (i = 0; i < P; i++)
        a[i] = &p[i*Q];

    return a;
}

void **init_board(int **a, int mrows, int ncols) {
    int i,j;

    for (i=1; i<mrows-1; i++) {
        for (j=1; j<ncols-1; j++) {
            a[i][j] = (drand48() < 0.5) ? 0 : 1;
        }
    }
}

void **update_ghost_cells(int **a, int mrows, int ncols) {
    int i,j;

    a[0][0] = a[mrows-2][ncols-2];
    a[0][ncols-1] = a[mrows-2][1];
    a[mrows-1][0] = a[1][ncols-2];
    a[mrows-1][ncols-1] = a[1][1];

    for (i=1; i<mrows-1; i++)
        a[i][0] = a[i][ncols-2];

    for (i=1; i<mrows-1; i++)
        a[i][ncols-1] = a[i][1];

    for (j=1; j<ncols-1; j++)
        a[0][j] = a[mrows-2][j];

    for (j=1; j<ncols-1; j++)
        a[mrows-1][j] = a[1][j];
}

int is_alive(int **a, int i, int j) {
    int num_neighbors = 0;

    num_neighbors += a[i-1][j-1] == 1 ? 1 : 0;
    num_neighbors += a[i-1][j]   == 1 ? 1 : 0;
    num_neighbors += a[i-1][j+1] == 1 ? 1 : 0;

    num_neighbors += a[i][j-1] == 1 ? 1 : 0;
    num_neighbors += a[i][j+1] == 1 ? 1 : 0;

    num_neighbors += a[i+1][j-1] == 1 ? 1 : 0;
    num_neighbors += a[i+1][j]   == 1 ? 1 : 0;
    num_neighbors += a[i+1][j+1] == 1 ? 1 : 0;

    // printf("i:%d j:%d neighbors: %d", i, j, num_neighbors);
    // printf("\n");

    if (num_neighbors == 3)
        return 1;

    if (!a[i][j])
        return 0;

    if (num_neighbors == 2)
        return 1;

    return 0;

}

int update_board(int **a, int **b, int mrows, int ncols) {
    int i, j;
    int changed = 0;
    int temp = 0;

    for (i=1; i<mrows-1; i++) { 
        for (j=1; j<ncols-1; j++) {
            b[i][j] = is_alive(a, i, j);
            
            if (a[i][j] != b[i][j])
                changed = 1;
        }
    }

    for (; i<mrows-1; i++) { 
        for (; j<ncols-1; j++) {
            temp = a[i][j];
            b[i][j] = is_alive(a, i, j);
        }
    }

   for (i=1; i<mrows; i++) {
        for (j=1; j<ncols; j++) {
            a[i][j] = b[i][j];
        }
    }

    return changed;
}

void print_board(int **a, int mrows, int ncols) {
    int i,j;
  
    for (i=0; i<mrows; i++) {
        for (j=0; j<ncols; j++)
              printf("%d ", a[i][j]);
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv) 
{
    int N, max_iterations;
    int **a=NULL, **b=NULL;
    double starttime, endtime;

    if (argc != 3) {
        printf("Usage: %s <N> <I>\n", argv[0]);
        exit(-1);
    }
    
    N = atoi(argv[1])+2;
    max_iterations = atoi(argv[2]);
    
    /* Allocate memory for the current board */
    a = alloc_board(N, N);
    b = alloc_board(N, N);

    /* Initialize the game matrix */
    srand48(123456);
    init_board(a, N, N);
    update_ghost_cells(a, N, N);
    // print_board(a, N, N);

    /* Run game loop until termination or max generation count is reached */
    starttime = get_time();

    int terminate = 0;
    int iterations = 0;

    while (!terminate && iterations < max_iterations) {
        terminate = !update_board(a, b, N, N);
        update_ghost_cells(a, N, N);
        // print_board(a, N, N);
        iterations ++;
        printf("%d\n", iterations);
    }

    endtime = get_time();

    /*
    #ifdef DEBUG_PRINT
        printboard(a, N, N);
        printf("\n");
    #endif
    */

    printf("Time taken = %lf seconds\n", endtime-starttime);

    return 0;
}
