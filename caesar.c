#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    //checking right args
    if (argc != 2)
    {
        printf ("You forgot to enter a key\n");
        return 1;
    }
    //convert char to int
    const int key = atoi(argv[1]);
    //asking for plaintext 
    printf("plaintext: ");
    string plain = get_string();
    //iterate over the characters of the plaintext
    for (int i = 0, n = strlen(plain); i < n; i ++)
    {
        //change chars according to lowercase or uppercase and key
        if (plain[i] >= 'A' && plain[i] <= 'Z')
        {
            plain[i]=('A' + (plain[i]-'A' + key) % 26 ); 
            continue;
        }
        if (plain[i] >= 'a' && plain[i] <= 'z')
        {
            plain[i]=('a' + (plain[i]-'a' + key) % 26 ); 
            continue;
        }
    }
    printf("ciphertext: %s\n", plain);
    return 0;
}

