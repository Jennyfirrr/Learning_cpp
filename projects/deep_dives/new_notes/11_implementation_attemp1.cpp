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
// [EDIT [11-03-26 05:20am]]
//=============================================================================
// i guess since time stamp is bundled with the data in tick data, and you could
// simply grab that whenever a trade is placed and stuff and because its just
// meta data, you could really just ignore that for the order packing, because
// you arnt really trying to place it at a certain time, just placing it when
// like X condition is specified right?, im probably just gonna do this one with
// a single symbol in mind, so that the "symbol" or ticker field can be ignored,
// idk im not really sure about this tbh, becuase there is alot of information
// that could be left out, and you could simply ignore alot of stuff until the
// actual order is place i think, so really you just need like volume, price,
// and amount, and those can be compacted down using like a set multiplier, or
// an associated left shift probably, because that could be done in a single
// operation
//=============================================================================
// [EDIT [11-03-26 05:55am]]
//=============================================================================
// the OrderMetaData wont be needed for placing orders, its just kind of a place
// holder while i plan out what is actually needed for order tracking and stuff,
// the actual execution stuff will just use the OrderInformation struct for now,
// until i realize it needs something else, this is just the preplanning stage
// really, and im realizing all the gaps from the other files, but those were
// mostly me getting used to this and building a mental model of what all goes
// into it, this way would allow 2 strategies to be ran side by side though
// using SWAR techniques, but its probably bad for actual production use cases,
// idk, ive never worked at an HFT firm before, this stuff is just H E L L A
// interesting
//=============================================================================
struct OrderPrice {
  uint16_t price;
};
static_assert(sizeof(OrderPrice) == 2, "OrderPrice should be 2 bytes");

struct Volume {
  uint16_t volume;
};
static_assert(sizeof(Volume) == 2, "Volume should be 2 bytes");

struct OrderInformation {
  OrderPrice price;
  Volume volume;
};
static_assert(sizeof(OrderInformation) == 4,
              "OrderInformation should be 4 bytes");
//=============================================================================
// [METADATA STRUCTS]
//=============================================================================
// this stuff can be tracked at the same time using a different thread i think,
// so that when an order is added to the orderpool initially, it can just
// identify what happens when so the actual execution isnt using uneeded bloat,
// and allows 2 simaltaneous operations using SWAR, these mainly just exist for
// order tracking for looking at the actions taken during the day, not the
// actualy execution path
//=============================================================================
struct Timestamp {
  uint64_t timestamp;
};
static_assert(sizeof(Timestamp) == 8, "Timestamp should be 8 bytes");

struct Symbol {
  uint16_t symbol;
};
static_assert(sizeof(Symbol) == 2, "Symbol should be 2 bytes");

struct OrderType {
  uint8_t order_type;
};
static_assert(sizeof(OrderType) == 1, "OrderType should be 1 byte");

struct OrderAmount {
  uint16_t order_amount;
};
static_assert(sizeof(OrderAmount) == 2, "OrderAmount should be 2 bytes");

struct Padding {
  uint8_t padding;
};
static_assert(sizeof(Padding) == 1, "Padding should be 1 byte");

struct OrderMetaData {
  uint64_t timestamp;
  uint16_t symbol;
  uint16_t order_amount;
  uint16_t price;
  uint8_t order_type;
  uint8_t padding;
};
static_assert(sizeof(OrderMetaData) == 16, "OrderMetaData should be 16");

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
// im still undecided on these, i feel like im forgetting something, it may be
// better to just use the entire 64 bits for a single order, but idk, im bored
// and experiementing, IDK
//=============================================================================
struct BuyConditions {
  uint32_t condition_0;
  uint32_t condition_1;
};
static_assert(sizeof(BuyConditions) == 8, "BuyConditions should be 8 bytes");

struct SellConditions {
  uint32_t condition_0;
  uint32_t condition_1;
};
static_assert(sizeof(SellConditions) == 8, "SellConditions should be 8 bytes");

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
// [DATA STREAM READING]
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
