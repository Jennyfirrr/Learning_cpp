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
// [ORDER INFORMATION STRUCTS]
//=============================================================================
// im probably gonna use some quick hashing method to convert ticker symbols
// into like a single byte or something, that would give the ability to track
// like 256 tickers in a songle byte, which for whatever my purposes are,
// because massive symbol watching has huge inference costs, probably best to
// limit the actual model thats watching the data stream for HFT purposes,
// unless of course your doing it on larger intervals like minute or 5 minute
// decisions, then models trained on larger universes would be more effecient,
// but im not interested in those atm, im also using like 2 bytes for price
// because that should give a large enough number that no single price for an
// individual or fractional stock should ever cap it out, and it can be
// converted to actual trade size later using a simple multiplication
// conversion, which would only add like a few clock cycles at most, and could
// probably actually avoid the main hot path too, because thats just for human
// readability, im not 100% sure about the size, it could be limited to a single
// byte, but the the amount of orders you can send at once is limited to 256,
// unless you use a conversion method downstream, and the ideal here is to pack
// all the needed info into 8 bytes so it all stays in a single register, in a
// single stocks trading pipeline, you could generally avoid the Ticker field,
// and free up a byte for something else, copilot in nvim is SUPER nice too lol,
// way better than vscode(I C K Y)
//=============================================================================
struct OrderID {
  uint8_t ID;
};
static_assert(sizeof(OrderID) == 1, "OrderID should be 1 byte");

struct OrderPrice {
  uint16_t price;
};
static_assert(sizeof(OrderPrice) == 2, "OrderPrice should be 2 bytes");

struct OrderTicker {
  uint8_t ticker;
};
static_assert(sizeof(OrderTicker) == 1, "OrderTicker should be 1 byte");

struct OrderSize {
  int16_t amount;
};
static_assert(sizeof(OrderSize) == 2, "OrderSize should be 2 bytes");

struct OrderInformation {
  OrderID id;
  OrderPrice price;
  OrderTicker ticker;
  OrderSize size;
};
static_assert(sizeof(OrderInformation) == 8,
              "OrderInformation should be 8 bytes");
//=============================================================================
// [ORDER POOL STRUCTS]
//=============================================================================
struct OrderPool {
  uint64_t bitmap;
  uint32_t capacity;
  OrderInformation *slots;
};
static_assert(sizeof(OrderPool) == 24, "OrderPool should be 24 bytes");
//=============================================================================
// [RISK GATE STRUCTS]
//=============================================================================
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
  uint8_t lane0;
  uint8_t lane1;
  uint8_t lane2;
  uint8_t lane3;
  uint8_t lane4;
  uint8_t lane5;
  uint8_t lane6;
  uint8_t lane7;
};
static_assert(sizeof(RiskGateOutflow) == 8,
              "RiskGateOutflow should be 8 bytes");

//=============================================================================
// [FUNCTIONS]
//=============================================================================
// [Pool_Allocator]
//=============================================================================
void OrderPool_init(OrderPool *pool, uint32_t capacity) {
  pool->slots = (OrderInformation *)calloc(capacity, sizeof(OrderInformation));
  pool->bitmap = 0;
  pool->capacity = capacity;
}

OrderInformation *OrderPool_Allocate(OrderPool *pool) {
  uint32_t index = __builtin_ctzll(~pool->bitmap);
  pool->bitmap |= (1ULL << index);
  return &pool->slots[index];
}

void OrderPool_Free(OrderPool *pool, OrderInformation *slot_ptr) {
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
