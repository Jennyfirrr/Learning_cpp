//======================================================================================================
// [Main Logic]
//======================================================================================================
#ifndef ORDER_GATES_H
#define ORDER_GATES_H

#include <stdint.h>
#include "PoolAllocator.h"
#include "FixedPoint32.h"
//======================================================================================================
// [STRUCTS]
//======================================================================================================
// SST_FP32 throughout — no float-to-int conversion boundaries, no precision surprises
// DataStream goes from 8 to 16 bytes but still cache-line friendly (two 8-byte stores)
// Packed gate trick is dropped — SST_FP32 comparisons are already branchless so packing buys nothing
//======================================================================================================
typedef struct {
    SST_FP32 price;
    SST_FP32 volume;
} DataStream;
static_assert(sizeof(DataStream) == 16, "DataStream should be 16 bytes");

typedef struct {
    SST_FP32 profit_target;
} ProfitTarget;
static_assert(sizeof(ProfitTarget) == 8, "ProfitTarget should be 8 bytes");

typedef struct {
    SST_FP32 price;
    SST_FP32 volume;
} BuySideGateConditions;
static_assert(sizeof(BuySideGateConditions) == 16, "BuySideGateConditions should be 16 bytes");

typedef struct {
    SST_FP32 price;
    SST_FP32 volume;
} SellSideGateConditions;
static_assert(sizeof(SellSideGateConditions) == 16, "SellSideGateConditions should be 16 bytes");

//======================================================================================================
//[ORDER GATES]
//======================================================================================================
// no more packing/unpacking — compare SST_FP32 fields directly (already branchless)
//======================================================================================================
static inline void BuyGate(const BuySideGateConditions *conditions, const DataStream *stream, OrderPool *pool) {
    int price_pass  = SST_FP32_LessThanOrEqual(stream->price, conditions->price);
    int volume_pass = SST_FP32_GreaterThanOrEqual(stream->volume, conditions->volume);

    int pass = price_pass & volume_pass;

    uint64_t mask  = (uint64_t)(-(int64_t)pass);
    uint32_t index = __builtin_ctzll(~pool->bitmap);
    pool->bitmap |= (mask & (1ULL << index));
    pool->slots[index].price    = stream->price;
    pool->slots[index].quantity = stream->volume;
}

static inline void SellGate(const SellSideGateConditions *conditions, const DataStream *stream, OrderPool *pool,
                            const ProfitTarget *profit_target) {
    int price_pass  = SST_FP32_GreaterThanOrEqual(stream->price, conditions->price);
    int volume_pass = SST_FP32_LessThanOrEqual(stream->volume, conditions->volume);

    int pass = price_pass & volume_pass;

    uint64_t active = pool->bitmap;
    while (active) {
        uint32_t idx       = __builtin_ctzll(active);
        SST_FP32 entry_price = pool->slots[idx].price;
        SST_FP32 target_price = SST_FP32_AddSat(entry_price, profit_target->profit_target);
        int exit_pass      = SST_FP32_GreaterThanOrEqual(stream->price, target_price);
        uint64_t clear_mask = (uint64_t)(-(int64_t)exit_pass) & (1ULL << idx);
        pool->bitmap &= ~clear_mask;
        active &= active - 1;
    }
}
//======================================================================================================
#endif // ORDER_GATES_H
