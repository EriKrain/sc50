#include <stdio.h>
#include <string.h>
#include <cs50.h>
#define _XOPEN_SOURCE
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

char *crypt(const char *key, const char *salt);
void checkPass (char pass [], char key [], string inhash);

int main (int argc, char *argv[])
{
    //check corectness of input
    if (argc != 2)
    {
        printf("Wrong input data\n");
        return 1;
    }
    //hash - for storage input hash-pass; pass[] - for making pass; salt[] - for crypt's needs
    const string hash = argv[1];
    char pass[5]; 
    char salt[] = {'5', '0'}; 
    //make array of symb-s used in pass
    static char symb [52];  
    char lower = 'a';
    char upper = 'A';
    int ind = 0; 
    while (lower <= 'z') { 
        for ( ; ind < 26; ) { 
            symb [ind] = lower; 
            lower++; 
            ind ++; 
        }
    }
    while (upper <= 'Z') { 
        for ( ; ind < 52; ) { 
            symb [ind] = upper; 
            upper++; 
            ind ++; 
        }
    }
    
    //printf("We are searching a password...\n");
    //part for cracking (four cycles for changing 1st, 2d, 3d and 4th symb of pass) 
    for (int f1 = 0; f1 < 52; f1++) //in this cycle we check one-symbol passwords
    { 
        pass[0] = symb[f1];
        char pass1[2]= {pass[0]};
        checkPass (pass1, salt, hash);
    
        for (int f2 = 0; f2 < 52; f2 ++) //here we check two-symbol passwords
        { 
            pass[1] = symb[f2];
            char pass2[3]= {pass[0], pass[1]};
            checkPass (pass2, salt, hash);
                    
            for (int f3 = 0; f3 < 52; f3 ++) //here we check three-symbol passwords
            { 
                pass[2] = symb[f3];
                char pass3[4]= {pass[0], pass[1], pass[2]};
                checkPass (pass3, salt, hash);
                
                for (int f4 = 0; f4 < 52; f4++) //here we check four-sumbol passwords
                { 
                    pass[3] = symb[f4];  
                    checkPass (pass, salt, hash);
                }
            }
        }
    }
    printf("smth is going wrong\n");//get an eror-message if smth doesn't work clear
    
    return 0;
}
//function for making hash from our picked pass and checking if it equals input hash
void checkPass (char pass [], char key [], string inhash)
{
    string hashP = crypt (pass, key);
    if ( strcmp(inhash, hashP) == 0)
    { 
        printf("%s\n", pass);
        exit (0);
    }
}

