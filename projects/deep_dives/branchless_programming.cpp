#include <iostream>
#include <vector>

// were expanding on bitwise operators with branchless programming today, i may
// go back and make notes for all the operators, like &, |, ^, <<, >>, ?, and a
// few others before I do this, idk, im seeing where the rabbit hole takes me

// branch way, using methods like this are bad in latency critical systems
// because the CPU essentially tries to predcit which way the if/else statement
// will fall, and if it guesses wrong, your down 15-20 clock cycles or so, which
// for most things isnt that big of a deal, but when something as minor as a few
// nano seconds can cost you money, this is critical. The reason this happens is
// because if the predictor is wrong, the entire pipeline has to flush, and
// essentially rebuild these operations until it gets it right, which is B A D
/*
int clamp(int x) {
  if (x < 0) {
    return 0;
  }
  if (x > 255) {
    return 255;
  }
  return x;
}
*/

// thankfully, someone smarter than me laid the groundwork to use bitwise
// operators to avoid this issue by using *gasp* M A T H, so by using bit
// manipulation techniques, we can just not use if statements, and mathmatically
// figure it out in a fraction of the time, the example below is also why you
// use unsigned integers, because if you use a signed integer, negative numbers
// have a 1 at the leftmost bit, which removes a bit that could other be used
// for data, and the weird bugs that a signed integer overflow causes, which ill
// probably eventually add to this file, we dont like undefined behavior, almost
// as much as we dont like java, im gonna keep beating this dead horse btw, if
// your using my notes get used to it

int clamp_branchless(int x) {
  x &= ~(x >> 31);         // if x is negative, x >> 31 is all 1's, ~ is all 0's
  std::cout << x << " , "; // therefore x & 0 is 0.
  x -= 255;
  std::cout << x << " , "; // shifting the range
  x &= (x >> 31);
  std::cout << x << " , "; // reapply the sign because it will shift it back to
                           // the inverse again
  x += 255;
  std::cout << x << "\n"; // shift back
  return x;
} // to avoid magic numbers, you could use the parameters passed to the function
  // as the example below

int math_clamp_branchless_i32(int value, int low, int hi) {
  // rest of function, this prevents magic numbers and its more readable and
  // more geared towards a maintainable codebase
}

// Visual representation of how this works
// Ok, so lets assume x = -5, x >> 31, gives you 0xFFFFFFFF, which is all ones,
// because the shift fills the entire register with the sign bit,
// ~0xFFFFFFFF(not) flips these to all 0's, because its not 0xFFFFFFFF, which is
// 0x00000000, then it compares with the and operator, so -5 & 0 = 0, because
// its clamped to 0, in case 2, x = 300, so x >> 31 returns 0x00000000, ~0 =
// 0xFFFFFFFF, 300 & 0xFFFFFFFF = 300, so it passes through, x -= 255 = 45, x &=
// (x >> 31), 45 is positive, so x >> 31 = 0x00000000, 45 & 0 = 0, x += 255 =
// 255, x = 100, 100 is positive, so it passes through the first gate as 100,
// then x -= 255 = -155, x &= (x >> 31), is -155 >> 31, which ends up being
// 0xFFFFFFFF, so -155 & 0xFFFFFFFF = - 155, then -155 += 255 = 100, THIS IS SO
// COOL WTF LOL, literally just directly manipulating bits, and removing the
// need for if statements, actually W I L D, ill probably come back and add a
// better description, but i added a main function to see how it actually works
// step by step, i gotta let it M A R I N A T E some in mah brain
//
// More indepth explanation without using hexcode: This basically works because
// of Two's complement, and sign extension in a 32-bit system, a positive 5 is
// represented as 00000000_00000000_00000000_00000101, to get the negative
// value, your cpu uses the two's complement, which inverts the bits, then adds
// 1 to the end, so you end up with 11111111_11111111_11111111_11111010, then it
// adds one so you get 11111111_11111111_11111111_11111011 which represents -5,
// Now the reason this reads as a negative number, is because the very first bit
// is the sign bit, and because its a 1, the number is negative. So when you
// tell the computer to perform the x >> 31, if the number is positive, the new
// empty slots on the left fill with 0's, and if it is negative, the fill with
// 1's, so in the example where I use -5, the computer essentially pushes all
// the bits to the right, and the "fall off", and because the origianl bit on
// the left, was a 1, it fills all the empty bit slots with 1's. so after
// shifting 31 times, you end up with 11111111_11111111_11111111_11111111 which
// is 0xFFFFFFFF, as seen above, I just wanted to add an explanation that
// actually shows the binary representation of why this works, so because of the
// way this works, for any SIGNED integer, it will essentially take the sign bit
// and smear it across the entire register, leading to the clamping behavior
// seen in the function above.

int main() {
  int num;
  std::cout << "Please select a number to see how the bit shifting works: ";
  std::cin >> num;

  clamp_branchless(num);

  return 0;
}
