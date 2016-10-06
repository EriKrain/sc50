/**
 * Implements a dictionary's functionality by using a trie
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

/**
 * A structure for keeping "letters"
 */
 
typedef struct node{
    bool isWord; // var for cheking if we get to the end of word
    struct node *children[27]; 
} node;

/**
 * Function and global variables declaration
 */
 
bool unloadDict(node *currentPos, node *previousPos);
node *root; // a pointer for a start of a trie
int wordsLoaded = 0; // counter for words loaded from dictionary

/**
 * Modificators for symbols
 */
 
const int FACTOR_ALPH = 96;
const int FACTOR_APOS = 39;

/**
 * Returns true if word is in dictionary else false.
 */
 
bool check(const char *word)
{
    node *currentPosPtr = root;
    int temp;
    // a temporary storage for lowered copy of a word
    char *wordAll = calloc(strlen(word), sizeof(char) + 1);
    if (wordAll == NULL){
        return false;
    }
    for (int ind = 0, length = strlen(word) + 1; ind < length; ind++){
        wordAll[ind] = word[ind];
        wordAll[ind] = tolower(wordAll[ind]);
        // add 0-symbol
        if (ind == length - 1){
            wordAll[ind] = '\0';
        }
    }
    // iterate over symbols in the word
    for (int ind = 0, length = strlen(wordAll); ind < length; ind++){
        // check if current symbol a letter or an apostroph
        if (wordAll[ind] == '\'')
            temp = (int)wordAll[ind] - FACTOR_APOS; // for apost
        else
            temp = (int)wordAll[ind] - FACTOR_ALPH; // for letters
        // check if current letter isn't the last letter in the word
        if (ind != length - 1){
            // check if there is a child with equal index
            if (currentPosPtr->children[temp] != NULL){
                currentPosPtr = currentPosPtr->children[temp]; // dive in to next level
            }
            else{
                free(wordAll);
                return false; // because there isn't such a letter
            }
        }
        // if current letter is the last letter in the word
        else {
            currentPosPtr = currentPosPtr->children[temp];
            // check if next pointer exists
            if (currentPosPtr == NULL){
                free(wordAll);
                return false;
            }
            // check if we get to the end of the word
            if (currentPosPtr->isWord == true){
                free(wordAll);
                return true;
            }
            else {
                free(wordAll);
                return false;
            }
        }
    }
    free(wordAll);
    return false;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
 
bool load(const char *dictionary)
{
    // open source for reading and check if everything is clear
    FILE *dictionaryPtr = fopen(dictionary, "r");
    if (dictionaryPtr == NULL){
        return false;
    }
    // set address for the root 
    root = calloc(1,sizeof(node)+8);    
    if (root == NULL){
        return false;
    }
    
    unsigned char stor = 0; // a place for storing current symbol
    unsigned char *storage = &stor; 
    node *currentPosPtr; // a pointer for pointing on current place in a trie
    currentPosPtr = root;
    while (fread(storage, 1, 1, dictionaryPtr) != 0){ // condition for non-end of input file
        //check if reached end of the word
        if (*storage == '\n'){
            currentPosPtr->isWord = true; // define the end of the word
            wordsLoaded++; //increase the number of words
            currentPosPtr = root; // return currentPosition to the head of trie
        }
        else{
            int temp; // determine child's index for putting current letter
            // check if current symbol a letter or an apostroph
            if (*storage == '\'')
                temp = (int) *storage - FACTOR_APOS; // for apost
            else
                temp = (int) *storage - FACTOR_ALPH; // for letters
            // check if asked children exist
            if (currentPosPtr->children[temp] != NULL){
                currentPosPtr = currentPosPtr->children[temp]; // go into it - change current position
            }
            else{
                currentPosPtr->children[temp] = calloc(1,sizeof(node)+8); // if not - allocate memory for it
                if (currentPosPtr->children[temp] == NULL){
                    return false;
                }
                currentPosPtr = currentPosPtr->children[temp]; // go into it 
            }
        }
    }
    fclose(dictionaryPtr); 
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
 
unsigned int size(void)
{
    int words = wordsLoaded;
    if (words != 0){
        return words;
    }
    return 0;
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */

bool unload(void)
{
    node *currentPosPtr = root;
    node *previousPosPtr = currentPosPtr;
    // give pointers to function which will unload dictionary
    if (unloadDict(currentPosPtr, previousPosPtr) == true){
       free(root); // at the end on the programm free the root of trie
       return true;
    }
    return false;
}

/**
 * Unloads dictionary 
 */
 
bool unloadDict(node *currentPos, node *previousPos){
    // iterate over the children of currentPosition
    for (int ind = 0; ind < 28; ind++){
        // check if current children was allocated
        if (currentPos->children[ind] != NULL){
            previousPos = currentPos; // save copy of currentPosition before changing
            currentPos = currentPos->children[ind]; // go into allocated child
            // if reached the last child in a chain, free it and zeroise it's pointer  
            if (unloadDict(currentPos, previousPos) == true){
                currentPos = previousPos; // return to the upper level of chain
                free(currentPos->children[ind]); // free the last child
                currentPos->children[ind] = NULL; // zeroise pointer
                continue; // continue iterate over the children
            }
        }
        // if current children wasn't allocated go to next one
        if (ind != 27 && currentPos->children[ind] == NULL){
            continue;
        }
        // get here if all shildren isn't allocated by now and return to the upper level of chain 
        if (currentPos->children[26] == NULL){
            return true;
        }
    }
    return false;
}
