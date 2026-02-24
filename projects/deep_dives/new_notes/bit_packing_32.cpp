#include <array>
#include <iostream>
#include <random>
#include <vector>

// LOL HERE WE GO NOW IM COOKING LOL
//
// gonna annotate this later, and probably do an inline asm analysis as well
// this is just playing around trying to see how packing different order bits
// into a single 32 bit int works

std::vector<int32_t>
build_order_book(const std::vector<int8_t> &potential_trades,
                 int32_t queued_orders) {
  int32_t order_bit_packed = 0;
  std::vector<int32_t> order_book_packed((queued_orders + 3) / 4);

  for (int j = 0; j < (int)order_book_packed.size(); j++) {
    order_bit_packed = 0;

    for (int i = 0; i < 4; i++) {
      order_bit_packed |= potential_trades[j * 4 + i] << (i * 8);
    }
    order_book_packed[j] = order_bit_packed;
  }

  return order_book_packed;
  // the packing for this works as expected i THInk, but the reading part and
  // applying the kill mask could be better, right now if one trade fails, they
  // all fail, this would probably need to be split at execution time, but im
  // experimenting with the bitwise operators and bit packing as well as
  // encoding information, plus this is just so fun lol, like, this shit is W A
  // C K Y, one way around this would be instead of completely killing the
  // order, shift to a cpu core dedicated for failure checks, and possibly
  // unpack and run each one through a single 8bit gate, but by that time the
  // trades may be bad? idk, or grouping similar orders together? This is a head
  // scratcher lol
}

int32_t build_kill_mask(int8_t kill_mask_bits) {
  int32_t kill_mask_built = 0;

  // this should probably be renamed the risk gate
  // its not a true kill mask or kill switch
  // a kill mask would probably only be engaged if something went
  // catastrophically wrong, and would probably be engaged by a self monitoring
  // system for system health or something

  for (int i = 0; i < 4; i++) {
    kill_mask_built |= kill_mask_bits << (i * 8);
  }
  // use packed_orders & kill_mask == kill_mask
  //
  // SO I JUST FOUND OUT ABOUT de Brujin-style multiplication, where you can
  // apparently multiply your int8_t, or byte mask by 0x01010101, and the math
  // naturally places the byte into all four slots
  // ID0: 0x01
  // ID1: 0x100
  // ID2: 0x10000
  // ID3: 0x1000000
  //
  // the cpu does this in a single imul instruction instead of shifting the same
  // byte and using |= to place the byte ID in the correct place
  //
  // Toggling the mask is just using XOR tricks, inverting it is 0xFFFFFFFF
  // or just use the ~ operator lol
  //
  // You can use XOR for something called delta encoding, which is using the
  // old_order_state, and new_order_state, which when XOR'd tells you exactly
  // which bits changed, if the result is 0, nothing changed and you can just
  // send the next order through, this lets you bypass the entire logic gate, so
  // I guess to take advantage of this you would basically check the state of
  // the current order_book, and store it in a register pre kill_mask, and post
  // kill_mask, then if the next order book was the same, and nothing changed,
  // you check it against the post kill_mask order state, which is probably
  // faster because its just a bool check which when compiled converts down to a
  // bit check i believe(?)
  //
  // This is called loop unrolling, which is manually doing what the compiler
  // TRIES to do, because while the compiler is very VERY smart, it still has a
  // deterministic output, its not ai, and I was right about it being faster
  // because this goes from like 8+ instructions, down to 1-2 which is GOOD, i
  // love this, you love this, everyone loves this
  //
  // SO because i havent made notes about imul yet, and ive referenced it a few
  // times, H E R E  W E  G O, imul is the x86 instruction set for signed
  // integer multiplication, this comes in a few flavors(just like crayons, red
  // is my favorite flavor personally), also these are gonna be added to the asm
  // ref file, as well as here:
  //
  // One operand:
  // imul r/m : multiples the EAX register(or RAX), by the operand, storing the
  // result in EDX:EAX(or RDX:RAX) for the full double width result
  //
  // Two operand:
  // imul dest, src : multiplies dest by src, and stores the truncated result in
  // dest
  //
  // Three operand:
  // imul dest, src, imm : multiplies src by the immedediate value, stores in
  // dest
  //
  // WHY ITS IMPORTANT, in low latency systems, its relevant because when you do
  // fixed point arithmatic nstead of FP to maintain deterministic execution,
  // integer multiplay by imul has P R E D I C T A B L E, C O N S I S T A N T
  // latency, (on INTEL cpu's typically around 3 cycles on modern cores), no
  // rouding mode surprises, no denormal stalls, as a note, for signed ints, the
  // compiler like imul, for unsigned it will choose mul, For signed quantities
  // like P&L, position deltas, or anything that can go N E G A T I V E(like my
  // bank account), the compiler picks imul, The two operand, and three operand
  // forms discard the upper half of the result(no EDX:EAX split), which is fine
  // when you know the result fits in the 32/64bit range, but if you need
  // overflow detection, the one operand form gives you the full width result to
  // check against, im kinda surprised im still going with this lol, like these
  // are WHOLE ASS PAPERS IN THE COMMENTS, and im having T H E  T I M E  O F  M
  // Y  L I F E writing these, like this is so freaking cool, imagine using java
  // and not having to know any of this lol, couldnt be me lmao, Also heres
  // another Z I N G E R
  //
  // What does the JIT stand for?
  //
  // Java Is Trash H A H A H A H A H A(your supposed to laugh here)
  // ok back to being serious *wink*
  //
  // also, to address some errors within the code here, im using int8_t, which
  // is signed, so that means the values from 128-255 are getting interpreted as
  // negative numbers(i wonder if this caused the issue earlier where the cin
  // error occured with it interpreting 128 as 1), when you shift a N E G T I V
  // E, int8_t left, you get sign extension into the upper bits, which will
  // corrupt adjacent packed slots, i probably should use uint8_t for the trade
  // ID's, but thats why WE ARE LEARNING, and also the kill mask 128 is probably
  // killing all trades because it converts to -128, and when you shift this
  // left, it smears 1's across the entire packed register, which is probably
  // why results looked weird,

  return kill_mask_built;
}

int32_t kill_switch(int32_t packed_order_int, const int32_t &kill_mask_built) {
  // so theres apparently a better way to do this, apparently I could have just
  // checked each 8 bit lane seperatly using something like the following:
  //
  // bool lane0_killed = (packed & 0xFF) == (kill_mask & 0xFF);
  // bool lane1_killed = ((packed >> 8) & 0xFF) == ((kill_mask >> 8) & 0xFF);
  //
  // and so one, etc, this gives per lane granularity, without having to unpack
  // back to individual 8bit values, in a real system, this is where you would
  // wanna use SIMD(_mm_cmpeq_epi8), but im just a baby engineer, so thats kinda
  // out of my expertise, but ILL LEARN, toodles
  //
  // also please clone my theme repo *pleading eyes emoji* it would make me SO
  // HAPPY
  //
  //
  return (packed_order_int & kill_mask_built) == kill_mask_built;
}

int main() {
  int32_t generate_x_orders;
  int32_t temp_kill_mask;

  std::cout << "Generate orders amount: ";
  std::cin >> generate_x_orders;
  generate_x_orders = generate_x_orders * 4;

  std::cout << "Select a kill mask id #[0-255]: ";
  std::cin >> temp_kill_mask;

  int8_t kill_mask_bits_id = static_cast<int8_t>(temp_kill_mask);
  // apparently when you use cin to grab an int8_t int, it reads it as a
  // character, and not as a number, so when you entered 1, or 128, it was
  // reading BOTH of them as 1 lol, thats so weird
  // using 128 as the kill mask id will block 100% of trades becaues its
  // basically checking every bit that is packed at the 7th bit, im sure theres
  // a bug in my kill switch logic, and i need to work it out, but thats HOW WE
  // LEARN RIGHT???, this is just a testing harness i slaped together in the
  // main function, ideally you would probabably just wanna use a class or
  // something that wraps everything else together in a big warm hug, but im
  // kinda bad at coding and citadel doesnt wanna reply to me becuase im BaD aT
  // lEeTcOdE

  int32_t kill_mask_built = build_kill_mask(kill_mask_bits_id);

  std::mt19937 rng(21); // deterministic fixed seed
  std::uniform_int_distribution<int> dist(0, 255);

  std::vector<int8_t> potential_trades(generate_x_orders);

  for (int i = 0; i < generate_x_orders; i++) {
    potential_trades[i] = static_cast<int8_t>(dist(rng));
  }

  auto packed = build_order_book(potential_trades, generate_x_orders);

  int32_t valid_trades = 0;
  int32_t killed_trades = 0;

  for (auto &order : packed) {
    int32_t result = kill_switch(order, kill_mask_built);
    killed_trades += result;
    valid_trades += (1 - result);
  }

  std::cout << "Successful trades: " << valid_trades << "\n";
  std::cout << "Failed trades: " << killed_trades << "\n";
  std::cout << "Success rate: "
            << 100 - ((static_cast<float>(killed_trades) /
                       (killed_trades + valid_trades)) *
                      100)
            << "%" << "\n";
  // so glad i remembered what i forgot, this file felt like it was missing
  // something
  std::cout << "Java bad\n";

  return 0;
}
