//==============================================================================
// This is probably gonna serve as a reference for all bool algebra
//==============================================================================
// this is just the math to explain and understand the function below and how it
// can be useful in the context of a risk gate
/*
uint32_t calc_laneMatchCount(uint32_t packed_order, uint32_t broadcast_mask) {
  uint32_t diff = packed_order ^ broadcast_mask;

  uint32_t matches = ((diff - 0x01010101) & ~diff & 0x80808080);

  return __builtin_popcount(matches);
}
*/

// OK SO, im gonna hand trace the binary math to actually do this, because
// subtraction is kinda W A C K Y, and no where near as intuitive as addition or
// other things, because like multiplication is just left shifts, division by a
// 2 power n is just the associated right shift, and then addition is kinda
// simple tbh, but this tripped me up
//
// SO, lets assume the packed_order = 0x03FF0A21, and the broadcast_mask =
// 0x03080A21 right?,
//
// so we XOR it first to ge tthe diff
//
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 0 0 0 0 0 0 1 1|1 1 1 1 1 1 1 1|0 0 0 0 1 0 1 0|0 0 1 0 0 0 0 1|
// 0 0 0 0 0 0 1 1|0 0 0 0 1 0 0 0|0 0 0 0 1 0 1 0|0 0 1 0 0 0 0 1| ^
// ------------------------------------------------------------------
// 0 0 0 0 0 0 0 0|1 1 1 1 0 1 1 1|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|
//
// so now the diff is just 0x00F70000 right, then you wanna subtract 0x01010101
// from the diff, and this is what i struggled with, because its kinda weird,
// the way i figured it out was calculating the total for each individual byte
// within the 32bit binary format, or lane, for the trading analogy, so the
// subtraction part will be below, so basically the way i kinda of figured this
// out, or a way that made sense to me, was by finding what actual value is in
// each lane right? so before the drawn out table ill list the lane values, with
// normal math so you can see how it made sense to me:
//
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 0 0 0 0 0 0 0 0|1 1 1 1 0 1 1 1|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|
// 0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1| -
// ------------------------------------------------------------------
//
// // [lane 0]: 0 - 1
// [lane 1]: 0 - 1
// [lane 2]: 247 - 1
// [lane 3]: 0 - 1
//
// the lanes are right to left btw, ill put a label above each one
//
// so because you cant just do 0 - 1 = -1, for a lane, since you have to effect
// all the other bytes in the packed order, you have to use carry logic, and to
// make that 0, you need to borrow one from the next byte over, so you end up
// with something like:
//
// [lane 0]: 0 - 1: borrow 1 from [lane 1]
//
// [lane 1]: 0 - 1 - borrow 1 from lane 2: borrow 2 from [lane 2] because there
// was nothing for [lane 0] to borrow so you have to borrow an extra one from
// [lane 2]
//
// [lane 2]: 247 - 2 - 1: doesnt need to borrow, because it has enough to share
// with the other 2 lanes, right?
//
// [lane 3]: so because this lane is basically 0 - 1, and it has nothing to
// borrow from, it basically overflows the top lane, because were using unsigned
// integers here so the 0 - 1 basically becomes 255
//
// and when you borrow a bit from the next byte in a higher lane, that 1 from
// the byte to the left, makes the lane below it become 255, so like, it doesnt
// work like the addition does, this is weirder lol, ill probably have to
// practice this some more, because this is NOT intuitive like AT ALL
//
// so the table i showed above, basucally becomes this table after you kind of
// shift it all around
//
//==============================================================================
//[EDIT [25-02-26 08:29pm]
//==============================================================================
// SO, one thing i just realized is another example to showcase the subtraction
// more easily, like suppose you have 0xFF000000 - 0x00000001, right, and easy
// way to think about this is that whenever you have to carry, you think about
// what the lane above the one that has the actual subtraction has, and when you
// reach a lane that does have enough, you just pull the associated bit from it,
// and because no matter how big the number you are subtracting from the initial
// value is, it will ALWAYS at most, pull a single bit from the lane above it,
// because that 1 bit in like lane 1 -> lane 0, makes lane 0, 255, because the
// maximum bit value in lane 0, is 128, and the first bit in lane 1 basically
// represents 256, ill show another example here, and like even if you have like
// 0x00000001 being subtracted from an intitial value of 0xFF000000, it
// propogates downward right, and so the bit that carries down, is also the bit
// being subtracted from the associated lane
//
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|
// 0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 1| -
//-------------------------------------------------------------------
// 255 - 1 -> 254 | 0 - > 255     | 0 - > 255     | 0 -> 255      |
//-------------------------------------------------------------------
// 1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|
// 0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 1| -
//-------------------------------------------------------------------
// 1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|
//
// so 0xFF000000 - 0x00000001 = 0xFEFFFFFF
//
// EDIT: so i was kinda wrong about the propogation and carrying downwards with
// the borrow logic, since you cant subtract 1 from 0x00, and then lane 1 is
// also 0x00, and lane 2 is also 0x00, you subtract 1 from lane 3, so it becomes
// 0xFE, and then that single bit has to propogate downwards and fills the other
// bits to 255 each, so you get 0xFEFFFFFF, if it were 0xFEFEFEFF, like i
// initially thought, then each lane would have had to lose a single bit, but it
// didnt, and i thought each lane continulally borrowed, instead of each byte
// below the lane that is borrowed from being set to the max value for the 8
// bits within it, and like for another way to see it, like what happens when
// you have 0x0100, right, its not 1, and since 0xFF is 255, that means that
// 0x0100, is 256 right, and 256 - 1, is 255, therefor 0x0100 - 0x0001, is
// 0x00FF, this is just a mini proof, but its an easier way to visualize it
// without all the noise in the table, i just kind like how those look in my
// code editor tbh

//==============================================================================
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 0 0 0 0 0 0 0 0|1 1 1 1 0 1 1 1|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|
// 0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1| -
// ------------------------------------------------------------------
// no borrow flip | 247 - 1 - 1   | borrw 1 - 1   | borrow 1      |
// ------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|1 1 1 1 0 1 0 1|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|
// 0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1| -
// ------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|1 1 1 1 0 1 0 1|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|
//
// so like one way to think about it, is that you want to make the bottom lane
// filled, then the one above, then the next etc I think, im still trying to
// build a mental model for this because this isnt just like, simple subtraction
// lol
//
// see how weird that is lol, this tripped me the heck up lmao
// NOW WE GET TO THE & OPERATORS,and its pretty easy from here tbh but ill show
// it anyways because i just LOVE THIS, like i havent had this much fun doing
// math or something since i was like 6 lol, java would probably be more
// engaging if i had to actually think about what the cpu is doing instead of
// thinking about how many classes i need to make so i can write a simple
// function in like 4x the loc that id have to write in c++, but thats a tangent
// for another file, so now, to show this in order, we have to use the result
// from diff - 0x01010101, so now we do the result of that, which is like:
// 0xFFF5FEFF & not diff, and not diff is basaclly everything flipped, so to
// find that, ill show it below
//
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 0 0 0 0 0 0 0 0|1 1 1 1 0 1 1 1|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0| ~
// ------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|0 0 0 0 1 0 0 0|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1|
//
// so the not diff because this because its just flipping the bits from the diff
// we found at the start
//
// so now that we have the not diff, we can do the (diff - 0x01010101) & ~diff,
// which to me is just S O  M U C H  E A S I E R to understand beacuse its just
// which lanes have the same bit enabled
//
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|1 1 1 1 0 1 0 1|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|
// 1 1 1 1 1 1 1 1|0 0 0 0 1 0 0 0|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1| &
// ------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|0 0 0 0 0 0 0 0|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|
//
// so now we have (diff - 0x01010101) & ~diff as the result shown above
//
// then we just do result and compare it using another & operator with the value
// 0x80808080
//
//-------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |
//-------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|0 0 0 0 0 0 0 0|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|
// 1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0| &
//-------------------------------------------------------------------
// 1 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|
//
// so then you get this value, which becomes 0x80008080,and as you can see, its
// missing the lane 2 value, which is just 00, because that lane was a failed
// lane, then you use the __builtin_popcount(count) function, to actually count
// these, and it should return like 3 or something im pretty sure, because only
// 3 bits are active, but yeah, thats how binary subtraction works lol
//
// but anyways, the reason that this works, is i like to think that it basically
// keeps flipping the values, to isolate a single bit per lane, which could be
// considered like an active flag, or confirm trade flag, and when you flip them
// and use these tricks like this, you end up with an isolated bit per lane, and
// you can use the pop count funciton because it just counts the active bits in
// the entire 32 bit register, ill probably go a little deeper on that function
// later to actually understand how it works, but this was like ALOT lol,
//
// EDIT: apparently this isnt even a function call anymore, its just a single
// assembly instruction called POPCNT, so apparently there is SILICON dedicated
// to counting the number of active bits in a single clock cycle
//
// this trick was attributed to Alan Mycroft from 1987 btw, its often called the
// mycroft trick, or hasless, or haszero macro, it became known through the use
// in strlen() implementations in libc, instead of checking 1 bit at a time for
// the null terminator, you can load 4-8 bytes at once and use this trick to
// detect which word contains a 0 byte, i got reccomeneded a book called the
// Hacker's Delight by Henry S. Warren Jr. which apparently covers TONS of stuff
// like this, so if i ever get that book, you can be *sparkle emoji*A S S U R E
// D*sparkle emoji* that i will start covering stuff from there in this, this
// also shows up in the de Bruijin sequence i mentioned in a prior file lol
//
// SO, i just wanted to add here, the reason that this actually works is the
// following:
//
// the (diff - 0x01010101) part works as a trap, if a lane is 0x00, it forces it
// to either flip, or borrow from another lane, so that is becomes all 1's, and
// sets the MSB(0x80) if the lane is 0
//
// then the & ~diff part, is the basically a guard, you ONLY want to see bits
// that were originally 0 in the diff, so if a byte was like 0x81, the
// subtraction would leave the MSB set, but the & ~diff would kill it beacuse
// the original MSB was already 1
//
// then the 0x80808080 part, this is the filter, it ignores all the noise, and
// ONLY looks for the sign bit in each lane
//
// so i guess this basically works to normalize all the bits except the sign bit
// to 0, and then sets the sign bit to either a 1 or 0 depending on the exact
// conditions that you want, which are eventually controlled by the 0x80808080
// part
//
// T H I S  S H I T  I S  A M A Z I N G
//
// Function for reference again when looking at the full table
//
/*
uint32_t calc_laneMatchCount(uint32_t packed_order, uint32_t broadcast_mask) {
  uint32_t diff = packed_order ^ broadcast_mask;

  uint32_t matches = ((diff - 0x01010101) & ~diff & 0x80808080);

  return __builtin_popcount(matches);
}
*/
//
// *gasp*  F U L L  T A B L E
//
// -------------------------------
// | packed_order   = 0x03FF0A21 |
// | broadcast_mask = 0x03080A21 |
// -------------------------------
// |E Q U A T I O N |
// ----------------------------------------
// | diff = packed_order ^ broadcast_mask |
// ----------------------------------------------
// | ((diff - 0x01010101) & ~diff & 0x80808080) |
// | return popcount of the matches from above  |
//---------------------------------------------------------------------
//      lane 3    |    lane 2     |    lane 1     |     lane 0    |   |
//---------------------------------------------------------------------
// 0 0 0 0 0 0 1 1|1 1 1 1 1 1 1 1|0 0 0 0 1 0 1 0|0 0 1 0 0 0 0 1|   |
// 0 0 0 0 0 0 1 1|0 0 0 0 1 0 0 0|0 0 0 0 1 0 1 0|0 0 1 0 0 0 0 1| ^ |
// --------------------------------------------------------------------
// 0 0 0 0 0 0 0 0|1 1 1 1 0 1 1 1|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|   |
// 0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1| - |
// --------------------------------------------------------------------
// no borrow flip | 247 - 1 - 1   | borrw 1 - 1   | borrow 1      |   |
// --------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|1 1 1 1 0 1 0 1|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|   |
// 0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1|0 0 0 0 0 0 0 1| - |
// --------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|1 1 1 1 0 1 0 1|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|   |
// 1 1 1 1 1 1 1 1|0 0 0 0 1 0 0 0|1 1 1 1 1 1 1 1|1 1 1 1 1 1 1 1| & |
//---------------------------------------------------------------------
// 1 1 1 1 1 1 1 1|0 0 0 0 0 0 0 0|1 1 1 1 1 1 1 0|1 1 1 1 1 1 1 1|   |
// 1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0| & |
//---------------------------------------------------------------------
// 1 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|1 0 0 0 0 0 0 0|   |
// popcount       |popcount       |popcount       |popcount       |   |
// --------------------------------------------------------------------
// 1              +0              +1              +1              = 3 |
// --------------------------------------------------------------------
//
// for a result of *sparkle emoji*T H R E E*sparkle emoji*
// (this is the part where everyone claps and tells me how great I am)
//
// i wanted to add this so i could see exactly how it traces thruogh in a
// single look
//
// EDIT: this is apparently called a SWAR technique, which ust means SIMD within
// a register, which is basically the art of treating a standard 64-bit register
// as a vector or list of 8-bit or 16-bit values, and this lets you perform
// parallel operations without actually using the specialized SIMD(AVX/SSE)
// hardware
//==============================================================================
// EDIT[27-02-26 02:06am] bool masking
//==============================================================================
// just a neat little trick to take advantage of two complement to check if a
// trade is good or not without using an if statement
//
// so, because of the way twos complement works, you can take advantage of that
// with a little line of code like this:
//
// uint64_t filtered_price = trade_price & -static_cast<int64_t>(trade_price >
// 100);
//
// so the reason this works, is that by taking advantage of the sign bit, since
// the trade_price > 100 , returns a bool, when you convert that to a negative,
// it basically becomes 0xFFFFFFFF, if its true, and 0x00000000, if its false,
// that way when you want to compare the actual trade your evaluation, it
// basically becomes the trade_price & 0xFFFFFFFF if its 1, and 0x00000000, if
// its 0, and because of the way this works, it will return either a 1 or 0
// based on twos complemenet, im pretty sure that i went over this in a
// different file, but its good to reiterate it here for pattern matching
// building, so while sign bits are icky for order packing, theyre actually
// kinda useful when taking advnateg of bitmasks and gates
//==============================================================================
// [EDIT [28-02-26 02:37am]] I couldnt sleep so im gonna do a simple
// multiplication trade to get familiar with it because its apparently wackier
// than subtraction [TAG] [ Binary Multiplication ]
//==============================================================================
//
// CANT SLEEP LOL
//
// so, in base-10 when you multiply to ints such as the equation 12 * 13, youre
// basically doing something like this [(12 * 3) + (12 * 10)], where its split
// into 2 simpler calculations, and then you add the results together, because
// this is just genuinely easier to parse, than straight up doing 12*13, binary
// is apparently kinda the same, but as state before, none of this is intuitive
// lol, so im gonna do a quick example for myself here just using a single byte,
// to kinda get familiar with it
//
// in this example, im gonna use 5 * 3, which is basically just 00000101 *
// 00000011, so now MORE TABLES :D, so in base 10, this is easy, we know its
// just 15, because were used to multiplication like that in our heads, but this
// makes zero sense to me in base 2
//
//--------------------
// 0 0 0 0 0 1 0 1 |
// 0 0 0 0 0 0 1 1 | *
// -------------------
//
// so apparently you wanna look at the multiplier first when doing this, which
// is 0011, so since bit[0] is a 1, you want to keep the original value of 0101
//
// so now we have
//
// 0 1 0 1
//       ^ <- keep because the bit 0 of the muliplier is 1
//
// so then because the bit[1] is a 1, you want to shift the origial value, by
// the left 1 iteration so you get
//
// 0 1 0 1
// 0 0 1 1
//     ^ <- because this is a 1, you want to shift the top number to the left
//     once
//
// 1 0 1 0
//
// then, we now have the two numbers in binary which are 1010 and 0101, and as
// stated by the example of 12*13 before, we first do 2 simpler mulitplication
// operations and then add the results, so now we just add them together
//
//-----------
// 1 0 1 0|
// 0 1 0 1| +
// ----------
// 1 1 1 1 = 15
//
// so im guessing this is because your basically multiplying the top number by
// each number that the associated bit in the multiplier? bceause the bit[0]
// being a 1, you multiply 0 1 0 1, by 1, so it basically stays then same, and
// then for the bit[1] that represents 2, so you basically shift it to the left
// once, because thats a multiplication operation by 2, and im guessing this
// extrapolates across the others, so if you have a bit[2], you would shift left
// twice, because thats basically multiplying by 4, by the nature of how left
// shifts work, where each shift is essentially 2^n where n is the amount of
// shifts you do
//
// so this is apparently called Booths multiplication Algorithm, or at least the
// shift + add foundation of it, and the way described above is basically how
// the ALU(arithmatic logic unit) handles all of this, and another neat insight
// that was pointed out to me, is that in this way of thinking about
// multiplication, multiplying by a power of 2 is just <<, always so like 2 4 8
// 16 32, etc, are just associated left shifts, where as multiplying by a
// constant like 7, is like (x << 3) - x, which i think is really neat, and then
// i guess if you were multiplying by a constant like 10, you would do something
// like (x << 3) + (x << 1), because 3 left shifts is 8, and then you wanna add
// that with the total from a single left shift, because if you just did x << 4,
// you would be multiplying by 16, wheras this way gives you the total of x << 3
// which is (x * 8) + (x * 2), which is essentially what the imul instruction
// was doing in a file with actual code where it was applying the risk gate all
// at once instead of iterating through a loop using something like (1 << 16) +
// (1 << 8), or something like that to get the number 65792, so i guess this is
// a basic version of de-bruijn multiplication
//
// EDIT: so i may have been wrong about the thing using imul above, when i was
// picking apart some asm code from another file, i noticed that this constant
// was used for an imul instruction, while apparently the shifts can be used to
// avoid calling the imul instruction which is higher latency, by like ~3 cycles
// or so, but shifting is a single operaiton, so its basically 66% faster, also,
// this is just a simplified shift + add operation, wheras booths algorithm is
// more complex, Booth's is a refinement that handles signed integers, and it
// can reduce the number of additions by encoding runs of 1's, so its
// technically related, but its more advanced,
//
// I was also wrong about this being de-bruijn, that is more like using a
// sequence as a perfect hash, de-bruijn typically shows up more in things like
// ctz(), or __builtin_ctz() which are implementations for hardware that lacks
// native bit instructions, so the thing below is more like just a constant is a
// risk gate code for a standard strength reduction, sorry i get so much wrong,
// im trying to learn as best i can, and its not like theres exactly a free
// course for all this stuff, and its more fun to kinda figure it out as i go
// tbh, ctz() is also a function to Count Trailing Zero's, just as a side note,
// i think in my asm_reference.cpp file, i have a TODO to go over this, so its
// on the agenda, i just havent gotten to it because im *sparkle emoji*F O L L O
// W I N G  M Y  H E A R T*sparkle emoji* <3
//
//==============================================================================
// [NEAT TRICK SHOWN TO ME]
//==============================================================================
//
bool isPowerOfTwo(uint32_t x) { return x && !(x & (x - 1)); }
//
// this is basically saying return x assuming its not equal to the power of 2
// check, because the x & (x -1) is the actual logic that checks for powers of
// 2, using the example 8, which is 00001000, and 7 is 00000111, so 00001000 &
// 00000111, returns 0, i kinda went over the logic of why this works in an
// earlier file with actual code, but those are monoliths lol, so ill expalain
// it here, when you use x where x is a power of 2, its always going to be a
// single bit set, so like 2 is 10, 4 is 100, 8 is 1000, 16 is 10000, etc, now
// when you do x - 1, you always get something like 01, 1, 011, 3, 0111, 7,
// 01111,15, so when you do x & (x - 1), it will return a 0 if x is a power of
// 2, because there wont be ANY bits that are the same, because it ends up
// looking like this, and for this example im using x = 16
//
//-------------------
// 0 0 0 1 0 0 0 0|
// 0 0 0 0 1 1 1 1| &
// ------------------
// 0 0 0 0 0 0 0 0 = 0, because no bits line up, i figured i may as well add
// this here even though its kinda basic because this is technically a reference
// file
//
// so in the code example above my explanation was a little wrong, if it is 0,
// it will return a 1, so x && 1, is basically just x, which means x is a power
// of 2, in any other situation it will reutnr x && 0, which is 0, so not a
// power of 2
//
// this would fail for non powers of 2, so lets use like 10 as an example:
//
//-------------------
// 0 0 0 0 1 0 1 0|
// 0 0 0 0 1 0 0 1| &
// ------------------
// 0 0 0 0 1 0 0 0 = 8, so then you get 10 & 8, which returns 8, which means 10
// is not a power of 2
//
//==============================================================================
//[TODO[0x077CB531, de-bruijn constant that lets you map a bit index to a has
// table in a single multiply shift]] more booths probably, divions is probably
// just shift operations and adding similar to multiplication, with just
// cascading powers of 2 shifts from largest -> least, iterating through
//==============================================================================
//==============================================================================
