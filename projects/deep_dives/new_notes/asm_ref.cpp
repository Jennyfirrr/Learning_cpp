// This is gonna serve as permenant ASM notes for how the cpp compiler compiles
// the code down into the specific asm portions because this stuff is I M P O R
// T A N T for avoidng branch prediciton(I C K Y), also because im gonna
// struggle to fit it in somewhere in this file, java is so bad it even has its
// own garbage collecter, the trash literally takes itself out lmao
//
// so in some other files i have here, there are parts that reference how the
// code compiles down into asm, and how that impacts wether or not the code is
// considered branchless or initializes the branch prediciton feature of the cpu
//
// this is meant to kinda go over the terms listed in asm so i can get a better
// understanding of them
//
// on x86_64: eax, ebx, ecx, edx are the 32 bit registers, and the 64bit are
// rax, rbx, rcx, rdx, and the 8bit lower portions are al, bl, cl, dl, which is
// why there is a file that references setl %al
//
// these registers actually have conventional purposes too, like the eax,rax, is
// called the accumulator, which is where return values go and where most
// arithmatic results land, so whenever you return something from a function,
// this is typically where it ends up
//
// the ecx/rcx registers is the counter which is used for loop counts and shift
// amounts, and im guessing when you write something like the standard for loop
// or (auto value : list), this is what it actually compiles down to in asm
//
// edx,rdx is the data register, which pairs with the *ax registers for
// multiplication and division results
//
// ebx/rbx is the base register, and its calle-saved(?) so it surives function
// calls
//
// ill probably write more about these later when I need to know more about them
// but for now this is fine for where im at, but for future reference im gonna
// go over these more:
// rsp[stack pointer], rbp[base pointer/frame pointer]
//
// the reason that these matter though is that when your watching the compiler
// output from using [g++ -S -02], you can see the compiler pick specific
// registers for specific jobs, and knowing why eax keeps showing up as the
// return or why the loop counter lands in the exc register makes this way more
// readable, this shit is so amazing like wtf, ITS NEVER ENDING LIKE THERES
// ALWAYS SOMETHING NEW TO LEARN THAT TIES TO SOMETHING ELSE WTFFF
//
// the registers are essentially the fastest memory available to the cpu, even
// faster than the l1 cache, its basically where your DATA goes when the cpu is
// actually using it
//
// so when something like cmovl %eax, %ebx, that basically means move data from
// the eax register to the ebx register, no cache hit, no ram hit(ICK), no bus
// transfer, its just the cpu shifting values that are actually stored within
// it,
//
// this is also why the lea eax, [ebx + ebx * 2] works, because its just dogin
// eax = ebx * 3 entirely within the registers, since this is a ebx + ebx * 2,
// you can only do odd multiples for this, so like *, *3, *5, *9,, or instead of
// multiples like a scaled factor like 1,2,4,8 just like the numbers when you
// play with your bits, after reading a bit more this trick is used to avoid
// calling the imul instruction, which takes more cycles, but i havent read much
// about that
//
// so apparently the things like movl $1, %edx is called AT&T syntax, but
// anyways the $1 basically means literally "put 1 into the edx register", $ is
// just a way to express literal, % is the symbol to use to say register,
//
// LFE#### just means Local Function End, and LFB#### means Local Function Begin
// this is what the lines that are like .cfi_startproc / .cfi_endproc means as
// well
//
// the tags for things like movl, addl,xorl,testl, notl, cmovne, cmovl, cmove,
// are just the suffix indicating what is being used, like l is the suffix for
// long which is 32-bit data, b is byte which is 8-bit, w is word which is
// 16bit, Ill update this part as I learn what different tags mean,
//
// cmp is compare, so like cmpl just means compare long, etc
