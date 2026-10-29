What was ruled out, so nobody repeats it
========================================

This file replaces a patch that was published here for a few hours and then
withdrawn. It is worth writing down why, because the reasoning that produced
it is convincing right up to the point where it meets a test.

The claim
---------

PixelFlinger assembles scanline routines into the heap and enters them through
a function pointer. When it crashed, the reports looked like this:

    PC=0x001a84b8  [guest: rw-] in 0000a000-001ad000 rw-p
    qemu: bytes at PC: 43f0 e92d 2344 e590 1348 e590
    qemu: LR=0xacf0afe4  libpixelflinger.so

Eight runs, eight different fault addresses, and the same four words at every
one of them. The instruction those words decode to — `ldr r4, [r0, #-0x423]`,
a large negative unaligned offset — is not something an assembler emits for a
scanline loop. From that it is a short step to: qemu is not executing these
bytes, it is executing a translation it built for this memory earlier, and the
`cacheflush` that should have dropped that translation was answered with a bare
success.

The step is plausible and it is wrong.

The test
--------

Two programs, both mapped RWX, both writing `mov r0,#7; bx lr`, calling it,
rewriting it to `mov r0,#9; bx lr`, calling `cacheflush`, and calling it again.
The second one enters through `mov lr,pc; ldr pc,[r4]` — the same shape as
`ggl_init_scanline` at 0xafdc/0xafe0 — rather than through a direct `blx`.

    direct call,   unmodified qemu-arm:  first 7, second 9   OK
    direct call,   patched qemu-arm:     first 7, second 9   OK
    indirect call, unmodified qemu-arm:  first 7, second 9   OK

qemu's own self-modifying-code tracking — the write protection on translated
pages — already covers both entry paths. `cacheflush` is not load-bearing here,
and the patch that made it invalidate unconditionally bought nothing while
charging Dalvik's JIT a page-collection lock on every compiled method.

So the fault signature above has some other cause. What it does not tell you is
that translations are stale; it only tells you the guest ended up at a heap
address whose contents do not execute as the code that was written there.

What this means for the next attempt
------------------------------------

The signature is still evidence. Eight runs, same bytes, different addresses,
energy concentrated in `libagl → libpixelflinger → libsurfaceflinger`, all of
it on the composite path. What it is evidence *of* is still open, and the next
step should be a measurement rather than another inference: dump the bytes
around the fault address together with the surrounding heap, and look at what
the assembler actually placed there before assuming what did.

If you are here looking for a fix, there is not one in this directory yet.