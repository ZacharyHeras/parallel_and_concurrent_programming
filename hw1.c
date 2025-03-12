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

int **init_board(int **a, int mrows, int ncols) {
    int i,j;

    for (i=1; i<mrows-1; i++) {
        for (j=1; j<ncols-1; j++) {
            a[i][j] = (drand48() < 0.5) ? 0 : 1;
        }
    }

    return a;
}

int **update_ghost_cells(int **a, int mrows, int ncols) {
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

    return a;
}

int **update_board(int **a, int mrows, int ncols) {
    int i, j;

    for (i=1; i<mrows-1; i++) {
        for (j=1; j<ncols-1; j++) {
            a[i][j] = isAlive(a, i, j);
        }
    }

    return a;
}

int is_alive(int **a, int i, int j) {
    
}

void print_board(int **a, int mrows, int ncols) {
    int i,j;
  
    for (i=0; i<mrows; i++) {
        for (j=0; j<ncols; j++)
              printf("%d ", a[i][j]);
        printf("\n");
    }
}

int main(int argc, char **argv) 
{
    int N, i, j, k;
    int **a=NULL, **b=NULL;
    double starttime, endtime;

    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        exit(-1);
    }
    
    N = atoi(argv[1])+2;
    
    /* Allocate memory for the current board */
    a = alloc_board(N, N);

    /* Allocate memory for the next board */
    b = alloc_board(N, N);
    
    /* Initialize the game matrix */
    srand48(123456);
    a = init_board(a, N, N);
    a = update_ghost_cells(a, N, N);

    /* Run game loop until termination or max generation count is reached */
    starttime = get_time();

    int terminate = 1;

    while (!terminate) {

        // game logic
        // update function
        // compare function
        // set terminate
        // printboard(a, N, N);

    }

    print_board(a, N, N);


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
