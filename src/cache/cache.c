/**************************************************************************
  * C S 429 system emulator
 * 
 * cache.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions, both dirty and clean.  The replacement policy is LRU. 
 *     The cache is a writeback cache. 
 * 
 * Copyright (c) 2021, 2023. 
 * Authors: M. Hinton, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cache.h"

#define ADDRESS_LENGTH 64

//bin/test-csim 1 do 

//test cases w/ respective A B C'
//gdb --args
//bin/csim -t testcases/cache/yi2.trace -A 1 -B 2 -C 4 
//TODO: bin/csim -t testcases/cache/yi.trace -A 2 -B 16 -C 512
//bin/csim -t testcases/cache/yi.trace -A 8 -B 8 -C 64
//bin/csim -t testcases/cache/dave.trace -A 1 -B 16 -C 64
//bin/csim -t testcases/cache/trans.trace -A 1 -B 8 -C 32
//bin/csim -t testcases/cache/trans.trace -A 2 -B 8 -C 64
//bin/csim -t testcases/cache/trans.trace -A 4 -B 8 -C 128
//bin/csim -t testcases/cache/trans.trace -A 1 -B 32 -C 1024
//bin/csim -t testcases/cache/long.trace -A 1 -B 32 -C 1024

/* Counters used to record cache statistics in printSummary().
   test-cache uses these numbers to verify correctness of the cache. */

//Increment when a miss occurs
int miss_count = 0;

//Increment when a hit occurs
int hit_count = 0;

//Increment when a dirty eviction occurs
int dirty_eviction_count = 0;

//Increment when a clean eviction occurs
int clean_eviction_count = 0;

/* STUDENT TO-DO: add more globals, structs, macros if necessary */
uword_t next_lru;

// log base 2 of a number.
// Useful for getting certain cache parameters
static size_t _log(size_t x) {
  size_t result = 0;
  while(x>>=1)  {
    result++;
  }
  return result;
}

/*
 * Initialize the cache according to specified arguments
 * Called by cache-runner so do not modify the function signature
 *
 * The code provided here shows you how to initialize a cache structure
 * defined above. It's not complete and feel free to modify/add code.
 */
cache_t *create_cache(int A_in, int B_in, int C_in, int d_in) {
    /* see cache-runner for the meaning of each argument */
    cache_t *cache = malloc(sizeof(cache_t));
    cache->A = A_in;
    cache->B = B_in;
    cache->C = C_in;
    cache->d = d_in;
    unsigned int S = cache->C / (cache->A * cache->B);

    cache->sets = (cache_set_t*) calloc(S, sizeof(cache_set_t));
    for (unsigned int i = 0; i < S; i++){
        cache->sets[i].lines = (cache_line_t*) calloc(cache->A, sizeof(cache_line_t));
        for (unsigned int j = 0; j < cache->A; j++){
            cache->sets[i].lines[j].valid = 0;
            cache->sets[i].lines[j].tag   = 0;
            cache->sets[i].lines[j].lru   = 0;
            cache->sets[i].lines[j].dirty = 0;
            cache->sets[i].lines[j].data  = calloc(cache->B, sizeof(byte_t));
        }
    }

    // make gcc happy, replace this with zero and comment out _log if you want
    next_lru = _log(0);
    return cache;
}

cache_t *create_checkpoint(cache_t *cache) {
    unsigned int S = (unsigned int) cache->C / (cache->A * cache->B);
    cache_t *copy_cache = malloc(sizeof(cache_t));
    memcpy(copy_cache, cache, sizeof(cache_t));
    copy_cache->sets = (cache_set_t*) calloc(S, sizeof(cache_set_t));
    for (unsigned int i = 0; i < S; i++) {
        copy_cache->sets[i].lines = (cache_line_t*) calloc(cache->A, sizeof(cache_line_t));
        for (unsigned int j = 0; j < cache->A; j++) {
            memcpy(&copy_cache->sets[i].lines[j], &cache->sets[i].lines[j], sizeof(cache_line_t));
            copy_cache->sets[i].lines[j].data = calloc(cache->B, sizeof(byte_t));
            memcpy(copy_cache->sets[i].lines[j].data, cache->sets[i].lines[j].data, sizeof(byte_t));
        }
    }
    
    return copy_cache;
}

void display_set(cache_t *cache, unsigned int set_index) {
    unsigned int S = (unsigned int) cache->C / (cache->A * cache->B);
    if (set_index < S) {
        cache_set_t *set = &cache->sets[set_index];
        for (unsigned int i = 0; i < cache->A; i++) {
            printf ("Valid: %d Tag: %llx Lru: %lld Dirty: %d\n", set->lines[i].valid, 
                set->lines[i].tag, set->lines[i].lru, set->lines[i].dirty);
        }
    } else {
        printf ("Invalid Set %d. 0 <= Set < %d\n", set_index, S);
    }
}

/*
 * Free allocated memory. Feel free to modify it
 */
void free_cache(cache_t *cache) {
    unsigned int S = (unsigned int) cache->C / (cache->A * cache->B);
    for (unsigned int i = 0; i < S; i++){
        for (unsigned int j = 0; j < cache->A; j++) {
            free(cache->sets[i].lines[j].data);
        }
        free(cache->sets[i].lines);
    }
    free(cache->sets);
    free(cache);
}

/* STUDENT TO-DO:
 * Get the line for address contained in the cache
 * On hit, return the cache line holding the address
 * On miss, returns NULL
 */
cache_line_t *get_line(cache_t *cache, uword_t addr) {
    /* your implementation */
    __uint32_t s = _log(cache->C / (cache->A * cache->B));
    size_t b = _log(cache->B);
    __uint32_t t = addr >> (s + b);
    __uint32_t I = (addr >> b) & ((1 << s) - 1);
    cache_set_t *cset = &cache->sets[I];
    for(int i = 0; i < cache->A; i++) {
        if(cset->lines[i].tag == t && cset->lines[i].valid) {
            return &cset->lines[i];
        }
    }
    return NULL;
}

/* STUDENT TO-DO:
 * Select the line to fill with the new cache line
 * Return the cache line selected to filled in by addr
 */
cache_line_t *select_line(cache_t *cache, uword_t addr) {
    /* your implementation */
    __uint32_t S = cache->C / (cache->A * cache->B);
    __uint32_t I = (addr >> _log(cache->B)) & ((1 << _log(S)) - 1);
    uword_t lowest = next_lru;
    cache_set_t *cset = &cache->sets[I];
    cache_line_t *selection;

    for(int i = 0; i < cache->A; i++) {
        if(cset->lines[i].lru <= lowest){
            lowest = cset->lines[i].lru;
            selection = &cset->lines[i]; // check pointers maybe?
        }
    }

    return selection;
}

/*  STUDENT TO-DO:
 *  Check if the address is hit in the cache, updating hit and miss data.
 *  Return true if pos hits in the cache.
 */
bool check_hit(cache_t *cache, uword_t addr, operation_t operation) {
    /* your implementation */
    //call get_line to determine if the line exists or not 
    cache_line_t *line = get_line(cache, addr);
    if(line) {
        line->dirty |= operation == WRITE;
        hit_count++;
        line->lru = next_lru;
        next_lru++;  
        return true;
    } else {
        miss_count++;
        return false;
    }
}

/*  STUDENT TO-DO:
 *  Handles Misses, evicting from the cache if necessary.
 *  Fill out the evicted_line_t struct with info regarding the evicted line.
 */
evicted_line_t *handle_miss(cache_t *cache, uword_t addr, operation_t operation, byte_t *incoming_data) {
    evicted_line_t *evicted_line = malloc(sizeof(evicted_line_t));
    evicted_line->data = (byte_t *) calloc(cache->B, sizeof(byte_t));
    /* your implementation */
    size_t b = _log(cache->B);
    size_t s = _log(cache->C / (cache->A * cache->B));
    __uint32_t t = addr >> (s + b);
    __uint32_t I = (addr >> b) & ((1 << s) - 1);
    bool empty = false;
    //search for empty line 
    cache_set_t *cset = &cache->sets[I];
    cache_line_t *line;
    for(int i = 0; i < cache->A; i++) {
        if(!cset->lines[i].valid) { // found empty line
            empty = true;
            line = &cset->lines[i];
            i = cache->A;
        }
    }
    //cache is full; evict
    if(!empty) {
        line = select_line(cache, addr); 
        //fill in evicted_line info 
        evicted_line->valid = line->valid;
        evicted_line->dirty = line->dirty;
        //reconstruct the address 
        evicted_line->addr = line->tag <<  (s + b);
        evicted_line->addr = evicted_line->addr | (I << b);
        memcpy(evicted_line->data, line->data, (1 << b));
        if(evicted_line->dirty == true){
            dirty_eviction_count++;
        } else {
            clean_eviction_count++;
        }
    }
    //fetch mem block from M to $ + place in avail. line
    //update valid + tag + dirty + lru
    line->valid = true;
    line->dirty = (operation == WRITE);
    if(incoming_data && operation == WRITE){
        line->data = (byte_t *) calloc(cache->B, sizeof(byte_t));
        memcpy(line->data, incoming_data, (1<<b));
    }
    line->tag = t;
    line->lru = next_lru;
    return evicted_line;
}

/* STUDENT TO-DO:
 * Get 8 bytes from the cache and write it to dest.
 * Preconditon: addr is contained within the cache.
 */
void get_word_cache(cache_t *cache, uword_t addr, word_t *dest) {
    /* Your implementation */

    for(int i=0; i<8; i++){
        // uword_t cur = addr + i;
        // cache_line_t *line = get_line(cache, cur);
        // uword_t index = ((1 << cache->B) -1 ) & cur;
        // dest[i] = line->data[index];
    }

}

/* STUDENT TO-DO:
 * Set 8 bytes in the cache to val at pos.
 * Preconditon: addr is contained within the cache.
 */
void set_word_cache(cache_t *cache, uword_t addr, word_t val) {
    /* Your implementation */
    // byte_t *valPtr = (byte_t *)&val;

    for(int i=0; i<8; i++){
        // uword_t cur = addr + i;
        // cache_line_t *line = get_line(cache, cur);
        //dont need to use offset (copying over all 8 bytes)
        // uword_t index = ((1 << cache->B) -1 ) & cur;
        // line->data[index] = valPtr[i];
    }
}

/*
 * Access data at memory address addr
 * If it is already in cache, increase hit_count
 * If it is not in cache, bring it in cache, increase miss count
 * Also increase eviction_count if a line is evicted
 *
 * Called by cache-runner; no need to modify it if you implement
 * check_hit() and handle_miss()
 */
void access_data(cache_t *cache, uword_t addr, operation_t operation)
{
    if(!check_hit(cache, addr, operation))
        free(handle_miss(cache, addr, operation, NULL));
}
