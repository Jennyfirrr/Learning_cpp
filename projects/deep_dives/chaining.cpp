#include <iostream>

int32_t check_first(int32_t num) {
  // this function extracts the bit from the from the bit index[1], while
  // leaving the original number untouched
  int32_t result = num & (1 << 1);
  // the cout here is so you can see what is happening
  std::cout << result << "\n";
  return result;
}

int32_t check_second(int32_t num) {
  // this function extracts the bit from the bit index [2], while leaving the
  // original untouched just like the first
  int32_t result = num & (1 << 2);
  std::cout << result << "\n";
  return result;
}

int32_t smallest(int32_t num1, int32_t num2) {
  // this compares the two bits to find which is the higher bit, sort of a small
  // signal extractor that would be used to unpack data from a bit
  // Apparently the name for this is a branchless comparator chain for comparing
  // bits, there are two ways to kind of use the data from this i guess, the one
  // im showing here is using the positional encoding that s foundin the bit,
  // where result from the prior 2 functions returns the actual value that the
  // bit represents, like 2, from check first, and 4 from check second, you
  // could also return a 0 or 1 by masking it, but im just kind of toying around
  // with this atm, according to what im reading, this is essentially the same
  // pattern used like i stated above or w/e, like imagine that the number 255
  // is an orderbook state, and you have the binary code 0x000000FF, so
  // 11111111, you basically have both the 2, and the 4 position considered
  // active, where these could represent different things, like, "is this bid
  // aggressive" and "is this spread wide", the positional value of the bit that
  // is active could also preserve where the bit is, not just is it set or not,
  // and this is apparently called implicit encoding, where the data carries its
  // own metadata, which further allows for even more functionality in the L1
  // cache memory limitations, because the more data you can encode into a
  // single bit, the faster your system will be, because no ICKY ram hits, we
  // dont like those, ram is for training models, not executing trades lol
  // im probably gonna keep expaning on this file for a bit, but its like 3am
  // lol on a sunday, and im procrastinating doing my java homework(shocker) by
  // experimenting with this lol, using this method though, you basically build
  // an if / else branch, without using branches that essntially do whatever is
  // encoded in num1, assuming that it is bigger than num2, if num2 is bigger,
  // do num2, im sure that there are versions of these out there that use the
  // entire 32bit or even 64 bit register and have it all running in tandem
  // using SIMD or AVX instructions but im just a girl, and a baby HFT(hopeful)
  // engineer, so im L E A R N I N G, there are probably methods to use the
  // outputs from the first 2 functions, to output to a vector<vector<int>> or
  // something, where you can basically preload deicisons or something assuming
  // there is memory for it, to ensure that the hotpath is continually fed and
  // never has down time, but this probably requires a network, or multiple
  // colocated servers operating together, with a central order routing "brain",
  // im not sure how the latency part is figured out, but if your continually
  // caching orders like that, it becomes more about ensuring that you are
  // generating potential deicisons fast enough, so that the vector is never
  // empty(?), im just kind of thinking here, so i have no clue if this is right
  // or not, but saturating an instruction list like that kind of seems optimal,
  // given just how fast these systems are
  //
  int32_t diff = -(num1 < num2);
  std::cout << diff << "\n";
  int32_t mask = diff >> 31;
  std::cout << mask << "\n";

  return num2 ^ ((num1 ^ num2) & mask);
}

// and to expand further on that, since saturating the actual order execution
// pipeline becomes the main challenge, you would probably want a buy vector,
// sell vector, and maybe a hold vector(?), maybe just 2 though so that you can
// retain the benefits of the speed of L1 caching since being able to represent
// things with either a t/f, 1/0 or encoding whatever else information is key,
// and that would make that significantly more difficult, you would also want a
// watcher that keeps track of what is in each vector right? because the goal
// isnt to buy 1 tick and sell 1 tick later, thats a way to get eaten alive by
// slippage cost, and even if your making positive pnl trades on paper, the
// broker fees and taxes per trade would significantly cut into that, so you
// would probably want sell under x conditions, or a target thats like sell x
// ticks later assuming the price is above or below a certain thresh-hold, and
// using these same encoding techniques to watch whats in the vectors and
// compare with that? In that sense it wouldnt really be a pipeline, but you
// could probably group orders by 8 using a 64 bit int, but that leaves alot of
// information out, so the way you pack bits to make that work would have to
// have each bit essentially represent huge amounts of information i think, but
// it would lead to extremely fast execution vs using all 64 bits or 32 bits to
// represent the conditions for a single order, because using a 256 or 512 bit
// register you could fit like, 4 to 8 order books all exeucting and operating
// at once, with each one storing the information for 8 trades, this is fucking
// NUTS lol wtf, how would you even use a model to control this? Would this all
// be just hard coded instructions like buy vs sell given a hard cutoff thats
// programmed? like an inference module wouldnt even be able to keep up with
// that level of data flow lol, like for longer durations, maybe like 1m for
// just a few stocks, and like up to maybe 10-50 stocks, on 5m data, but for
// tick level LOL, the hardware limitations wouldnt allow for inference that
// fast would it? like maybe though, idk, id have to look more into that but
// holy shit thats insane lol, so i guess you use a model to control the hard
// constraints? How often would those change, do you only change them when you
// notice a decrease in performance? OMG THIS IS SO FUCKING COOL LOL I LOVE THIS
//
// like wtf, would you have like an overseer module that monitors performance,
// beacause when missing an order execution by like 3ns could lose you millions
// of dollars, like a human cant monitor that lmao, holy shit this is insane,
// how would you even optimize this lol, like i wanna know SO BAD
//
// if only leetcode was this entertaining lol, this shit is A M A Z I N G, like
// how am i expected to sit there and draw shapes in J A V A(ICK), when stuff
// like this exists lol, this isnt fair to me, can you spell A D D I C T I N G,
// lol like, im H O O K E D
//
int main() {
  int32_t num;
  int32_t num2;

  std::cout << "Please choose a num to see how this works: ";
  std::cin >> num;
  std::cout << "Please choose a num2: ";
  std::cin >> num2;

  std::cout << smallest(check_first(num), check_second(num2)) << "\n";

  return 0;
}
