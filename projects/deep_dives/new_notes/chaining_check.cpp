#include <iostream>
#include <vector>

// WIP FOR ACTUALLY BUILDING ORDERBOOK LOGIC, i understand that its better to
// use a single reusable function, but im doing it this way to actually see how
// things operate across the entire process for better understanding
// *sparkle emoji*T R U S T  T H E  P R O C E S S*sparkle emoji* <3

int32_t check_0bit(int32_t num) { return (num & (1 << 0)) >> 0; }

int32_t check_1bit(int32_t num) { return (num & (1 << 1)) >> 1; }

int32_t check_2bit(int32_t num) { return (num & (1 << 2)) >> 2; }

int32_t check_3bit(int32_t num) { return (num & (1 << 3)) >> 3; }

int32_t check_4bit(int32_t num) { return (num & (1 << 4)) >> 4; }

int32_t check_5bit(int32_t num) { return (num & (1 << 5)) >> 5; }

int32_t check_6bit(int32_t num) { return (num & (1 << 6)) >> 6; }

int32_t check_7bit(int32_t num) { return (num & (1 << 7)) >> 7; }

// i was wrong about this, this way normalizes to one, because it first checks
// if the associated bit is active, then it shifts the value returend back to
// the 0 bit
//
// originially i was just using something like result = num & (1 << bit_index),
// which just returned the actual value of the bit, like for 7, returns 128,
// which isnt ideal, so i went back and used logic to normalize it to a 1 or 0
// using a right shift for a written out example see below
//
// so assuming you start with bit 7 in a number like 255, bit 7 represents 128
// so
//
// 1 1 1 1 1 1 1 1 = 255
// 1 0 0 0 0 0 0 0 = 1 << 7, then apply the & operator
// ---------------
// 1 0 0 0 0 0 0 0 = 128, so then the reason you wanna use the result >> 7, is
// because this normalizes it to either 1 or 0
//
// 1 0 0 0 0 0 0 0 >> 7 = 0 0 0 0 0 0 0 1 which equals 1, and allows for better
// and easier parsing, because it simply cuts it down to an on or off state, god
// playing with my bits is so much fun

/*
 * single function to check all states at once, but AS EXPLAINED IN MY LAST
 * EMAIL, im learning the process by actually writing out the functions bit by
 * bit, for a better M E N T A L  M O D E L, kthxbaiiiiii int32_t
 * extract_bit_state(int32_t packed_state, uint8_t bit_index) { return
 * packed_state & (1 << bit_index);
 * }
 */

std::vector<int32_t> build_vector_states(int32_t order_book0) {
  std::vector<int32_t> states(8);

  states[0] = check_0bit(order_book0);
  states[1] = check_1bit(order_book0);
  states[2] = check_2bit(order_book0);
  states[3] = check_3bit(order_book0);
  states[4] = check_4bit(order_book0);
  states[5] = check_5bit(order_book0);
  states[6] = check_6bit(order_book0);
  states[7] = check_7bit(order_book0);

  // basically this function, uses the 8 other functions, to build a vector that
  // shows which states are true or false, and im probably gonna try to build a
  // fail switch function next, that uses the vector as an input and checks for
  // the nth bit which i wanna encode as a kill switch, im also probably gonna
  // start looking into how to actually convert these to buy/sell statements but
  // based off of what i know about jump tables, it may be oky(?) to use a jump
  // table here because its basically a single clock cycle instead of the branch
  // prediction mess that if else would cause, i could be wrong, but im learning
  // as i go, so BEAR WITH ME PLEASE

  return states;
}

// so I was thinking about how a kill switch would work, and my original thought
// was like if/else, or case switching because thats what im used to, but
// apparently just using a mask, and clearing the bits is a beeter idea, because
// then nothing else triggers after the kill switch checks happen, and its
// resuseable to, so in theory you could add more kill switch checks, make
// differnt states dependent on different varibles, so that if anything
// misaligns it can be killed at any stage, or at least thats where my thought
// process is
//
// EDIT: [23-02-26 | 02:34am]
// so i did some further research about if statements and how they compile down
// to machine code, and apparently when you use just a single if statement, you
// can actually avoid the branch prediction as well, like assuming your using a
// loop, that constantly takes in an order book number from a vector, or however
// orders are fed to saturate a pipeline, you could potentially use things like
// if (state == 1) or something like if ((bit_mask & state) != 0) using a break
// statement, or soemthing else because these are apparently single operation
// cycles? specifically the first example, i think the 2nd one would consume 2
// cycles since your packing in the & operator as well as the comparison to 0,
// but you would have to do that in another part of code anyways, i think this
// would probably be worth looking at how it converts at compile time
//
// about the cycles consumed in the above, i was wrong, on x86 architecutre I
// was told, that the TEST instruction does a bitwise & and sets the zero flag
// in a single intstruction
//
// so like if((bit_mask & state) != 0) becomes
// TEST eax, ebx ;
// JNZ target    ;
//
// and
//
// if (state == 1) becomes
// CMP eax, 1
// JE target
//
// a note about the asm code here or machine code, im pretty sure this is asm,
// you can use the [g++ -S -02 -o -program_name.cpp | c++filt] and look for the
// function names, if you see TEST/AND/CMOV, your G O L D E N, and if you see
// JE/JNE/JMP, your code is B A D and you need to write less java, and study
// more branchless programming
//
// so the bitmask version isnt actually slower, theyre equivalent in cycle cost,
// but the bitmask version is more flexible, because you can check any bit
// position  without changing the instruction, which is ideal for what I wanna
// do
//
// So apparently the stuff i just wrote about the compile time behavior is based
// on likely hood for the compiler to emit a cmov, vs a JMP, when the body is
// simple enough, E X A M P L E
// if (state == 1) {
//	 do_function();
//}
// so apparently this syntax has a higher likely hood of being converted to a
// jump whereas
//
// int32_t result = (state == 1) ? value_a : value_b;
// is more like to be converted to a cmov(branchless), so apparently this is the
// ideal way to use if statements when branch prediction is a concern, im sure
// there are far more weird compiler things that you would need to know, but im
// just know scratching that surface deeper, maybe custom compiler settings or
// an inhouse compiler would be used that could always guarantee cmov over
// branch prediction paths would be ideal?
//
// One key insight is that its apparently not about how many ifs there are, but
// how complex what is inside them is, the simpler the statement is, the more
// likely it will be compiled to a cmov, its usually only converted to branch
// prediction when you call a function, or add complex logic, so while the way
// using the ? operator may look harder for a human to read, at compile time its
// computationally far simpler, and more direct

int32_t check_kill_switch(const std::vector<int32_t> &states,
                          int32_t kill_switch_index) {
  // udated to take constant reference instead of copying the vector, oopsies
  return ~(states[kill_switch_index]) & 1;
}

// so i did some more reading, and apparently this could be made better by doing
// something like packing all the states into a single byte using uint8_t
// E X A M P L E
/*
uint8_t sst_state_pack(const std::array<int32_t, 8> &states) {
  uint8_t packed = 0;
  for (int i = 0; i < 8; ++i) {
    packed |= (states[i] & 1) << i;
  }
  return packed;
}

if any kill switch bit is set here this fails
this changes it to a single operation, which is ideal
int32_t sst_kill_switch_apply(uint8_t packed_state, uint8_t kill_mask) {
        return (packed_state & kill_mask) == kill_mask;
}
*/
// using this method, the entire kill switch is one AND + one CMP, two
// instructions total, zero branches, and fully deterministic for cycle count
// and benchmarking, which is A M A Z I N G

int main() {
  int32_t order_book_sim;
  std::cout << "Please choose a number between 0 and 255: ";
  std::cin >> order_book_sim;

  const std::vector<int32_t> states_of_order_book =
      build_vector_states(order_book_sim);

  for (auto order : states_of_order_book) {
    std::cout << order << ", ";
  }

  std::cout << "\n";

  switch (check_kill_switch(states_of_order_book, 0)) {
  case 1:
    std::cout << "Stopping buy and selling current positions...";
    break;
  default:
    std::cout << "Placing buy/sell order...";
  }
  // so according to my other nothes, this should be fine, because of how the
  // case switching converts to a jump table after the compiler does its thing,
  // and the reason it should theoretically be fine, is because a jump table
  // stores the pointer to a memory address with the branching statements,
  // instead of having to initialize branch prediction paths, so its a single
  // cpu cycle to initiate the kill switch im guessing, by setting it to the 0
  // index bit, you can base the entire decision tree of what happens starting
  // from here, so if its a 0, that means the kill switch is not engaged, if its
  // a 1, it means kill the order, something is wrong, and im guessing you could
  // take this further by using either more bitwise operations in the actual
  // kill switch logic to make the index its reading dependent on a certain
  // other condition, or a switch statement again, because theyre fast too and
  // deterministc, right? I may try to implement that at some other point, but
  // im doing alot of theory right now instead of actual coding, to create a
  // solid foundation to work off of, try doing this in java lol, so apparently
  // its better to use the left most sign as the kill switch, because then you
  // can just do a sign check, which is a single instruction, so its WAY faster,
  // more at 11pm tonight on the local news
  //
  // EDIT: [23-02-26 3:01am] so i read some more about the sign as the kill
  // switch and WHY its good, on x86 architecture, after any arithmatic op the
  // sign flag(SF) is set automatically, so checking if int32_t is L I T E R A L
  // L Y  F R E E, its just a JS intstruction which is a jump if sign, and you
  // can do this branchless by using an arithmatic right shift using the
  // following:
  /*
  if bit 7 is the kill switch, packed state in int8_t is
  int32_t sst_kill_switch_apply(int8_t packed_state) {
  return packed_state >> 7;
  }
  */
  //
  // like thats literally it lol, i went more into detail about the kill switch
  // earlier in this file, but i feel this is noteworthy, this is good because
  // its a single SAR instruction, and while im not sure what SAR is atm, im
  // probably gonna learn about that more later, because this shit is S O  F R E
  // A K I N G  C O O L, and im just reading and asking questions to learn
  //
  // I was wrong about this being converted to a jump table, thats when there
  // are many conditions, when there are only like 2, its typically converted to
  // a cmov operation, which is fine, as outlined in prior notes, because it
  // avoids the prediciton branching, which is the entire point of what im
  // trying to learn lol

  std::cout << "\n";

  return 0;
}
