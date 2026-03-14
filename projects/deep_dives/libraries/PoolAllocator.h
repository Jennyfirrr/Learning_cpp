
//======================================================================================================
// [POOL ALLOCATOR]
//======================================================================================================
#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

//======================================================================================================
// [CURRENT ORDER STRUCTURE]
//======================================================================================================
// these are the current structs, theyll probably change but idk, just consider these more like intial jsut to lay ground work, these are almost definitly gonna change now that i think about it lol
//======================================================================================================
typedef struct {
    uint64_t order_id;
    uint64_t price;
    uint64_t quantity;
} CurrentOrder;
static_assert(sizeof(CurrentOrder) == 24, "struct must be 24 bytes");

typedef struct {
    CurrentOrder *slots;
    uint64_t bitmap;
    uint32_t capacity;
} OrderPool;
static_assert(sizeof(OrderPool) == 24, "struct must be 36 bytes");
//======================================================================================================
// [POOL ALLOCATOR FUNCTION PROTOTYPES]
//======================================================================================================
// current working code, subject to chaaanggggeeeee
//======================================================================================================
void OrderPool_init(OrderPool *pool, uint32_t capacity) {
    pool->slots    = (CurrentOrder *)calloc(capacity, sizeof(CurrentOrder));
    pool->bitmap   = 0;
    pool->capacity = capacity;
}

CurrentOrder *OrderPool_Allocate(OrderPool *pool) {
    uint32_t index = __builtin_ctzll(~pool->bitmap);
    pool->bitmap |= (1ULL << index);
    return &pool->slots[index];
}

void OrderPool_Free(OrderPool *pool, CurrentOrder *slot_ptr) {
    uint32_t index = (uint32_t)(slot_ptr - pool->slots);
    pool->bitmap &= ~(1ULL << index);
}

uint32_t OrderPool_CountActive(const OrderPool *pool) {
    uint32_t popcount = __builtin_popcountll(pool->bitmap);
    return popcount;
}
//======================================================================================================
#endif // POOL_ALLOCATOR_H
