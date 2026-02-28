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
// output from using [g++ -S -02] [c++filt < <file_name> | less], you can see
// the compiler pick specific registers for specific jobs, and knowing why eax
// keeps showing up as the return or why the loop counter lands in the exc
// register makes this way more readable, this shit is so amazing like wtf, ITS
// NEVER ENDING LIKE THERES ALWAYS SOMETHING NEW TO LEARN THAT TIES TO SOMETHING
// ELSE WTFFF
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
// IMPORTANT: cmovl is different from movl, cmovl means move if less than, while
// movl just means move long, because cmov infers from the register as pointed
// out in the intel vs AT&T syntax
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
//
// So, the J part of the JNE/JE/JMP just means jump, similar to how some of the
// other operators work, and im gonna kind list more of the suffixes here,
// specifically the condition code suffixes, because the sizing ones are gone
// over above:
//
// [e / z] equal/zero
// [ne/nz] not equal/not zero
// [g] is greater assuming the int is signed
// [ge] greater or equal (signed)
// [l] less(signed)
// [le] less or equal (signed)
// [a] above (unsigned)
// [ae] above or equal (unsigned)
// [b] below (unsigned)
// [be] below or equal (unsigned)
// [s] sign flag set(negative)
// [ns] sign flag not set
//
// for my use cases, the signed vs unsigned distinction matters, because g/l
// respect the sign bit, while a/b treat everything as unsigned, i MAY go over
// the floating point operators here, but floating point is I C K Y when fixed
// point math is a thing, so dont expect it
//
// apparently when i start diving into SIMD instructions, ill start to see
// epi8/epi16/epi32 suffixes which just indicated packed integer widths
//
//=================================================================================
// EDIT[25-02-26 09:59pm]
//=================================================================================
// SO, were gonna go over more ASM stuff rn, for funsies, because i think this
// stuff is N E A T O
//
// so, to start the night we have an appetizer that contains the subjects rsp
// and rbp, so lets put our napkins in our shirt collars, and start the causal
// conversations over the appetizer,
//
// so, this stuff relates to the stack, and as noted before in the
// memory_references.cpp file under new_notes currently, we know that [sub rsp,
// N] allocates in the stack, its fast, its ferocious, its deterministic, and
// its as stated before unadultered pointer management, we love that shit, so,
// rsp is basically the stack pointer, this register will A L W A Y S hold the
// address at the top of the stack, which may be a little confusing because it
// grows downwards or whatever, but in this case, the top basically means the
// last item you inserted into the stack, so conceptually, its the top, the CPU
// keeps track of this at all times, rain, sun, cloudy, stormy, night, you name
// it, the cpu knows where it is, and because of this, every push, pop, call,
// and ret, implicitly modifies the rsp register, because they change where the
// memory address of the top of the stack is, like push, adds something to the
// stack, so memory address change, pop removes, so memory address changes,
// call, i think uses the memory address, im not too sure on that one, but ret
// basically closes out the function that was using a stack, and as we know,
// once a function is done, the stack frame within it also collapses, and
// dissappears *poof*, so, whenever you see [sub rsp, 64] in asm code, that
// basically just means the code is saying "hey bucko, i need like 64 bytes of
// local variable space, can ya do that for me?", and because its sub, the
// register loses 64 bytes of space, so it moves the pointer down by 64 bytes,
// the reverse is [add rsp, 64] which basically creates space, like saying, hey
// this register need that 64 bytes back, ya donzo, and thats basically all that
// means, allocation and deallocation of the rsp register, no lists, no metadata
// headers, no fragmentation, just pure, white, booger sugar,
//
// now, we can move onto rbp, which is colloquially known as the base pointer,
// or the frame pointer, its basically the anchor point for the CURRENT
// functions stack frame, when you look at asm, youll basically see this pattern
// everytime a stack is used right:
//
// push rbp - save the OLD base pointer
// mov rbp, rsp - set YOUR base pointer to the current stack top
// sub rsp, N - make room for local variables
//
// and what this basically says is, push the current value in rbp to the stack,
// move the value in rbp to rsp then allocate space N for the stack,or for a
// better explanation, see the tags, and when it closes out, you get the reverse
//
// mov rsp, rbp - throw away local variables
// pop rbp - restore callers base pointer
// ret - jump back to caller (ew a jump(I C K Y))
//
// or sometimes just
//
// leave - this does the same thing just in one instruction, probably a modern
// invention
// ret (i think the other ones look cooler)
//
// anyways, the REASON this exists, is so that you have a fixed reference point
// within the stack frame, and that once you set rbp = rsp at the start, rbp
// doesnt move for the ENTIRE function, it must not have adhd, even as you push
// and pop other stuff, so the compuler can ALWAYS access your local variables
// as negative offsets from the value of rbp, and function arguments, that were
// pushed by the caller, as positive offsets, like [rbp - 8] is your first local
// variable, and [rbp - 16] is your second etc, just like how i showcased the
// good and bad structs within the memory_references file,
//
// BUT, this is where it gets interesting, apparently, when you compile using
// the [-O2] flag, the compiler often omits the frame pointer instruction
// entirely, thats really weird wtf,
//
/* .L6:
        subl	$1, %eax
        movq	(%r12), %r9
        xorl	%edi, %edi
        leaq	4(,%rax,4), %r8
        .p2align 4
        .p2align 3
.L9:
        leaq	(%r9,%rdi), %rdx
        xorl	%ecx, %ecx
        xorl	%esi, %esi
.L8:
        movsbl	(%rdx), %eax
        addq	$1, %rdx
        sall	%cl, %eax
        addl	$8, %ecx
        orl	%eax, %esi
        cmpl	$32, %ecx
        jne	.L8
        movl	%esi, (%rbx,%rdi)
        addq	$4, %rdi
        cmpq	%rdi, %r8
        jne	.L9
.L1:
        addq	$24, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 40
        movq	%rbp, %rax
        popq	%rbx
        .cfi_def_cfa_offset 32
        popq	%rbp
        .cfi_def_cfa_offset 24
        popq	%r12
        .cfi_def_cfa_offset 16
        popq	%r13
        .cfi_def_cfa_offset 8
        ret
        .p2align 4,,10
        .p2align 3
.L15:
        .cfi_restore_state
        pxor	%xmm0, %xmm0
        movq	$0, 16(%rdi)
        movq	%rbp, %rax
        movups	%xmm0, (%rdi)
        addq	$24, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 40
        popq	%rbx
        .cfi_def_cfa_offset 32
        popq	%rbp
        .cfi_def_cfa_offset 24
        popq	%r12
        .cfi_def_cfa_offset 16
        popq	%r13
        .cfi_def_cfa_offset 8
        ret

*/

// so i grabbed some asm code from the 06 asm file, and yeah, you can kinda see
// here, it doesnt have those, im guessing this is what the CFI offsets are for,
// i think i annotated one of those in one of the longer files, and its like the
// chik fla thing i referecned, where the Dwarf thingy, anyways, apparently it
// does this because with optimization on, the compiler just think "i can just
// track everything as offsets from rsp directly, and I get rbp back as a free
// general purpose register", and this is called frame pointer omission, oh
// thats really cool, im guessing this is a modern day optimization, and the GCC
// enables this by default at -O2, and yeah from what im reading this is why we
// see the .cfi offset lines, those are the offsets that the compiler inserted
// instead to track the variables and stuff instead of manually moving the
// pointers around i guess, OH RIGHT these are the DWARF unwinders, and its
// keeping track of where the frame is witout the rbp because the debugger still
// needs to know how to walk the stack even when the frame pointer is gonezo, so
// its basically a fancy way of saying, "at this point in the code, the
// canonical frame address is rsp + this_offset", so the unwinder can
// reconstruct the call chain, that so neat wtf, i wonder why this optimization
// was implemented, i guess to make readability easier, or like i read before,
// how it gives you the rsp back as free general register, god DAMN there is so
// much to learn lol, its lke the hole just keeps getting deeper, and i keep
// finding new cool shit,
//
// this is apparently good for hot paths, because you get an extra register,
// which is NICE, because theyre ricky bobby fast, the trade off though is that
// debugging gets way harder, but we arnt gonna be debugging mid trade, you run
// tests, simulations, and all that jazz before you EVEN think about letting
// code start placing trades, lol, because while a bad strategy may end up with
// like a 5-10% maxDD, bad exexecution can tank your account faster than you can
// blink lmao, but that gets more into a trading strategy, and exploting alpha,
// vs just the exectuon harness that actually controls the stream of trades,
// which I HAVENT GOTTEN TO YET
//
// CONTINUING, when you see something like push rax, its really just a shorthand
// way of saying, [sub rsp, 8] then [mov [rsp], rax], decrement the stack
// pointer, then store the value at the new top, one stone, 2 birds, ez pz,
// great value prices
//
// pop rax, is the reverse, its just [mov rax, [rsp]] then [add rsp, 8] read the
// value from the top, then move the pointer back up, call some_function is push
// rip(save the return address), then jmp(icky) some_function, thats why you see
// ret, its just pop rip, grab the return address off the top of the stackm then
// just jump your way back to the associated address, this is also apparently
// why stack overflows happen, if you keep calling functions without returning,
// you just keep pushing return addresses, and then the stack either overflows
// the available memory, or it meets where the heap ends, which is B A D, shit
// gets all fucky wucky, im probably gonna go over the rip register next, but
// idk, im just *sparkle emoji* F O L L O W I N G  M Y  H E A R T*sparkle
// emoji*, as everyone else should, and following wherever this leads me because
// this is honestly like opening up pandoras box lmao, like 20k words in like a
// WEEK, but i cant be assed to write a 500 page paper for my sociology class
// lmao, such is life right?
//
// one thing i was wrong about is that whereever i reference call, its worth
// noting that call pushes the address of the next instruction after the call,
// (not rip itself, but the value rip will have after the call instruction
// finishes), subtle but it apparently matters when i start reading return
// addresses in stack traces
//
//=================================================================================
// TODO[EFLAHS, rip register, SAR v SHR] (its nap time, im kinda cranky today
// anyways), probably gonna add a deeper dive into the DWARF unwinders here,
// because i wanna know exactly how the track the stack frame pointer to free up
// the rbp register or whatever it was, mem@PLT, _Znem@PLT, probably go over asm
// instructions more too, pxor, .cfi_remember_state, [.cfi_def_cfa_offset 24
// .cfi_offset 12, -24] i want to know more about the imediate offset being set,
// and then subtracted, maybe just all @PLT calls, i have no clue what those do
//=================================================================================
// EDIT[27-02-26 06:00pm]: UpDaTeS
//=================================================================================
// ALRIGHTY THEN, so after annotating and tearing apart then mycroft
// implementation, that i then swapped to a for loop, because it was having
// weird results, im gonna go over some of the TODO's here
//
// after reading some more about _ZdlPvm@PLT, and the other @PLT intrusctions,
// these are apparently just operator delete(void*, size_t), which is a sized
// dellocation, and the _Znwm@PLT is just operator new(size_t) which is the
// opposite i guess, for whatever reason, the compiler just calls this stuff by
// these names, apparently running the asm file through c++filt will convert
// these back to be human readable instead of whatever the fuck that is
//=================================================================================
// EDIT: the @PLT part stands for the procedure linkage table, which is the
// dynamic linkers indirection for calling shared library functions, the first
// call goes through the resolver, and any subsequent calls hit a cached
// address, THIS IS BAD BECAUSE WE WANNA USE THE CACHE ONLY FOR PACKED ORDERS,
// you would wanna avoid these in hot paths because they take up instruction
// cycles AND L1 cache space(I C K Y), you can either staticaly link, or use the
// CLA -fno-plt, ill probably go more into detail on this later but for now this
// is good enough for me
//=================================================================================
//
// the %fs:40 is just a stack canary, once again, i think i went over these in
// this or in like the 06 or 05 file, but i forgot about it, because its only
// been like a week since i started diving into this stuff, but anyways, what
// this does is that it loads a random value from thread-local storage at
// function entry, then checks it before the return value from the function, now
// the interesting thing happens if its overwritten, which in that case, the
// __stack_chk_fail gets called, and this is the stack smashing protection, not
// related to cin like i think i annotated in the mycroft implementation file,
// but i was just guessing there and now im going back over it to self correct,
// like a *sparkle emoji*S C I E N T E S T*sparkle emoji*
//
// so also in the mycroft implementation, there was somewhere i noted where
// there were 2 ret instructions in a single function, and it confused the HECK
// out of me, because like, whats the purpose lol? anyways, it actually is smart
// though, each ret function is a seperate exit path from the function, not
// multiple returns of different values, the compiler apparently duplicates the
// epilogue,(which is where the pop sequence + ret is), for different branches,
// so it avoids jump backs to within the function, apparently this is a size vs
// speed tradeoff, and it means that for that specific function, you have
// BRANCHING PATTERNS( I C K Y ),
//
// one thing to note about PDEP/PEXT, is that on the older AMD processors, the
// implementation is in microcode, so its basically just emulated, and not
// actually a hardware instruction, anything zen3+ is fine, and has around the
// same cycle count that intel processors have(3 cycles, give or take a few), so
// for code that runs anywhere a wrapper for functions that uses these and
// dispatches based on hardware would be ideal,but thats not SUPER important,
// because this would be tailored for specailized CPU's anyways, theres also a
// compuler CLA that is like -march= to target your specfic deployment hardware
//
// so onto the SAR vs SHR, the SHR is a logical shift, which just fills with
// 0's, and the SAR is the arithmatic shift, which i think i kinda covered this
// briefly earlier, but its good to re go over it anyways BECAUSE I FORGOT,
// because my brain is smooth and aerodynamic, just like a racecar, the only
// real difference from what i can tell is that SAR instructions preserves the
// sign bit, so when you compile with unsigned ints i guess it will tend to
// prefer SHR, whereas when you have a signed integer, the compiler emits an SAR
// because it has a sign bit and that has to be preserved because of how two's
// complement works, in bit maskes and gates, and using bool outputs to convert
// to a 1 or 0, the SAR is useful, otherwise unsigned is ALWAYS btter because
// you dont lose the highest 8 bits to nothing because you lose the last bit to
// a sign bit in signed ints
//
//=================================================================================
//[TODO] tzcnt, bsf instructions, intrinsics
//=================================================================================
//=================================================================================
