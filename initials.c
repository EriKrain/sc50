#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    string s = get_string();
    if (s != NULL)
    {
        for (int i = 0, n = strlen(s); i < n; i++) 
        {
            if((i == 0 && s[i] != ' ') || ((s[i-1]==' ') && (s[i]!=' ')))
            {
                printf("%c", toupper(s[i]));
            }
        }
        printf("\n");
    }
}  