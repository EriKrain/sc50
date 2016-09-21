/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);
bool checkMove (int li, int po, int keyL, int keyP);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE *file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    int val = d*d - 1;//this code starts to full board with max allowable number
    for (int line = 0; line < d; line ++)
    {
        for (int pos = 0; pos < d; pos ++)
        {
            board[line][pos] = val;//put number on required position
            val--;//decreasing to nest number 
        }
        if (d % 2 == 0)//after having board done, swapping 1 and 2 if we have odd number of tiles  
        {
            int temp = board[d - 1][d - 2];
            board[d - 1][d - 2] = board[d - 1][d - 3];
            board[d - 1][d - 3] = temp;
        }
    }
    return;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    printf("\n");
    for (int line = 0; line < d; line ++)
    {
        for (int pos = 0; pos < d; pos ++)
        {
            if (board[line][pos] == 0)
                printf("    _");
            else
                printf("%5d", board[line][pos]);
        }
        printf("\n\n");
    }
    return;
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    for (int line = 0; line < d; line ++)
    {
        for (int pos = 0; pos < d; pos ++)
        {
            if (board[line][pos] == tile)
            {
                if (board[line + 1][pos] == 0 && line + 1 < d)
                {    
                    checkMove (line, pos, 1, 0);
                    return true;
                }
                
                if (board[line - 1][pos] == 0 && line - 1 >= 0)
                {   
                    checkMove (line, pos, -1, 0);
                    return true;
                }
                
                if (board[line][pos + 1] == 0 && pos + 1 < d)
                {    
                    checkMove (line, pos, 0, 1);
                    return true;
                }
                
                if (board[line][pos - 1] == 0 && pos - 1 >= 0)
                {  
                    checkMove (line, pos, 0, -1);
                    return true;
                }
                else
                    return false;
            }
        }
    }
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    int check = 0;
    //this part of code checks every position in board and it's value
    for (int line = 0; line < d; line ++)
    {
        for (int pos = 0; pos < d; pos ++)
        {
            if (board[line][pos] == pos + 1 + line * d)//if value is required (according to formula) increase check
                check++;
        }
    }
    if (check == d * d - 1)
        return true;//function return true if we have required value of check
    else
        return false;
}

bool checkMove (int li, int po, int keyL, int keyP)
{
    int temp = board[li][po];
    board[li][po] = 0;
    board[li + keyL][po + keyP ] = temp;
    return true;
}
