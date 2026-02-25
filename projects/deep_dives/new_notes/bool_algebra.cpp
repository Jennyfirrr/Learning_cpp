// this is kinda a placeholder file for something im gonna implement + the math
// behind it

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
// so now the diff is just 0x03080A21 right, then you wanna subtract 0x01010101
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
// T H I S  S H I T  I S  A M A Z I N G
