/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */

#include <cs50.h>
#include <stdlib.h>
#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    if (n < 0)
        return false;
    //initialize vars for working with parts of array 
    int lowerBorder = 0;
    int upperBorder = n - 1; 
    int middle = (lowerBorder + upperBorder) / 2;
    //this loop will work until we have values in studied part of array 
    while (lowerBorder <= upperBorder)
    {
        if (values[middle] == value)//checking if we have found our value 
        return true;
        else if (value < values[middle])//change borders of studied part
        {
            upperBorder = middle - 1;
            middle = (lowerBorder + upperBorder) / 2;
        }
        else if (value > values[middle])
        {
            lowerBorder = middle + 1;
            middle = (lowerBorder + upperBorder) / 2;
        }
        else 
            break;
    }
  
return false;
}

/**
 * Sorts array of n values.
 */

//IN DEFAULT PROGRAMM USES LINEAR SORT. FOR USING BUBLSORT, PLEASE, COMMENT THIS SORT FUNCTION AND UNCOMMENT NEXT ONE

void sort(int values[], int n)
{
    int maxValue = values[0];
    //search the max value of array for defining the size of new array
    for (int ind = 1; ind < n; ind++)
    {
        if (maxValue < values[ind])
            maxValue = values[ind];
    }
    //initialize a temporary array (use maaloc cause we have undefined var maxValue)
    int* tempValues = (int*) malloc (sizeof(int) * (maxValue + 1));
    for (int ind = 0; ind < n; ind++)
    {
        //we estimate times of every value's appiarence (in values[]) and change value with index (in tempValues[]) equaled value in values[]
        tempValues[values[ind]]++;    
    }
    //rewrite values[] using tempValues[]
    for (int indTemp = 0, indValues = 0; indTemp < maxValue + 1; indTemp++)
    {
        while (tempValues[indTemp] != 0)//zero means there isn't value equaled index  
        {
            values[indValues] = indTemp;
            indValues++;
            tempValues[indTemp]--;
        }
    }
    free (tempValues);
    return;
}

//BUBLSORT FUNCTION
/*
void sort(int values[], int n)
{
    for (int j = n - 1; j >= 0; j--)
    {
        int change = 0;
        for (int i = 0; i < j ; i++)
        {
            if (values[i] > values[i + 1])
            {
                int temp = values[i];
                values[i] = values[i + 1];
                values[i + 1] = temp;
                change = 1;
            }
        }
        if (change == 0)
            break;
    }
    return;
}
*/
