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
  uint32_t price;
};
static_assert(sizeof(OrderPrice) == 4, "OrderPrice should be 4 bytes");

struct Volume {
  uint32_t volume;
};
static_assert(sizeof(Volume) == 4, "Volume should be 4 bytes");

struct OrderInformation {
  OrderPrice price;
  Volume volume;
};
static_assert(sizeof(OrderInformation) == 8,
              "OrderInformation should be 8 bytes");
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
//=============================================================================
// im just gonna have this use a hashID based on the symbol, so it has like 65k
// availble options or whatever because its 2 bytes
//=============================================================================

struct OrderType {
  uint8_t order_type;
};
static_assert(sizeof(OrderType) == 1, "OrderType should be 1 byte");

struct OrderAmount {
  uint16_t order_amount;
};
static_assert(sizeof(OrderAmount) == 2, "OrderAmount should be 2 bytes");
//=============================================================================
// i may compact this to a single byte and just have something else to encode
// vaalues using a dynamic multiplier idk
//=============================================================================

struct Padding {
  uint8_t padding;
};
static_assert(sizeof(Padding) == 1, "Padding should be 1 byte");

struct OrderMetaData {
  uint64_t timestamp;
  uint32_t order_amount;
  uint32_t price;
  uint16_t symbol;
  uint8_t order_type;
  uint8_t padding;
};
static_assert(sizeof(OrderMetaData) == 24, "OrderMetaData should be 16");

//=============================================================================
// [ORDER POOL STRUCTS]
//=============================================================================
struct OrderPool {
  uint64_t bitmap;
  uint32_t capacity;
  uint32_t count;
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
struct BuySideGateConditions {
  uint32_t price;
  uint32_t volume;
};
static_assert(sizeof(BuySideGateConditions) == 8, "BuySideGateConditions");

struct SellSideGateConditions {
  uint32_t price;
  uint32_t volume;
};
static_assert(sizeof(SellSideGateConditions) == 8, "SellSideGateConditions");

struct SellGateBuilt {
  uint64_t packed_conditions_sell;
};
static_assert(sizeof(SellGateBuilt) == 8, "SellGateBuilt should best 8 bytes");

struct BuyGateBuilt {
  uint64_t packed_conditions_buy;
};
static_assert(sizeof(BuyGateBuilt) == 8, "BuyGateBuilt should be 8 bytes");

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
// [CONDITIONS]
//============================================================================
// standard build conditions, i probably need a more robust way to split off
// which side actually gets added to the orderpool, probably like 4 check
// functions, with each one outputting a different OrderInformation struct, so
// it may actually be bad to use SWAR techniques, im not really sure
//=============================================================================
BuyGateBuilt build_buy_conditions(BuySideGateConditions *conditions) {
  uint64_t packed_conditions = 0;
  packed_conditions |= conditions->price;
  packed_conditions |= ((uint64_t)conditions->volume << 32);
  return {packed_conditions};
}

SellGateBuilt build_sell_conditions(SellSideGateConditions *conditions) {
  uint64_t packed_conditions = 0;
  packed_conditions |= conditions->price;
  packed_conditions |= ((uint64_t)conditions->volume << 32);
  return {packed_conditions};
}

//=============================================================================
// [DATA STREAM READING]
//=============================================================================
// the bool being returned probably isnt necessary, too many jumps in the asm
// code, the actual order information check below it has no jump but i think it
// could be optimized further, this needs to add the order to the orderpool i
// think, and should probably be a void, because its not returning anything its
// just watching the data stream to see if an order should be added
//=============================================================================
// [EDIT [11-03-26 11:50pm]]
//=============================================================================
// has 2 jumps so ITS BAD, im gonna have to figure out a mask to use for this,
// so it does it wihtout  emitting 2 branches, this way has no jumps, just
// always try to add to the pool, and add based of the 1 or 0 emitted by the
// pass variable, so if its 0 it doesnt add, just like i wrote before but forgot
// because idk, these both have 0 jumps but theyre still like 18 instructions
// each, it cacn probably be optimized further idk, good start though removing
// the jumps i guess, and its not java(I C K Y)
//=============================================================================
// [EDIT [12-03-26 12:25am]]
//=============================================================================
// this has major issues, im trying to think this through, but the idea was to
// track active buy orders in a pool allocator and then remove them using the
// sell gate, it may be bad deisgn but who knows, ive never actually seen the
// internals of an HFT firm, so im just kinda E X P E R I M E N T I N G, i may
// sleep on this or like just do something else and come back to it idk, this
// would need like a for loop, beacuse it would just build up all the same buy
// orders and stuff, without really having a way to implement a strategy, but
// these are extremely basic, im sure there are normaliziation practices to use,
// but there would need something like a linear regression function to actually
// work with a certain regime, unless you could normalize the data to work with
// a flat slope, otherwise this wouldnt work, this also ties back into where i
// mentioned that a model would basically update the risk gates depending on
// overall patterns for a given time interval, but thats like, a little ways
// off, its probably dumb as hell to make this stuff OSS, but who knows maybe
// someone will hire me
//=============================================================================
// [EDIT [12-03-26 04:11am]]
//=============================================================================
// so i made some updates for actual order tracking, im probably gonna sway the
// order pool thing to be a .h file at some point because ive seen people doing
// that stuff, but for now i like to have all the logic in the file, its easier
// for me to see how it all works, but this is a rough check for order tracking
// and only seeling currently held orders, this has 2 jumps, i thought it would
// only have 1, so im gonna have to investigate this, the buy side is fine, it
// doesnt have any jump, and i expected the sell side to have a single jump but
// it has 2?
//
// so apparently the reason the 2 jumps in the sell side function are acceptable
// are because its data dependent, and not a branch prediciton, because it exits
// early if there is nothing in the pool, and simply does a while loop while the
// pool has things in it, apparently this can be avoided using using popcountll
// to get the iteration count upfront, and use a counted for loop, but this
// shouldnt introduce branch predcition paths like and if/else jump would cause,
// so it probably doesnt introduce problematic latency variance
//=============================================================================
struct DataStream {
  uint32_t price;
  uint32_t volume;
};
static_assert(sizeof(DataStream) == 8, "DataStream should be 8 bytes");

struct ProfitTarget {
  uint32_t profit_target;
};
static_assert(sizeof(ProfitTarget) == 4, "ProfitTarget should be 4 bytes");

void check_buy_lane0(const BuyGateBuilt *packed_conditions,
                     const DataStream *stream, OrderPool *pool) {
  uint32_t price = stream->price;
  uint32_t volume = stream->volume;

  uint32_t price_pass =
      price <= (packed_conditions->packed_conditions_buy & 0xFFFFFFFF);
  uint32_t volume_pass =
      volume >= (packed_conditions->packed_conditions_buy >> 32);

  uint32_t pass = price_pass & volume_pass;

  uint64_t mask = (uint64_t)(-(int64_t)pass);
  uint32_t index = __builtin_ctzll(~pool->bitmap);
  pool->bitmap |= (mask & (1ULL << index));
  pool->slots[index].price.price = price;
  pool->slots[index].volume.volume = volume;
}

void check_sell_lane0(const SellGateBuilt *packed_conditions,
                      DataStream *stream, OrderPool *pool,
                      const ProfitTarget *profit_target_struct) {
  uint32_t price = stream->price;
  uint32_t volume = stream->volume;

  uint32_t price_pass =
      price >= (packed_conditions->packed_conditions_sell & 0xFFFFFFFF);
  uint32_t volume_pass =
      volume <= (packed_conditions->packed_conditions_sell >> 32);

  uint32_t pass = price_pass & volume_pass;

  uint64_t active = pool->bitmap;
  while (active) {
    uint32_t idx = __builtin_ctzll(active);
    uint32_t entry_price = pool->slots[idx].price.price;
    uint32_t exit_pass =
        (price >= entry_price + profit_target_struct->profit_target);
    uint64_t clear_mask = (uint64_t)(-(int64_t)exit_pass) & (1ULL << idx);
    pool->bitmap &= ~clear_mask;
    active &= active - 1;
  }
}

//=============================================================================
// [USAGE EXAMPLE]
//=============================================================================
int main() { return 0; }
//=============================================================================
// [ASM BREAKDOWN]
//=============================================================================
//=============================================================================
