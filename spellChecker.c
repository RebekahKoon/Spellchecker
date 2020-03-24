/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Rebekah Koon
 * Date: 11/27/19
 */

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    assert(file != NULL);
    assert(map != NULL);

    char *curWord = nextWord(file);

    /* Putting all words in the file into the map's table with initial value of -1 */
    while (curWord != NULL)
    {
        hashMapPut(map, curWord, -1);

        free(curWord);
        curWord = nextWord(file);
    }
}

/**
 * Calculates the Levenshtein distance between two words in order to determine
 * how similar the two words are to each other. Returns the distance.
 * @param word1: first word for comparison
 * @param word2: second word for comparison
 * @param length1: length of the first word
 * @param length2: length of the second word
 * @return Levenshtein distance between the two words.
 * Sources: https://en.wikipedia.org/wiki/Levenshtein_distance,
 *          https://www.lemoda.net/c/levenshtein/ */
int calculateLevenshteinDist(const char *word1, const char *word2, int length1, int length2)
{
    assert(word1 != NULL);
    assert(word2 != NULL);

    /* matrix will hold the Levenshtein distances between prefixes of word1 and word2 */
    int matrix[length1 + 1][length2 + 1];
    int i,
        j;

    /* Initializing the matrix */
    for (i = 0; i <= length1; i++)
    {
        matrix[i][0] = i;
    }
    for (j = 0; j <=length2; j++)
    {
        matrix[0][j] = j;
    }

    /* Calculating the Levenshtein distance between the two words */
    for (i = 1; i <= length1; i++)
    {
        char curChar1 = word1[i - 1];

        for (j = 1; j <= length2; j++)
        {
            char curChar2 = word2[j - 1];

            /* If current characters are the same, no cost */
            if (curChar1 == curChar2)
            {
                matrix[i][j] = matrix[i - 1][j - 1];
            }

            else
            {
                int delete,
                    insert,
                    substitute,
                    minimum;

                /* Finding the cost to delete, insert, and substitute a character */
                delete = matrix[i - 1][j] + 1;
                insert = matrix[i][j - 1] + 1;
                substitute = matrix[i - 1][j - 1] + 1;
                minimum = delete;

                /* Determining whether deleting, substituting, or inserting costs less */
                if (insert < minimum)
                {
                    minimum = insert;
                }
                if (substitute < minimum)
                {
                    minimum = substitute;
                }

                matrix[i][j] = minimum;
            }
        }
    }

    return matrix[length1][length2];
}

/**
 * Calculates the Levenshtein distance for each word key in the map. Stores
 * this distance as the key's value.
 * @param map: map containing table of words
 * @param inputWord: user's inputted word
 */
void findDistancesForDictionary(HashMap *map, const char *inputWord)
{
    int i,
        distance,
        lengthInputWord,
        lengthCurWord;

    lengthInputWord = strlen(inputWord);

    /* Finding the Levenshtein distance for each value in map's table */
    for (i = 0; i < hashMapCapacity(map); i++)
    {
        HashLink *curLink = map->table[i];

        while (curLink != NULL)
        {
            lengthCurWord = strlen(curLink->key);

            distance = calculateLevenshteinDist(inputWord, curLink->key, lengthInputWord, lengthCurWord);
            hashMapPut(map, curLink->key, distance);

            curLink = curLink->next;
        }
    }
}

/**
 * Finds five of the smallest Levenshtein distances in the hash map.
 * Prints these matches.
 * @param map: Hash map containing words as keys
 */
void findMatches(HashMap *map)
{
    int i,
        j,
        numMatches,
        maxDistance,
        maxDistanceIndex;

    int *curValue;
    const int size = 5;
    const char *matchesKeys[size];

    numMatches = 0;
    maxDistance = 0;
    maxDistanceIndex = 0;

    /* Finding five of the smallest Levenshtein distances */
    for (i = 0; i < hashMapCapacity(map); i++)
    {
        HashLink *curLink = map->table[i];

        while (curLink != NULL)
        {
            /* Filling array of matches if array is not at capacity */
            if (numMatches < size)
            {
                matchesKeys[numMatches] = curLink->key;

                /* Determining the greatest Levenshtein distance in the array */
                if (curLink->value > maxDistance)
                {
                    maxDistance = curLink->value;
                    maxDistanceIndex = numMatches;
                }

                numMatches++;
            }

            else
            {
                /* Determining if curLink is a closer match */
                if (curLink->value < maxDistance)
                {
                    matchesKeys[maxDistanceIndex] = curLink->key;
                    maxDistance = curLink->value;

                    /* Finding the new greatest Levenshtein distance */
                    for (j = 0; j < numMatches; j++)
                    {
                        curValue = hashMapGet(map, matchesKeys[j]);
                        if (maxDistance < *curValue)
                        {
                            maxDistance = *curValue;
                            maxDistanceIndex = j;
                        }
                    }
                }
            }

            curLink = curLink->next;
        }
    }

    /* Printing out five of the closest distances */
    for (i = 0; i < numMatches; i++)
    {
        curValue = hashMapGet(map, matchesKeys[i]);

        if (i == numMatches - 1)
        {
            printf("or %s?\n", matchesKeys[i]);
        }

        else
        {
            printf("%s,\n", matchesKeys[i]);
        }
    }
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..
        /* Source for making string lowercase:
         * https://stackoverflow.com/questions/2661766/how-do-i-lowercase-a-string-in-c */
        int onlyAlpha = 1;

        for (int i = 0; inputBuffer[i]; i++)
        {
            if (!isalpha(inputBuffer[i]))
            {
                onlyAlpha = 0;
            }

            else
            {
                inputBuffer[i] = tolower(inputBuffer[i]);
            }
        }

        if (onlyAlpha == 0)
        {
            printf("Please only enter alphabetic characters and one word.\n");
        }

        else if (strcmp(inputBuffer, "quit") != 0)
        {
            /* If word is spelled correctly and is in dictionary */
            if (hashMapContainsKey(map, inputBuffer))
            {
                printf("The inputted word \"%s\" is spelled correctly.\n", inputBuffer);
            }

            /* Finding five of the closest matches if word is not found */
            else
            {
                printf("The inputted word \"%s\" is spelled incorrectly.\n", inputBuffer);
                printf("Did you mean...\n");
                findDistancesForDictionary(map, inputBuffer);
                findMatches(map);
            }
        }

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}
