#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 40
#define COLOUMNS 10
#define MAXBLOCKS ((ROWS + 2) * (COLOUMNS + 2))
#define SECONDWAIT 1
#define MSWAIT 333
#define EMPTYCELL '.'
#define FULLCELL 'O'

int PLAYGAME = 1;

#ifndef __unix__
/*  IF ON WINDOWS :  */
/*--------------------------------------*/
#include <windows.h>
#include <conio.h>
void clearBoard(void)
{
    system("cls");
}
void sleepSeconds(int seconds)
{
    long milliseconds = seconds*1000;
    Sleep(milliseconds);
}
void sleepMilliseconds(long milliseconds)
{
    Sleep(milliseconds);
}
char getch_noblock(void)
{
    char pickedup = EOF;
    if (_kbhit())
    {
        pickedup = _getch();
    }
    return pickedup;
}
/*--------------------------------------*/
#else
/*  IF ON LINUX :  */
/*--------------------------------------*/
#include <termios.h>
#include <unistd.h>
#include <time.h>
void clearBoard(void)
{
    system("clear");
}
void sleepSeconds(int seconds)
{
    sleep(seconds);
}
void sleepMilliseconds(long milliseconds)
{
    long seconds = 0;
    long millisecs = 0;
    if (milliseconds >= 1000)
    {
        seconds = (long)((milliseconds / 1000));
        millisecs = (long)((milliseconds % 1000));
    }
    else
    {
        seconds = 0;
        millisecs = milliseconds;
    }
    long nanoseconds = 1000000 * milliseconds;

    struct timespec sleepytime;
    sleepytime.tv_sec = seconds;
    sleepytime.tv_nsec = nanoseconds;

    nanosleep(&sleepytime, NULL);
}
char getch_noblock(void)
{
    struct termios terminal_current = {};
    int readstatus;
    int readbuffer;

    if (tcgetattr(STDIN_FILENO, &terminal_current) < 0)
    {
        perror("tcgetattr()");
    }

    terminal_current.c_lflag = ((terminal_current.c_lflag) & (~(ICANON | ECHO)));
    terminal_current.c_cc[VMIN] = 0;
    terminal_current.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_current) < 0)
    {
        perror("tcsetattr()");
    }

    readstatus = read(STDIN_FILENO, &readbuffer, 1);
    if (readstatus < 0)
    {
        perror("read()");
    }

    terminal_current.c_lflag = ((terminal_current.c_lflag) | ((ICANON | ECHO)));

    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &terminal_current) < 0)
    {
        perror("tcsetattr()");
    }
    
    if (readstatus > 0)
    {
        return readbuffer;
    }
    else
    {
        return EOF;
    }
}
/*--------------------------------------*/
#endif





typedef bool flag;
typedef struct vec2
{
    int x, y;
} vec2;
typedef struct block
{
    vec2 squares[4];
    bool is_static;
} block;


block all_blocks[MAXBLOCKS] = {};
int blocks_count = 0;

block* add_block(block new_block)
{
    all_blocks[blocks_count] = new_block;
    blocks_count = blocks_count + 1;
    return &all_blocks[(blocks_count-1)];
}

bool checkCollision(block check_block, int check_block_index)
{
    flag will_collide = false;
    for (int other_blocks_index = 0; other_blocks_index < blocks_count; other_blocks_index++)
    {
        if (other_blocks_index == check_block_index)
        {
            continue;
        }
        block second_block = all_blocks[other_blocks_index];
        if (second_block.is_static == false)
        {
            continue;
        }
        
        for (int chk_blk_sqr_indx = 0; chk_blk_sqr_indx < 4; chk_blk_sqr_indx++)
        {
            int main_y = check_block.squares[chk_blk_sqr_indx].y;
            int main_x = check_block.squares[chk_blk_sqr_indx].x;

            for (int scnd_blk_sqr_indx = 0; scnd_blk_sqr_indx < 4; scnd_blk_sqr_indx++)
            {
                int secondary_y = second_block.squares[scnd_blk_sqr_indx].y;
                int secondary_x = second_block.squares[scnd_blk_sqr_indx].x;

                int collission_y = main_y + 1;
                int collission_x = main_x + 0;
                if (collission_y == secondary_y)
                {
                    if (collission_x == secondary_x)
                    {
                        will_collide = true;
                    }
                }

            }
        }

    }
    return will_collide;
}

const vec2 gravity_force = {0, 1};

void gravity(void)
{
    for (int block_index = 0; block_index < blocks_count; block_index += 1)
    {
        block current_block = all_blocks[block_index];
        if (current_block.is_static == true)
        {
            continue;
        }
        bool will_collide = checkCollision(current_block, block_index);
        if (will_collide == true)
        {
            all_blocks[block_index].is_static = true;
            //printf("a block is now static\n");
        }
        else
        {
            for(int square_i = 0; square_i < 4; square_i += 1)
            {
                all_blocks[block_index].squares[square_i].x += gravity_force.x;
                all_blocks[block_index].squares[square_i].y += gravity_force.y;
            }
        }
    }
}

void ProcessKeys(char keyboardpress)
{
    if (keyboardpress == 'Q' | keyboardpress == 'q' | keyboardpress == 27)
    {
        if (keyboardpress == 27)
        {
            char nextkey = getch_noblock();
            if (nextkey != EOF)
            {
                return;
            }
        }
        printf("END\n");
        PLAYGAME = 0;
    }
    else if (keyboardpress == 'S' | keyboardpress == 's')
    {
        printf("DOWN\n");
    }
    else if (keyboardpress == 'A' | keyboardpress == 'a')
    {
        printf("LEFT\n");
    }
    else if (keyboardpress == 'D' | keyboardpress == 'd')
    {
        printf("RIGHT\n");
    }
    else if (keyboardpress == ' ' | keyboardpress == '\n' | keyboardpress == '\r')
    {
        printf("SMASH DOWN\n");
    }
    else
    {
        printf("NOT RECOGNIZED : %d\n", keyboardpress);
    }
}



void draw(void)
{
    int cells = ROWS * (COLOUMNS+1);
    char board[cells];
    memset(board, 0, sizeof(board));
    int cell_i = 0;
    for(int row_i=0; row_i<ROWS; row_i++)
    {
        for(int coloumn_i=0; coloumn_i<COLOUMNS; coloumn_i++)
        {
            char current_cell = EMPTYCELL;
            flag is_full = false;
            for(int checking_block_index=0; checking_block_index<blocks_count;checking_block_index+=1)
            {
                block current_checking_block = all_blocks[checking_block_index];
                for (int checking_square_index=0; checking_square_index<4; checking_square_index+=1)
                {
                    vec2 checking_pos = current_checking_block.squares[checking_square_index];

                    if ((checking_pos.x == coloumn_i) && (checking_pos.y == row_i))
                    {
                        is_full = true;
                        break;
                    }
                }
                if (is_full)
                {
                    break;
                }
            }
            if (is_full)
            {
                current_cell = FULLCELL;
            }
            board[cell_i] = current_cell;
            cell_i += 1;
        }
        board[cell_i] = '\n';
        cell_i += 1;
    } 
    printf("%s\n", board);
}

void gameLoop(void)
{
    // int game_cycle = 200;

    block floor_block_1, floor_block_2, floor_block_3;
    
    floor_block_1.squares[0].x = 0; floor_block_1.squares[0].y = ROWS;
    floor_block_1.squares[1].x = 1; floor_block_1.squares[1].y = ROWS;
    floor_block_1.squares[2].x = 2; floor_block_1.squares[2].y = ROWS;
    floor_block_1.squares[3].x = 3; floor_block_1.squares[3].y = ROWS;
    
    floor_block_2.squares[0].x = 4; floor_block_2.squares[0].y = ROWS;
    floor_block_2.squares[1].x = 5; floor_block_2.squares[1].y = ROWS;
    floor_block_2.squares[2].x = 6; floor_block_2.squares[2].y = ROWS;
    floor_block_2.squares[3].x = 7; floor_block_2.squares[3].y = ROWS;
    
    floor_block_3.squares[0].x = 8; floor_block_3.squares[0].y = ROWS;
    floor_block_3.squares[1].x = 9; floor_block_3.squares[1].y = ROWS;
    floor_block_3.squares[2].x = 10; floor_block_3.squares[2].y = ROWS;
    floor_block_3.squares[3].x = 11; floor_block_3.squares[3].y = ROWS;

    floor_block_1.is_static = true;
    floor_block_2.is_static = true;
    floor_block_3.is_static = true;

    block* last_block;
    last_block = add_block(floor_block_1);
    last_block = add_block(floor_block_2);
    last_block = add_block(floor_block_3);

    int frequency = 6;
    do
    {
        draw();
        gravity();
        // game_cycle -= 1;
        block last_block_copy = (*last_block);
        if (last_block_copy.is_static == true)
        {
            if (blocks_count < (MAXBLOCKS-5))
            {
                block new_block;
                new_block.squares[0].x = 0; new_block.squares[0].y = 1;
                new_block.squares[1].x = 1; new_block.squares[1].y = 1;
                new_block.squares[2].x = 1; new_block.squares[2].y = 0;
                new_block.squares[3].x = 2; new_block.squares[3].y = 0;
                new_block.is_static = false;
                last_block = add_block(new_block);
            }
            else
            {
                printf("NO MORE BLOCKS, MAX REACHED\n");
            }
        }
        char keypress = EOF;
        keypress = getch_noblock();
        ProcessKeys(keypress);
        sleepMilliseconds(MSWAIT);
        clearBoard();
    }
    while(PLAYGAME == 1);
    clearBoard();
    draw();
}

void endGame(void)
{
    PLAYGAME = 0;
}

int main(int argc, char** argv)
{
    printf("START\n");
    gameLoop();
    printf("END\n");
    return 0;
}
