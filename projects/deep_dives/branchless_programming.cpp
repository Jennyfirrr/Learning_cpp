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

int clamp(int x) {
  if (x < 0) {
    return 0;
  }
  if (x > 255) {
    return 255;
  }
  return x;
}

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
  x &= ~(x >> 31); // if x is negative, x >> 31 is all 1's, ~ is all 0's,
                   // therefore x & 0 is 0.
  x -= 255;        // shifting the range
  x &= (x >> 31);  // reapply the sign because it will shift it back to the
                   // inverse again
  x += 255;        // shift back
  return x;
}
