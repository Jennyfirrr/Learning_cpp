//=============================================================================
// [MEMORY ALLOCATION IMPLEMENTED]
//==============================================================================
// [TAGS] [Pool_Allocator] [State_tracking] [Order_parsing]
// [Decision_Engine] [Java-bad] [Data_stream_reading]
//==============================================================================
// [DISCLAIMER]
//==============================================================================
// *caution emoji* Everything in this file was produced while under the effects
// of potentially fatal amounts of caffiene, I take no responsbility for anyone
// who copies my methods or is injured by unlicsenced bit manipulation. Code at
// your own risk, and remember, this is what your brain looks like on bit
// manipulation, think of how your parents would feel. SWAR and SIMD not even
// once. *caution emoji*
//==============================================================================
// [INCLUDE]
//==============================================================================
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <random>
#include <x86intrin.h>

static std::mt19937_64 rng(123124);
//==============================================================================
// [PLANS AND GOALS]
//==============================================================================
// alrighty then class, on todays episode of "Java is bad 101", were gonna try
// to implement a faux data stream using a random number generator to feed data
// through a decision engine, where we use a pool allocator to track order
// states, with a maximum of N orders, and implicit encoding where once a buy
// order is held, if its tracked in the pool allocator, it becomes a sell order
// implicitly because we like losing money faster, and we definitely dont have
// diamond hands lol, so were gonna learn to make trades based on pre computed
// decisions using simple strategies like SMA cross over or something like were
// stuck in the past and my dad still loves me, ill probably kinda go off plan
// for this one but the expected layout is gonna be something like  the
// following:
//
// [Data stream read] -> [data meets criteria] -> [stores simulated buy] ->
// [data stream read] -> [sell condition met] -> [clear slot in pool allocator]
// -> [loop]
//
// and were gonna try to store something like 8 orders or something, and
// actually start getting into encoding data within the actualy bytes since weve
// gotten familiar with that practice right? so, within this, im gonna try to
// use the MSB in each byte to indiciate wether the order is good or not, the
// 7th bit to track buy or sell, where sell is a 1, and buy is a 0, and the rest
// idk, that should probably be enough for this exercise, but maybe not who
// really knows lmao, were also gonna start using __asm__ volatile("mfence" :::
// "memory"); along with seperate rdstc counters to actually track and compare
// cycle counts that the cpu has to use per function, along with the discovery
// that took me nearly 40k words to get to where we learned you just have to use
// a fucking *sprakle emoji*S T R U C T*sparkle emoji*, that kinda broke me for
// a few days ngl, like harder than when i realized my dad doesnt love me,
// because the realization you just have to organize your data in a struct is,
// lets be honest, a once in a life time realization, so anyways, were gonna
// continue using that, and actully implement a pool alocator
//
// OH YEAH, were also gonna have like a .txt file or something, maybe a .csv
// file idk i havent decided yet, that stores the orders and stuff that are mad,
// with a faux portfolio number to play with, so like, and very primitive
// backtester, we may get to implementing like actual historical data reading in
// this but idk, its entirely dependent on my mood, and now that whatever
// hyperfocus spiral happened over the last 2 weeks has kinda settled down, i
// dont feel AS pressured to do this, but im just curious, also the files may be
// a little less unhinged, because like idk what possessed me to do all that
// lol, but its still gonna have a similar tone lmao, i still hate java, as you
// can clearly see, so were like almost back to normal, anyways, im gonna get a
// better mental model built and think some about this ebfore i actually start
// to code, but its gonna build on concepts that ive covered previously
//
// im not actually too sure how the tracking and managing multiple positions
// will work, but ill probably figue it out as i go, ill probably update this as
// needed or something as i come to more realizations or discoveries about using
// stuff, this is probably need the lock-free concurrency too, because well be
// tracking states and reading and writing to a file, so thats something ill
// need to keep in mind too, feel free to follow along lol, or suggest anything,
// im not an expert and im still learning some stuff, even if i am kinda
// educated about other stuff
//
// TLDR: java is bad and were using pool allocators
//==============================================================================
// EDIT: ok so i thought more about this, and the current way im routing orders
// and feeding data to them is bascailly simulating data streams from multiple
// different stocks at the same time, but for continuously building up the way
// these are going, it would be kinda bad to jump straight to a cross sectional
// model for that, because thats basically what it would be anyways, like 8
// seperate data streams, with a single lane watching each one is basically how
// a zoo of models would work, for models trained on seperate stocks, so its
// like a multi data stream input system, so for this, im not exactly sure what
// would happen, like i guess using the implcit encoding i kinda went over where
// each lane could be a seperate strategy, while runnig the same 8 bits through
// each one that may be the move tbh, because theyd basicall act as gates or
// triggers, like:
//
// if (condition met) {
// 		store buy order in pool
// } else {
// 		continue
// }
//
// if(other condition met) {
// 		place sell orders
// 		update portfolio value
// 		remove from pool
// } else {
// 		continue
// }
//
// so kinda like that, basically running multiple strategies and using each lane
// to watch for different things, idk, im still kinda thinking this through,
// because im not exactly sure how it would work, im just THEORY CRAFTING right
// now, and i kinda like how these are getting cloned, it makes me feel a tad
// less isolated even if i am fucking weird irl(please love me), idk its a start
// lol, obviously we arnt gonna jsue if/else statements , its just pseudo code,
// and im gonna have to think more about how the bitwise operators would work
// for this, because each condition could basically be evaluated as a true false
// chain, so it never has to branch, or just maybe a jump table, or actually
// using asm to create cmov instructions to force it to avoid branch prediction,
// so hmmm, but also like, there could be a main data feed, that then splits off
// to seperate "watchers" who manage each seperate pool slot? idk, this is why
// im thinking it through lol
//
// EDIT2: gonna work on this more later, and im wondering if i should keep the
// slightly desperate feel to be loved in the tone, or if the earlier ones where
// im comparing this to cocaine are better, well let the clone rates decide i
// guess, unless your actually here for the technical content, in which case,
// you confuse me, dont you just love seeing my inner monologue though? im
// adorable arnt i, mild mannered, kinda boring appearing in person, and then
// you read this, and im just fucking insane, god i love the duality, something
// something duality of man philisophical quote or something, idk im not a
// philosopher, im just kinda setting this up with everything well need before
// actually adding new stuff because im kinda happy with the code from the 09
// file, but i wanna modify it and toy around with it some more, and i dont
// wanna break that one because these are meant to kinda interate on eachother
//
// dont worry, well be back to writing code that normal jobs would consider a
// violation of the geneva convetions soon, i assure you, because thats what
// were all here for, as well as my unhinged inner monologue maybe idk, i guess
// this has gone from personal notes to technical shitposts lol, solid technical
// information as well as performance art, and we all get to learn about cursed
// coding patterns like the orderpacking function that would make your boss ask
// you what the fuck your doing if you work at a normal dev job, and the
// craziest thing is like im actually retaining this stuff weirdly enough
//==============================================================================
// [FUNCTIONS[CODE]]
//==============================================================================
// [STRUCTS]
//==============================================================================
// this is probably gonna require more, like an initial struct or something
// passed as an entire 64 bits for a single order,idk, im working through it in
// my head
//==============================================================================
// [EDIT [04-03-26 2:16am]]
//==============================================================================
// so these structs are the base layout for just testing configs, these are most
// likely gonna need to be split upsomehow to reflect the planning and goals in
// the outline above, im not sure if ill keep them as structs, but like i could
// pass the struct and just route a single order, because the compiler will
// probably just break that down and do a single mov instruction to move the
// associated byte?, im not sure, but that would be one way to split into like a
// watcher function, im not sure if i should make it iterate through orders,
// because a for loop would introduce overhead and probably ADD latency, maybe
// typing out each actual think i want done similar to the order packing and the
// risk gate will be the move here, because that compiles down cleaner even if
// it does look like a 5 year olds code, that way it just has to pass the entire
// struct too, but i could have a seperate water for the data stream, like a
// buyside and a sell side depending on conditions met? idk, thats why this
// takes more thought, because while each previous file was like stepping up a
// single stair, this is like a magnitude more complex, but we can work through
// this together, also im not just using ai to code this for me because its
// actually kinda fun to code by hand lol, like im actually enjoying this
// despite the extreme amount of time im spending on it, like i said im HOOKED
// lol, and i hope someone else finds this as cool as i do, even if theyre using
// it for different applications like game design and stuff, like passing a full
// stuct in as a function parameter doesnt actually compile downt o moving the
// entire contents of one register into another, it may pass it as a stack
// pointer? im not too sure entirely, ill probably think about this some more,
// add like a rough early function and do some asm breakdowns like last time
// where we slowly iterate through the optimization process T O G E T H E R,
// because its not about the destination, but the friends we made along the way
// :) <3
//==============================================================================
// [EDIT [04-03-26 03:08am]]
//==============================================================================
// idk why i didnt just think of this earlier, but the order routing is you just
// assume that every order will be placed, and you just use the risk gate to
// pass bad orders as 0 lol, so every time the function runs, you attempt to
// place the order with the current information being passed through the bits,
// and unless it matches the specific risk detail or strategy you set for the
// lane, it only places it when it meets that condition lol, like idk wtf i was
// thinking about earlier lmao, and i thought this was ognna some complex thing
// when i literally made the logic for it earlier, god i swear my brain is like
// a gold fishes, lmao, ONCE AGAIN, overthinking everything, the only problem is
// storing a position in the pool because were using it as a statemachine lmao,
// where an order either exists and is waiting to be sold wunder x conditions,
// or a position is empty and waiting to be filled under y condition, like why
// do i make this so hard and overcomlpicated when the simple answer is almost
// always better lmao, i literally wrote this in the last file lmao
//
// and even that part is branchless lol, beacuse you just use a bool value after
// splitting the orders off using the watcher idea i laid out earlier probably
// lol, like if lane 1, add to the pool, then wait for a sell bit to be 1, and
// if the conditions for that is met, it sells, updates portfolio, and clears
// the pool, and continue, GOD WHY DO I OVERCOMPLICATE EVERYTHING, see i told
// you my brain was smooth and aerodynamic just like the planes boeing makes,
// SOLID ENGINEERING, less drag means faster operation, which means im smarter,
// no folds, smooth, good brain, 200iq, 200 years of collective design
// experience
//==============================================================================
struct buy_lanes {
  uint8_t buy0;
  uint8_t buy1;
  uint8_t buy2;
  uint8_t buy3;
  uint8_t buy4;
  uint8_t buy5;
  uint8_t buy6;
  uint8_t buy7;
};
static_assert(sizeof(buy_lanes) == 8, "struct must be 8 bytes");

struct sell_lanes {
  uint8_t sell0;
  uint8_t sell1;
  uint8_t sell2;
  uint8_t sell3;
  uint8_t sell4;
  uint8_t sell5;
  uint8_t sell6;
  uint8_t sell7;
};
static_assert(sizeof(sell_lanes) == 8, "struct must be 8 bytes");

struct CurrentOrder {
  uint8_t current_order;
};
static_assert(sizeof(CurrentOrder) == 1, "struct must be 1 byte");

struct RiskGate_BuySide {
  uint8_t buyside_risk0;
  uint8_t buyside_risk1;
  uint8_t buyside_risk2;
  uint8_t buyside_risk3;
  uint8_t buyside_risk4;
  uint8_t buyside_risk5;
  uint8_t buyside_risk6;
  uint8_t buyside_risk7;
};
static_assert(sizeof(RiskGate_BuySide) == 8, "struct must be 8 bytes");

struct RiskGate_SellSide {
  uint8_t sellside_risk0;
  uint8_t sellside_risk1;
  uint8_t sellside_risk2;
  uint8_t sellside_risk3;
  uint8_t sellside_risk4;
  uint8_t sellside_risk5;
  uint8_t sellside_risk6;
  uint8_t sellside_risk7;
};
static_assert(sizeof(RiskGate_BuySide) == 8, "struct must by 8 bytes");

struct OrderPool {
  CurrentOrder *slots;
  uint64_t bitmap;
  uint32_t capacity;
};
static_assert(sizeof(OrderPool) == 24, "struct must be 24 bytes");

//==============================================================================
// [POOL ALLOCATOR]
//==============================================================================
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

//==============================================================================
// [ORDER PACKING]
//==============================================================================
// [EDIT [04-03-26 03:33am]]
//==============================================================================
// so these are gonna need to be reworked, because it doesnt make sense to cap
// through put using a split lane design for risk gates, so im gonna rework this
// to function like having a buy risk gate an then a sell risk gate, that way
// all 8 lanes can be configured, and routing the orders through a circular
// system that would require that logic requres uneeded complexity that would
// add latency overhead, while adding 2 risk gate checks, just makes more sense
// because then its a linear path the entire way, once again, trying to develop
// this like im a java developer lol, we dont need that unneeded over head lol,
// java bad, the earlier files were just kinda experiemnting and getting used to
// this ebcause it kind of requires thinking about integers differently, like
// instead of thinking about it like 10 + 10 = 20, its kind of a mental shift to
// thinking about them as which bits are set, idk its hard to describe tbh, if
// you kind of go through the same process and hand trace binary tables like i
// did youll understand
//==============================================================================
// [FLAGGED FOR REWOKR] [IM TALKING TO YOU JENNIFER, WERE REWROKING THIS BITCH,
// DONT OPEN THIS FILE AFTER YOU SLEEP AND IGNORE THE ABOVE MESSAGE OR I SWEAR
// TO GOD WERE GONNA HAVE WORDS]
//==============================================================================
// [EDIT [04-03-26 12:59pm]]
//==============================================================================
// this one may be uneeded with the new risk gate layout, idk why im making this
// public, part of me feels like other people who are bored in class may like
// this, idk, i wonder if anyone actually enjoys reading and learning from these
//==============================================================================
uint64_t order_packing_8_byte(OrderPair pair) {
  uint64_t packed_orders = 0;

  packed_orders |= static_cast<uint64_t>(pair.buys.buy0);
  packed_orders |= static_cast<uint64_t>(pair.buys.buy1) << 8;
  packed_orders |= static_cast<uint64_t>(pair.buys.buy2) << 16;
  packed_orders |= static_cast<uint64_t>(pair.buys.buy3) << 24;
  packed_orders |= static_cast<uint64_t>(pair.sells.sell0) << 32;
  packed_orders |= static_cast<uint64_t>(pair.sells.sell1) << 40;
  packed_orders |= static_cast<uint64_t>(pair.sells.sell2) << 48;
  packed_orders |= static_cast<uint64_t>(pair.sells.sell3) << 56;

  return packed_orders;
}
//==============================================================================
// [RISK GATE]
//==============================================================================
// [EDIT [04-03-26 03:38am]]
//==============================================================================
// gonna need two seperate risk gates, and a way to encode different strategies
// using bits, will probably require nomalizing stock prices and winsorization
// once i get to useing real stock data, but for now, i can cap the orders using
// uint8_t and then converting to uint64_t using static_cast, and building the
// orders before passing them through the pipeline, but i just wanted to leave a
// note that there will be a seperate buy/sell gate, so all 8 lanes are
// dedicated to a single process, as outlined above
//==============================================================================
uint64_t build_risk_gate_buy(RiskGate_BuySide sides) {
  uint64_t risk_gate_built_buy_side = 0;

  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk0);
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk1) << 8;
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk2) << 16;
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk3) << 24;
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk4) << 32;
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk5) << 40;
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk6) << 48;
  risk_gate_built_buy_side |= static_cast<uint64_t>(sides.buyside_risk7) << 56;

  return risk_gate_built_buy_side;
}

uint64_t build_risk_gate_sell(RiskGate_SellSide sides) {
  uint64_t risk_gate_built_sell_side = 0;

  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk0);
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk1) << 8;
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk2)
                               << 16;
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk3)
                               << 24;
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk4)
                               << 32;
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk5)
                               << 40;
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk6)
                               << 48;
  risk_gate_built_sell_side |= static_cast<uint64_t>(sides.sellside_risk7)
                               << 56;

  return risk_gate_built_sell_side;
}

uint64_t risk_gate_check_buy_side(uint64_t packed_order,
                                  uint64_t risk_gate_buy_side) {
  uint64_t breach_buy =
      (risk_gate_buy_side - packed_order) & 0x8080808080808080ULL;
  return breach_buy;
}

uint64_t risk_gate_check_sell_side(uint64_t packed_order,
                                   uint64_t risk_gate_sell_side) {
  uint64_t breach_sell =
      (risk_gate_sell_side - packed_order) & 0x8080808080808080ULL;
  return breach_sell;
}

//==============================================================================
// [MAIN]
//==============================================================================
//==============================================================================
// [ASM BREAKDOWN BY FUNCTION]
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
