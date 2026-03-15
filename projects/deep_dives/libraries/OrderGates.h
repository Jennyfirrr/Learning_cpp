//======================================================================================================
// [Main Logic]
//======================================================================================================
#ifndef ORDER_GATES_H
#define ORDER_GATES_H

#include <stdint.h>
#include "PoolAllocator.h"
#include "FixedPointN.h"
//======================================================================================================
// [STRUCTS]
//======================================================================================================
// SST_FPN throughout - no float-to-int conversion boundaries, no precision surprises
// Packed gate trick is dropped - SST_FPN comparisons are already branchless so packing buys nothing
//======================================================================================================
template<unsigned F>
struct DataStream {
    SST_FPN<F> price;
    SST_FPN<F> volume;
};

template<unsigned F>
struct ProfitTarget {
    SST_FPN<F> profit_target;
};

template<unsigned F>
struct BuySideGateConditions {
    SST_FPN<F> price;
    SST_FPN<F> volume;
};

template<unsigned F>
struct SellSideGateConditions {
    SST_FPN<F> price;
    SST_FPN<F> volume;
};

//======================================================================================================
//[ORDER GATES]
//======================================================================================================
// no more packing/unpacking - compare SST_FPN fields directly (already branchless)
//======================================================================================================
template<unsigned F>
inline void BuyGate(const BuySideGateConditions<F> *conditions, const DataStream<F> *stream, OrderPool<F> *pool) {
    int price_pass  = SST_FPN_LessThanOrEqual(stream->price, conditions->price);
    int volume_pass = SST_FPN_GreaterThanOrEqual(stream->volume, conditions->volume);

    int pass = price_pass & volume_pass;

    uint64_t mask  = (uint64_t)(-(int64_t)pass);
    uint32_t index = __builtin_ctzll(~pool->bitmap);
    pool->bitmap |= (mask & (1ULL << index));
    pool->slots[index].price    = stream->price;
    pool->slots[index].quantity = stream->volume;
}

template<unsigned F>
inline void SellGate(const SellSideGateConditions<F> *conditions, const DataStream<F> *stream, OrderPool<F> *pool,
                     const ProfitTarget<F> *profit_target) {
    int price_pass  = SST_FPN_GreaterThanOrEqual(stream->price, conditions->price);
    int volume_pass = SST_FPN_LessThanOrEqual(stream->volume, conditions->volume);

    int pass = price_pass & volume_pass;

    uint64_t active = pool->bitmap;
    while (active) {
        uint32_t idx = __builtin_ctzll(active);
        SST_FPN<F> entry_price  = pool->slots[idx].price;
        SST_FPN<F> target_price = SST_FPN_AddSat(entry_price, profit_target->profit_target);
        int exit_pass           = SST_FPN_GreaterThanOrEqual(stream->price, target_price);
        uint64_t clear_mask     = (uint64_t)(-(int64_t)exit_pass) & (1ULL << idx);
        pool->bitmap &= ~clear_mask;
        active &= active - 1;
    }
}
//======================================================================================================
#endif // ORDER_GATES_H
