//======================================================================================================
// [Main Logic]
//======================================================================================================
#ifndef ORDER_GATES_H
#define ORDER_GATES_H

#include <stdint.h>
#include <immintrin.h>
#include "PoolAllocator.h"
#include "FixedPoint32.h"
//======================================================================================================
// [STRUCTS]
//======================================================================================================
typedef struct {
    uint32_t price;
    uint32_t volume;
} DataStream;
static_assert(sizeof(DataStream) == 8, "DataStream should be 8 bytes");

typedef struct {
    uint32_t profit_target;
} ProfitTarget;
static_assert(sizeof(ProfitTarget) == 4, "ProfitTarget should be 4 bytes");

typedef struct {
    uint32_t price;
    uint32_t volume;
} BuySideGateConditions;
static_assert(sizeof(BuySideGateConditions) == 8, "BuySideGateConditions should be 8 bytes");

typedef struct {
    uint32_t price;
    uint32_t volume;
} SellSideGateConditions;
static_assert(sizeof(SellSideGateConditions) == 8, "SellSideGateConditions should be 8 bytes");

typedef struct {
    uint64_t packed_conditions_buy;
} BuyGateBuilt;
static_assert(sizeof(BuyGateBuilt) == 8, "BuyGateBuilt should be 8 bytes");

typedef struct {
    uint64_t packed_conditions_sell;
} SellGateBuilt;
static_assert(sizeof(SellGateBuilt) == 8, "SellGateBuilt should be 8 bytes");

//======================================================================================================
// [BUILD GATES]
//======================================================================================================
static inline BuyGateBuilt build_buy_conditions(BuySideGateConditions *conditions) {
    uint64_t packed_conditions = 0;
    packed_conditions |= conditions->price;
    packed_conditions |= ((uint64_t)conditions->volume << 32);
    return (BuyGateBuilt){packed_conditions};
}

static inline SellGateBuilt build_sell_conditions(SellSideGateConditions *conditions) {
    uint64_t packed_conditions = 0;
    packed_conditions |= conditions->price;
    packed_conditions |= ((uint64_t)conditions->volume << 32);
    return (SellGateBuilt){packed_conditions};
}

//======================================================================================================
//[ORDER GATES]
//======================================================================================================
static inline void BuyGate(const BuyGateBuilt *packed_conditions, const DataStream *stream, OrderPool *pool) {
    uint32_t price  = stream->price;
    uint32_t volume = stream->volume;

    uint32_t price_pass  = price <= (packed_conditions->packed_conditions_buy & 0xFFFFFFFF);
    uint32_t volume_pass = volume >= (packed_conditions->packed_conditions_buy >> 32);

    uint32_t pass = price_pass & volume_pass;

    uint64_t mask  = (uint64_t)(-(int64_t)pass);
    uint32_t index = __builtin_ctzll(~pool->bitmap);
    pool->bitmap |= (mask & (1ULL << index));
    pool->slots[index].price.price   = price;
    pool->slots[index].volume.volume = volume;
}

static inline void SellGate(const SellGateBuilt *packed_conditions, DataStream *stream, OrderPool *pool,
                            const ProfitTarget *profit_target_struct) {
    uint32_t price  = stream->price;
    uint32_t volume = stream->volume;

    uint32_t price_pass  = price >= (packed_conditions->packed_conditions_sell & 0xFFFFFFFF);
    uint32_t volume_pass = volume <= (packed_conditions->packed_conditions_sell >> 32);

    uint32_t pass = price_pass & volume_pass;

    uint64_t active = pool->bitmap;
    while (active) {
        uint32_t idx         = __builtin_ctzll(active);
        uint32_t entry_price = pool->slots[idx].price.price;
        uint32_t exit_pass   = (price >= entry_price + profit_target_struct->profit_target);
        uint64_t clear_mask  = (uint64_t)(-(int64_t)exit_pass) & (1ULL << idx);
        pool->bitmap &= ~clear_mask;
        active &= active - 1;
    }
}
//======================================================================================================
#endif // ORDER_GATES_H
