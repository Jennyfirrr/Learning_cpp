// A professor referenced COBOL in one lecture, and a different professor
// brought up the limitations of floating point comparison, so I decided to look
// at an example of cobol. FIrst off, the all caps hurts my eyes, ew no. What
// was interesting though was how numbers are stored. I think its interesting
// that fixed point operations are baked into the COBOl language. like using the
// syntax 9(5)V99, means a variable that is 5 digits long, with a virtual
// decimal represented by the V, with 2 digits after the decimal, this allows
// for using all calculations to be in integers, which removes the variance that
// floating point comparisons can introduce, which while usually arnt that
// important, when youre dealing with hundreds of thousands to millions of
// buy/sell orders, or moving money around, it stacks up. This basically works
// by keeping the number 101.55, stored at 10155, so that it stays an integer,
// which removes the variance from FP operations, as well as increasing the
// speed at which operations can occur because integer math is always faster
// than FP math. There are issues where you may hit the INT_MAX size and stuff
// using methods like this, but before that becomes an issue solutions like
// using representations to signify larger numbers can be used to simulate them,
// like 1055, multiplied 10 times or something similar. I thought this was
// really intereting, and can see how it would be useful in the Quant/HFT world,
// as well as finance in general, and that these methods can be adopted in other
// languages.
//
// This is typically handled in cpp by using a struct to warp a value like
// int64_t because of the far large maxmimum value, over the regular INT_MAX.
// ALso, adding the decimals back in is only done when it needs to be read by a
// human, otherwise, keeping them stored as whole integers improves
// functionality and determinism.
