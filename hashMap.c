/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Rebekah Koon
 * Date: 11/27/19
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    int i;

    /* Freeing all memory in the table */
    for (i = 0; i < map->capacity; i++)
    {
        struct HashLink *curLink = map->table[i];

        /* Going through each key's linked list to delete all values */
        while(curLink != NULL)
        {
            struct HashLink *freeLink = curLink;
            curLink = curLink->next;
            hashLinkDelete(freeLink);
        }

        free(curLink);
    }

    free(map->table);
    map->table = NULL;

    map->capacity = 0;
    map->size = 0;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    /* Adapted from my group's worksheet 37 solution */
    assert(map != NULL);
    assert(key != NULL);

    /* Finding the index where the key should be if in the table */
    int index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
    {
        index += map->capacity;
    }

    struct HashLink *curLink = map->table[index];

    /* Searching for the key in the key's linked list bucket */
    while (curLink != NULL)
    {
        /* If key in table and given key are equal */
        if(strcmp(curLink->key, key) == 0)
        {
            int *value = &curLink->value;
            return value;
        }

        curLink = curLink->next;
    }

    /* If key was not found */
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity) {
    // FIXME: implement
    /* Adapted from my group's worksheet 38 solution */
    assert(map != NULL);
    assert(capacity > 0);

    int i;

    /* Creating new hash map */
    struct HashMap *newMap = hashMapNew(capacity);
    assert(newMap != NULL);

    /* Rehashing all links in old table into the new table */
    for (i = 0; i < hashMapCapacity(map); i++)
    {
        struct HashLink *curLink = map->table[i];

        while (curLink != NULL)
        {
            hashMapPut(newMap, curLink->key, curLink->value);
            curLink = curLink->next;
        }
    }

    hashMapCleanUp(map);

    map->table = newMap->table;
    map->size = newMap->size;
    map->capacity = newMap->capacity;

    free(newMap);
    newMap = NULL;

    assert(map->capacity == capacity);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
    /* Adapted from worksheet 37 group work */
    assert(map != NULL);
    assert(key != NULL);

    struct HashLink *newLink;
    int index;

    /* Determining if table should be resized */
    if (hashMapTableLoad(map) >= MAX_TABLE_LOAD)
    {
        resizeTable(map, map->capacity * 2);
    }

    /* Finding the index of the bucket the key belongs in */
    index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
    {
        index += map->capacity;
    }

    /* Updating value if key already exists */
    if (hashMapContainsKey(map, key))
    {
        int *curVal = hashMapGet(map, key);
        *curVal = value;
        return;
    }
    /* Making new link if key doesn't already exist */
    else
    {
        newLink = hashLinkNew(key, value, map->table[index]);
    }

    map->table[index] = newLink;
    map->size++;
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
    /* Adapted from worksheet 38 group work */
    assert(map != NULL);
    assert(key != NULL);

    int index;

    /* Finding the correct index for the key */
    index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0)
    {
        index += map->capacity;
    }

    struct HashLink *curLink = map->table[index];
    struct HashLink *prevLink = NULL;

    /* Searching the bucket to determine if the key is in the table */
    while (curLink != NULL)
    {
        /* Deleting link if the key was found in the bucket */
        if (strcmp(curLink->key, key) == 0)
        {
            if (prevLink == NULL)
            {
                map->table[index] = curLink->next;
            }

            else
            {
                prevLink->next = curLink->next;
            }

            hashLinkDelete(curLink);
            map->size--;

            return;
        }

        prevLink = curLink;
        curLink = curLink->next;
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);

    /* Finding the index for the key */
    int index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
    {
        index += map->capacity;
    }


    /* Finding the bucket that the key should be in */
    struct HashLink *curLink = map->table[index];

    /* Determining if the key is in the linked list bucket */
    while (curLink != NULL)
    {
        if (strcmp(curLink->key, key) == 0)
        {
            return 1;
        }

        curLink = curLink->next;
    }

    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    int numEmptyBuckets = 0;

    /* Going through table to determine the number of empty buckets */
    for (int i = 0; i < hashMapCapacity(map); i++)
    {
        if (map->table[i] == NULL)
        {
            numEmptyBuckets++;
        }
    }

    return numEmptyBuckets;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    float tableLoad = (float)hashMapSize(map) / (float)hashMapCapacity(map);
    return tableLoad;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXME: implement
   for (int i = 0; i < hashMapCapacity(map); i++)
   {
       struct HashLink *curLink = map->table[i];

       /* Printing all key/value pairs in each bucket */
       printf("\nBucket %d links:", i);
       while (curLink != NULL)
       {
           printf("\nkey: %s, value: %d", curLink->key, curLink->value);
           curLink = curLink->next;
       }

       printf("\n");
   }

   printf("\n");
}
