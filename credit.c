#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>

long long countEvenPos(long long cardN);
long long countUnevenPos(long long cardN);
long long getCardNumber();
long long getAmount(long long n);
int getTotalSum(int amt, long long cn);
int check(long long cd, int pos);

int main(void)
{
    long long cardNumber = getCardNumber();
    int amount = getAmount (cardNumber);
    int totalSum = getTotalSum(amount, cardNumber);
    
    
    if (totalSum % 10 != 0)
    {
        printf("INVALID\n");
        //printf("%d", totalSum);
    }
    else 
    {
        switch (amount)
        {
            case 13: 
                if (check(cardNumber, 1) == 4)
                {
                    printf("VISA\n");
                    break;
                }
            case 15:
                if (check(cardNumber, 2) == 34 || check(cardNumber, 2) == 37)
                printf("AMEX\n");
                break;
            case 16:
                if (check(cardNumber, 1) == 4)
                {
                    printf("VISA\n");
                    break;
                }
                if (check(cardNumber, 2) == 51 || check(cardNumber, 2) == 52 || check(cardNumber, 2) == 53 || check(cardNumber, 2) == 54 || check(cardNumber, 2) == 55)
                {
                    printf("MASTERCARD\n");
                    break;
                }
            default:
                printf("INVALID\n");
        }
    }
    return 0;
}

long long countEvenPos(long long cardN)
{
    int sum = 0;
    do
    {
        int num = cardN % 10;
        num *= 2;
        if (num / 10 != 0)
        {
            int num1 = num % 10;
            num /= 10;
            num += num1;
        }
        sum += num;
        //printf("SumEven: %d\n", sum);
        cardN /= 100;
    }
    while (cardN != 0);
    return sum;
}

long long countUnevenPos(long long cardN)
{
    int sum = 0;
    do
    {
        int num = cardN % 10;
        sum += num;
        //printf("SumElse: %d\n", sum); 
        cardN /= 100;
    }
    while (cardN != 0);
    return sum;
}



long long getCardNumber()
{
    long long cardNumber;
    printf("Number: ");
    cardNumber = get_long_long();
    if (getAmount(cardNumber) < 13 || getAmount(cardNumber) > 16 || getAmount(cardNumber) == 14)
    {
        printf ("INVALID\n");
        exit(0);
    }
    return cardNumber;
}

long long getAmount(long long n) 
{ 
int amount = 1; 
while ( n /= 10) 
{
    amount++;
}
//printf("Amount: %d\n", amount);
return amount; 
}

int getTotalSum(int amt, long long cn)
{
    int totalSum = 0;
    totalSum = countEvenPos(cn / 10) + countUnevenPos(cn);
    return totalSum;
}

int check(long long cd, int pos)
{
    bool a = true;
    do
    {
        cd /= 10;
        if (pos == 1)
        {
            if (cd / 10 == 0)
            {
                return cd;
                a = false;
            }
        }
        if (pos == 2)
        {
            if (cd / 100 == 0)
            {
                return cd;
                a = false;
            }
        }
    }
    while (true);
}
