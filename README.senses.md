qemu-arm, as built for AEmulator / SENSES
=========================================

This is the complete source tree of QEMU 8.2.4 as modified for the SENSES
emulator engine — the tree that `linux-user/guest_*.c` lives in, and the tree
that the `qemu-arm` binary shipped to users is built from.

Why it is here
--------------

QEMU is licensed under GPLv2. Section 3 of that licence is not optional: if
you distribute a binary, you owe the recipient the corresponding source. The
`qemu-arm` binary has been distributed with builds of the emulator for a long
time, and the source of the modifications was not published alongside it. That
puts every user who wanted to understand a crash, or fix one, at a
disadvantage — including the ones who ran into the binder and PixelFlinger
problems these very files are responsible for.

That is not a moral lecture, it is just how the licence works, and it is why
this tree exists. Claims of the form "the source is somewhere" are not a
licence term; the source has to actually be reachable by the person who got
the binary.

What is in here
---------------

Upstream QEMU 8.2.4, plus:

  * 19 new files in `linux-user/` (`guest_*.c`) with the headers that go with
    them — binder, ashmem, fb, fs, net, proc, oom, rlimit, selinux, script,
    persona, ids, input, alarm, kill, tls, fwinit, trace, pagelog.
  * Edits to upstream files where those additions hook in: `syscall.c`,
    `mmap.c`, `elfload.c`, `main.c`, `linuxload.c`, `arm/cpu_loop.c`,
    `accel/tcg/user-exec.c`.

See NOTICE for the full list and for the attribution that each added file
already carries in its own header. Every one of those files declares
`SPDX-License-Identifier: GPL-2.0-or-later` and names its copyright holder;
this publish keeps those headers intact and adds nothing to them.

Fixes on top of this tree
-------------------------

`patches/` is where changes made after this publish would go. It currently
holds no patch: one was written, tested, and withdrawn, and `patches/README.md`
records what it claimed, why the claim was plausible, and the measurement that
falsified it. Read that before assuming a crash here is a stale-translation
problem.

How to build
------------

    mkdir build && cd build
    ../configure --target-list=arm-linux-user --static
    ninja qemu-arm

Requirements are the usual ones for a static user-mode build: a C compiler,
make or ninja, and glibc or musl with static libraries. The build takes a few
minutes from scratch and seconds incrementally.

Notes on the code you are about to read
---------------------------------------

The comments in these files are thorough — unusually so. They record, in
detail, which approach was tried, what it broke, and why it was abandoned:
cacheflush invalidation that stayed in for half a day and was taken out again,
a page-protection scheme for self-modifying code that would not let the
emulator start, a trap on writes that missed the freshly written code. That is
genuinely useful material, and it is readable in translation.

What those comments do not do is let someone else build the binary. For that
you need the files themselves, which is the point.

If a copyright holder named in these files wants the attribution phrased
differently, open an issue. The intention here is to comply with the licence
and to let people fix their own crashes.
