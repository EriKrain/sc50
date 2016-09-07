#include <stdio.h>
#include <cs50.h>

int main(void)
{
    int time;
    
    do 
    {
        printf ("Minutes: ");
        time = get_int();
    }
    while (time <= 0); 
    
    int bottles = time * 12;
    printf("bottles: %i\n", bottles);
    
}