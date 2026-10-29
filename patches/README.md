Patches on top of the SENSES tree
=================================

The tree in this repository is the one the shipped `qemu-arm` is built from.
Patches here are applied on top of it, not on top of upstream QEMU.

They exist for one reason: the binary is what people actually run, and a bug
in it can only be fixed here. Editing the tree in place and rebuilding works
too — these files are for keeping the change legible and separate.

Applying
--------

    git am patches/*.patch          # keeps author and message
    # or
    patch -p1 < patches/0001-cacheflush.patch

Both were checked against the tree as published.

What is here
------------

**0001-cacheflush.patch** — `linux-user/arm/cpu_loop.c`

`cacheflush` is the syscall a program uses to say "the code at this address
changed, drop whatever you translated from it". qemu's answer to it was a bare
success with the request dropped, on the grounds that Dalvik's JIT calls it for
every method it compiles and the invalidation costs something.

The reasoning holds for the JIT. It does not hold for a program that assembles
code and enters it through a function pointer, which is what the syscall is
for. PixelFlinger is that program on Android 1.0–2.x: it builds a scanline
routine in the heap, announces it, jumps in.

Seen from outside, the ignored announcement looks like this:

    PC=0x001a84b8  [guest: rw-] in 0000a000-001ad000 rw-p
    qemu: bytes at PC: 43f0 e92d 2344 e590 1348 e590
    qemu: LR=0xacf0afe4  libpixelflinger.so

The address is different on every run; the bytes at that address are identical
to the byte; and the instruction that faults is not one the assembler emits.
That combination means the guest ran a translation built before the code was
written there — not the bytes it just produced.

The patch invalidates the named range unconditionally. Cost was the original
objection, so it is worth being exact about where the cost lands: a page that
was just written has no translation yet, `tb_invalidate_phys_range` finds no
PageDesc for it and returns without taking the page collection lock. A page the
JIT has already executed has one, and pays. The bill goes to the JIT, which is
where the original decision wanted it, and not to the assembler, which cannot
work without it.

`GUEST_TBFLUSH` still overrides for measurement: unset for the behaviour above,
`1` for a full `tb_flush` on every call.

Not fixed by this
-----------------

Whether the patch is enough is not established. An assembly routine can be
entered on a thread while another thread holds a translation of the same
addresses, and user mode has no way to stop a thread in the middle of a block;
that race is what the comments in `cpu_loop.c` describe from earlier attempts.
PixelFlinger calls this from one thread, so the race should not apply to it —
but that is an expectation, not a measurement.