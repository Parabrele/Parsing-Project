/****************************************************************************
 * This file defines a generic hash table that includes an inbuilt worklist, as well as
 * some methods: wHashCreate, wHashInsert, wHashFind, wHashPop.

 * IMPORTANT : We did not code this file ourselves, it was given in the subject of the project.
 * We just changed and added some comments to make it more understandable.

 * This will be usefull in the third level of the project, when we will have to
 * explore the state space of the program. The hash table will be used to store
 * the states that have already been explored, and the worklist will be used to
 * store the states that are yet to be explored.

 * The hash table is implemented as an array of linked lists. The number of buckets
 * is always a power of 2. The hash function is expected to return a hash value
 * that is a multiple of the number of buckets. The hash table grows automatically
 * if the fill rate exceeds a certain ratio. The worklist is implemented as a linked
 * list. The hash table is initialized with an empty worklist.
 * This ensures algorithmic considerations with regard to complexity of operations :
 * 1. Insertion and lookup in the hash table is O(1) on average.
 * 2. Insertion and removal from the worklist is O(1).
 *
 * Typical use cases:
 *
 * First, write a function that compares two states using an appropriate metric.
 *
 * int states_compare (wState *state1, wState *state2)
 * {
 *     // '0' if state1 is considered equal to state2
 *     // '-1' if state1 is "less than" state2
 *     // '1' if state1 is "greater than" state2
 *     // "less/greater" indicate any total order
 * }
 *
 * Next, create a hash table and add an initial state:
 *
 * wHash *hash = wHashCreate(states_compare);
 * wState *s = malloc(sizeof(wState));
 * s->memory = ...	// put initial state here
 * s->hash = ...	// some hash function over the memory
 * wHashInsert(hash,s);
 *
 * Finally, explore all reachable states:
 *
 * while ((s = wHashPop(hash))
 * {
 *     // find all states t such that s -> t and insert them:
 *     wHashInsert(hash,t);
 * }
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* This struct represents a program state. Before passing this structure
 * to wHashInsert and wHashFind, it is required to fill the fields - hash and memory
 * appropriately. The fields - 'next' and 'work' are internal to these functions,
 * so they should not be manually manipulated.
 */
typedef struct wState
{
    struct wState *next;	// linked list in the hash
    struct wState *work;	// linked list in the worklist
    unsigned long hash;		// hash value of state goes here
    void *memory;			// information about the state goes here
} wState;


/* This struct represents a hash table*/
typedef struct wHash
{
    wState** buckets;	// an array of lists of states
    wState* worklist;	// a list of states yet to be explored
    int min_buckets;	// minimum number of buckets
    int num_buckets;	// current number of buckets
    int num_entries;	// number of elements in the hash map
    int mask;	    // bitmask; used to position states in the appropriate bucket
    int (*cmp)(wState*,wState*);  // function to compare two states
} wHash;


/***************************************************************************/
/* Generic hash table:							   */

#define WHASH_MIN 1024  // initial number of buckets. Note that it must be a power of 2.


/* Create a new hash table. The table has WHASH_MIN buckets initially, but grows automatically
 * if the fill rate exceeds a certain ratio. Takes a pointer to a user-supplied
 * function as its argument which compares two states and returns -1, 0, 1
 * if the first is smaller-than, equal-to, or larger-than the latter.
 * These comparisons are used by wHashFind and wHashInsert to determine the correct
 * position of new elements added to the hash. The table is initialized with an empty worklist.
*/
extern wHash* wHashCreate(int(*)(wState*,wState*));


/* Insert a state into the hash table. If a copy of the given state is already present
 * in the hash table, the pointer to that state is returned. However, if the given state
 * is indeed new, it is inserted into the hash, and then returned. The 'hash' field of
 * the state passed to it is expected to carry the hash value of the state. If the state
 * is new, it is also enqueued to the worklist of the hash. Finally, a pointer to the
 * state just inserted is returned by the function.
*/
extern wState* wHashInsert(wHash*, wState*);


/* Check whether a given state is within the hash table. If not, return NULL.
 * However, if it is, a pointer to the copy of that state is returned by the function.
*/
extern wState* wHashFind(wHash*, wState*);


/* Remove a state from the worklist and return it. Returns NULL if worklist is empty.
 * Please note that the given state actually remains in the hash table. Its removal
 * is only limited to the worklist created by this hash table.
*/
extern wState* wHashPop(wHash*);


wHash* wHashCreate(int(*cmp)(wState*, wState*))
{
    wHash *table = malloc(sizeof(wHash));

    table->min_buckets = WHASH_MIN;	// initial number of buckets
    table->num_buckets = WHASH_MIN;	// current number of buckets
    table->num_entries = 0;			// number of entries in the hash map
    table->mask = WHASH_MIN-1;		// bitmask used to position states
    table->buckets = calloc(1,WHASH_MIN * sizeof(wState*)); // an array of pointers to state structures
    table->worklist = NULL;			// empty worklist
    table->cmp = cmp;				// function to compare two states and log if they are equal

    return table;
}

wState* wHashInsert(wHash *table, wState *entry)
{
    int i, cmp = 1;
    wState *last_entry = NULL;
    wState *next_entry = table->buckets[entry->hash & table->mask];

    // determine where the new entry should go. When next_entry is null, we arrived at our destination.
    while (next_entry)
    {
        cmp = table->cmp(next_entry,entry);
        if (cmp >= 0) break;
        last_entry = next_entry;
        next_entry = next_entry->next;
    }

    // If an existing copy of the given state is already present, return it.
    if (!cmp) return next_entry;

    // rehash if needed; increase the table size by a factor of 2 and update the bitmask.
	// This is a part of the implementation of the algorithmic consideration mentioned above in this file.
    if (table->num_entries++ >= table->num_buckets * 3/4)
    {
        table->mask = table->num_buckets * 2 - 1;
        table->buckets = realloc(table->buckets,
                                table->num_buckets * 2 * sizeof(void*));

        for (i = 0; i < table->num_buckets; i++)
        {
            wState *l1 = NULL, *l2 = NULL;

            // Split the i-th bucket
			last_entry = table->buckets[i];
            while (last_entry)
            {
                next_entry = last_entry->next;
                if ((last_entry->hash & table->mask)
                    == (unsigned long)i)
                {
                    last_entry->next = l1;
                    l1 = last_entry;
                }
                else
                {
                    last_entry->next = l2;
                    l2 = last_entry;
                }
                last_entry = next_entry;
            }

            // Reverse the order of the list.
			last_entry = NULL;
            while (l1)
            {
                next_entry = l1->next;
                l1->next = last_entry;
                last_entry = l1;
                l1 = next_entry;
            }

            table->buckets[i] = last_entry;
            last_entry = NULL;

            while (l2)
            {
                next_entry = l2->next;
                l2->next = last_entry;
                last_entry = l2;
                l2 = next_entry;
            }

            table->buckets[i + table->num_buckets] = last_entry;
        }

        table->num_buckets *= 2;

        // Redetermine the correct position for the new state just added in the modified bucket of the hash table.
        last_entry = NULL;
        next_entry = table->buckets[entry->hash & table->mask];
        while (next_entry)
        {
            cmp = table->cmp(next_entry,entry);
            if (cmp >= 0) break;
            last_entry = next_entry; next_entry = next_entry->next;
        }
    }

    // insert the new state into the appropriate bucket.
    i = entry->hash & table->mask;

    if (!last_entry)
    {
        entry->next = table->buckets[i];
        table->buckets[i] = entry;
    }
    else
    {
        entry->next = next_entry;
        last_entry->next = entry;
    }

    // finally, add the new entry to worklist
    entry->work = table->worklist;
    table->worklist = entry;

    return entry;
}

wState* wHashFind(wHash *table, wState *entry)
{
    wState *next_entry = table->buckets[entry->hash & table->mask];

    while (next_entry)
    {
        int cmp = table->cmp(next_entry,entry);
        if (cmp > 0) return NULL;
        if (cmp >= 0) return next_entry;
        next_entry = next_entry->next;
    }
    return NULL;
}

wState* wHashPop(wHash* table)
{
    wState* result = table->worklist;

    if (result) table->worklist = table->worklist->work;

    return result;
}
