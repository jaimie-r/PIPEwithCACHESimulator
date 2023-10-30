/**************************************************************************
 * C S 429 system emulator
 * 
 * cache.h - Headers for cache hierarchy used by the emulator. 
 * 
 * Copyright (c) 2021, 2022, 2023.
 * Authors: S. Chatterjee, M. Hinton, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _CACHE_H_
#define _CACHE_H_
#include <stdio.h>
#include <stdbool.h>

/*
 * A possible hierarchy for the cache. The helper functions defined below
 * are based on this cache structure.
 * lru is a counter used to implement LRU replacement policy.
 */

typedef unsigned char byte_t;
typedef long long int word_t;
typedef long long unsigned uword_t;

typedef struct cache_line {
    bool valid;
    uword_t tag;
    bool dirty;
    uword_t lru;
    byte_t *data;
} cache_line_t;

typedef struct cache_set {
    cache_line_t *lines;
} cache_set_t;

typedef struct cache {
    cache_set_t *sets;
    unsigned int A; /* Associativity */
    unsigned int B; /* Bytes per block or line */
    unsigned int C; /* Capacity */
    unsigned int d; /* delay - used as a cache miss penalty */
} cache_t;


typedef enum {
    READ,
    WRITE
} operation_t;

typedef struct {
    bool valid;
    bool dirty;
    uword_t addr;
    byte_t *data;
} evicted_line_t;


cache_t *create_cache(int A_in, int B_in, int C_in, int d_in);
void free_cache(cache_t *cache);
void access_data(cache_t *cache, uword_t addr, operation_t operation);

evicted_line_t *handle_miss(cache_t *cache, uword_t addr, operation_t operation, byte_t *incoming_data);
bool check_hit(cache_t *cache, uword_t addr, operation_t operation);

void get_word_cache(cache_t *cache, uword_t addr, word_t *dest);
void set_word_cache(cache_t *cache, uword_t addr, word_t val);

cache_t *create_checkpoint(cache_t *cache);
void display_set(cache_t *cache, unsigned int set_index);
#endif