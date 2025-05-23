/*
Name: Zachary Heras
ID: 916316192
Homework #: 2
To Compile: gcc -fopenmp -o hw2 -DSET_SEED hw2.c
To Run: ./hw2 <N> <I> <T>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int **alloc_board(int P, int Q) {
    int i;
    int *p;
    int **game_board;

    p = (int *)malloc(P*Q*sizeof(int));
    game_board = (int **)malloc(P*sizeof(int*));

    if (p == NULL || game_board == NULL) 
        printf("Error allocating memory\n");

    /* for row major storage */
    for (i = 0; i < P; i++)
        game_board[i] = &p[i*Q];

    return game_board;
}

/*
 * This function randomly initializes each non-ghost
 * cell to a value of 0 for dead or 1 for alive
 */
void init_board(int **game_board, int mrows, int ncols) {
    int i;
    int j;

    #pragma parallel omp for private(j)
    for (i=1; i < mrows-1; i++) {
        for (j=1; j < ncols-1; j++) {
            game_board[i][j] = (drand48() < 0.5) ? 0 : 1;
        }
    }
}

/*
 * This function updates the ghost cells based on the
 * current non-ghost cells
 */
void update_ghost_cells(int **game_board, int mrows, int ncols) {
    int i;
    int j;

    #pragma omp sections
    {

        #pragma omp section
        {
            // These lines update the corner ghost cells
            game_board[0][0] = game_board[mrows-2][ncols-2];
            game_board[0][ncols-1] = game_board[mrows-2][1];
            game_board[mrows-1][0] = game_board[1][ncols-2];
            game_board[mrows-1][ncols-1] = game_board[1][1];
        }

        #pragma omp section
        {
            // This updates the ghost cells in the leftmost column
            for (i=1; i < mrows-1; i++)
                game_board[i][0] = game_board[i][ncols-2];
        }

        #pragma omp section
        {
            // This updates the ghost cells in the rightmost column
            for (i=1; i < mrows-1; i++)
                game_board[i][ncols-1] = game_board[i][1];
        }

        #pragma omp section
        {
            // This updates the ghost cells in the topmost row
            for (j=1; j < ncols-1; j++)
                game_board[0][j] = game_board[mrows-2][j];
        }

        #pragma omp section
        {
            // This updates the ghost cells in the bottommost row
            for (j=1; j < ncols-1; j++)
                game_board[mrows-1][j] = game_board[1][j];
        }
    }
}

/*
 * This function prints the game board
 */
void print_board(int **game_board, int mrows, int ncols) {
    int i;
    int j;
  
    for (i=0; i < mrows; i++) {
        for (j=0; j < ncols; j++)
              printf("%d ", game_board[i][j]);
        printf("\n");
    }

    printf("\n");
}

double get_time(void) {
    struct timeval tval;

    gettimeofday(&tval, NULL);

    return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

/*
 * This function determines if a cell will
 * be alive in the next iteration
 */
int is_alive(int **game_board, int i, int j) {
    int num_neighbors = 0;

    num_neighbors += game_board[i-1][j-1] == 1 ? 1 : 0;
    num_neighbors += game_board[i-1][j]   == 1 ? 1 : 0;
    num_neighbors += game_board[i-1][j+1] == 1 ? 1 : 0;

    num_neighbors += game_board[i][j-1] == 1 ? 1 : 0;
    num_neighbors += game_board[i][j+1] == 1 ? 1 : 0;

    num_neighbors += game_board[i+1][j-1] == 1 ? 1 : 0;
    num_neighbors += game_board[i+1][j]   == 1 ? 1 : 0;
    num_neighbors += game_board[i+1][j+1] == 1 ? 1 : 0;

    if (num_neighbors == 3)
        return 1;

    if (!game_board[i][j])
        return 0;

    if (num_neighbors == 2)
        return 1;

    return 0;

}

/*
 * This function updates each cell's status and returns true
 * if the status of any cell changed
 */
void update_board(int **game_board, int **temp_board, int mrows, int ncols, int *updated) {
    int i;
    int j;

    *updated = 0;

    #pragma omp for private(j)
        for (i=1; i < mrows-1; i++) { 
            for (j=1; j < ncols-1; j++) {
                temp_board[i][j] = is_alive(game_board, i, j);
                
                if (game_board[i][j] != temp_board[i][j]) {
                    #pragma omp critical
                    {
                        *updated = 1;
                    }
                }
            }
        }

    #pragma omp for private(j)
        for (i=1; i < mrows-1; i++) { 
            for (j=1; j < ncols-1; j++) {
                game_board[i][j] = temp_board[i][j];
            }
        }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: %s <N> <I> <T>\n", argv[0]);
        exit(-1);
    }
    
    int N = atoi(argv[1])+2;
    int max_iterations = atoi(argv[2]);
    int thread_count = atoi(argv[3]);
    
    // Allocate memory for the game board
    int **game_board = alloc_board(N, N);

    // Allocate memory for the temp board
    int **temp_board = alloc_board(N, N);

    // seed the random number generator
#ifdef SET_SEED
    srand48(123456);
#endif

    // initialize the game board
    init_board(game_board, N, N);
    update_ghost_cells(game_board, N, N);

    int iterations = 0;

#ifdef DEBUG_PRINT
    printf("%d\n", iterations);
    print_board(game_board, N, N);
    printf("\n");
#endif

    int terminate = 0;
    int updated = 0;
    double starttime = get_time();

    #pragma omp parallel num_threads(thread_count)
    {
        // Run game loop until termination or max generation count is reached
        while (!terminate && iterations < max_iterations) {
            update_board(game_board, temp_board, N, N, &updated);

            terminate = !updated;

            update_ghost_cells(game_board, N, N);

            #pragma omp single
            {
                iterations ++;
            }
            
#ifdef DEBUG_PRINT
            #pragma omp single
            {
                printf("%d\n", iterations);
                print_board(game_board, N, N);
                printf("\n");
            }
#endif
            
        }
    }

    double endtime = get_time();


    printf("Time taken = %lf seconds\n", endtime-starttime);

    return 0;
}
