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
// %rsi is one of the general purpose registers, it has multiple portions laid
// out like
//
// %rsi = full 64 bit
// %esi = lower 32 bit
// %si = lower 16 bit
// %sil = lower 8 bit
//
// historically the %rsi stood for "source index" and was originally used for
// the source pointer for string operations like rep movsb(memory copy), but in
// modern x86_64, its just a general purpose register, the only place source
// still means anything here is that its the second function argument in the
// system V calling convention, which only matters in (Linux(arch btw)/macOS)
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
// b = byte = 8 bit
// w = word = 16bit
// l = long = 32 bit
// q = quad = 64 bit
// These are the AT&T syntax suffixes, INTEL doesnt use these since the operand
// size is inferred from the register names
//
// the actual logic names are just what you call the operators like ==, ~, ^, &,
// etc within the higher level languages, its just literally like cmp for ==,
// xor for ^, not for ~, etc
//
//====================
//   btl	%eax, %esi
//   setc	(%rdx)
//====================
// in this example, the btl means bit test long : test bit [eax] of esi, result
// goes into CF the setc part means store CF (0 or 1 because its testing the
// bit) into memory at address in rdx
//
// ZF = zero flag
// CF = carry flag
// SF = sign flag
// OF = overflow flag
//
// cmp is compare, so like cmpl just means compare long, etc
//
// JNE/JE/JMP are the syntax for branches being used instead of branchless logic
// in asm, when reading that code, first identify if the function is on a
// critical hot path, because for somethings the jumps dont really matter like
// initialization code, etc, but on the hot path avoiding this is critical as
// outlined in the 03 file, for loops use these, but its mostly fine because
// theyre predictable like 99.9% of the time especially in cases where they
// simply count up by 1, the only real risk there is when you read the end case
// that stops the for loop, but even then, 1 mistiming there isnt SUPER bad, its
// just not ideal in my mind, in production use cases this may be largely
// ignored because of just how low the variance this would cause is, also for
// loops probably shouldnt be used for hot paths
