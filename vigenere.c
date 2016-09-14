#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

string fullKey (string pln, string k);

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong input data\n");
        return 1;
    }
    
    string key = argv[1];
    //check if key contains only alphabetical chars
    for (int j = 0, n = strlen(key); j < n; j++ )
    {
        if (key[j] < 'A' || (key[j] > 'Z' && key[j] < 'a') || key[j] > 'z')
        {
            printf("Wrong input data\n");
            return 1;
        }
    }
    //taking plaintext
    string plain = get_string();
    //transform key-chars to lowercase (for simplification)
    for (int j = 0, n = strlen(key); j < n; j++ )
    {
        key[j] = tolower(key[j]);
    }
    
    key = fullKey(plain, key);
    //cipher plaintext
    for (int i = 0, n = strlen(plain), j = 0; i < n; i++)
    {
        if (plain[i] >= 'A' && plain[i] <= 'Z')
        {
            plain[i] = ('A' + (plain[i] - 'A' + (key[j] - 97)) % 26);
            j ++;
            continue;  
        }
        if (plain[i] >= 'a' && plain[i] <= 'z')
        {
            plain[i] = ('a' + (plain[i] - 'a' + (key[j] - 97)) % 26);
            j ++;
            continue;
        }
    }
    printf("%s\n", plain);
}    
//function equalizes length of plaintext and key    
string fullKey (string pln, string k)
{
    if (strlen(k) < strlen(pln))
    {
        k = strcat(k, k);
    }
    else
    {
        return k;
    }
    k = fullKey(pln, k);
    return k;
}
