#include <stdio.h>
#include <cs50.h>

void printHashes(int times);
void printSpace(int times);

int main(void)
{
    int height;
    
    do
    {
        printf ("Height: ");
        height = get_int();
    }
    while (height < 0 || height > 23);
    
    for (int line = 0; line < height; line++) 
    {
        printSpace(height - (line + 1));
        printHashes(line + 1);
        printf("  ");
        printHashes(line + 1);
        printf("\n");
    }
    
}

void printSpace(int times)
{
    for (int i = 0; i < times; i ++)
    {
    printf(" ");
    }
}

void printHashes(int times)
    {
    for (int i = 0; i < times; i ++)
    {
    printf("#");
    }
}