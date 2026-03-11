//=============================================================================
// [BASIC ORDER HANDLING]
//=============================================================================
// [TAGS] [Pool_Allocator] [Order_Storage] [Data_Stream_Read] [Basic_Decision]
// [Java_Bad] [State_Tracking] [Execution_Harness]
//=============================================================================
// [PLANS AND GOALS]
//=============================================================================
// just gonna try to actually start using the stuff gone over in the other files
// and try to actually create something that isnt just a basic learning script
//=============================================================================
// [INCLUDED]
//=============================================================================
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <random>
#include <x86intrin.h>
//=============================================================================
// [STRUCTS]
//=============================================================================
struct CurrentOrder {
  uint64_t order_bits;
};
static_assert(sizeof(CurrentOrder) == 8, "CurrentOrder should be 8 bytes");

struct OrderPool {
  uint64_t bitmap;
  uint32_t capacity;
  CurrentOrder *slots;
};
static_assert(sizeof(OrderPool) == 24, "OrderPool should be 24 bytes");

struct RiskGateInflow {
  uint8_t lane0;
  uint8_t lane1;
  uint8_t lane2;
  uint8_t lane3;
  uint8_t lane4;
  uint8_t lane5;
  uint8_t lane6;
  uint8_t lane7;
};
static_assert(sizeof(RiskGateInflow) == 8, "RiskGateInflow should be 8 bytes");

struct RiskGateOutflow {

}
//=============================================================================
// [FUNCTIONS]
//=============================================================================
// [Pool_Allocator]
//=============================================================================
void OrderPool_init(OrderPool *pool, uint32_t capacity) {
  pool->slots = (CurrentOrder *)calloc(capacity, sizeof(CurrentOrder));
  pool->bitmap = 0;
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

//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
// [MAIN]
//=============================================================================
//=============================================================================
// [ASM BREAKDOWN]
//=============================================================================
//=============================================================================
