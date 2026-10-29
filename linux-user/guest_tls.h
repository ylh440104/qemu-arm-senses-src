// SPDX-License-Identifier: GPL-2.0-or-later
//
// ★ PART OF A MODIFIED QEMU. This file was ADDED to the QEMU sources
// (linux-user) by the ISTRATII_TECH SENSES project and is compiled into
// qemu-arm, therefore it is distributed under the terms of the GNU General
// Public License version 2 or (at your option) any later version.
//
// Full licence text: see COPYING in the project root.
// Everything changed in QEMU is listed in NOTICE.
//
//
// Published under GPLv2 section 3, because that is what the licence asks for
// when the compiled qemu-arm is handed to anyone. Until this tree was put up,
// the binary shipped without its corresponding source, and anyone who hit a
// crash in it had nothing to read: the binder daemon, the ashmem mapping and
// the translation invalidation are all here, and none of it was reachable.
// The header below already said the file is GPL; now the file itself is
// obtainable, which is the part that matters.
//
// Copyright (c) 2026 istratiitech <https://github.com/istratiitech>
// Changed: 2026-08-29

/* guest_tls.h — перехват чтения legacy-слота TLS. */
#ifndef GUEST_TLS_H
#define GUEST_TLS_H

#include "hw/core/cpu.h"

/*
 * Вызывается из cpu_loop_exit_sigsegv до превращения отказа в гостевой сигнал.
 * true — отказ обработан, состояние гостя поправлено, исполнение продолжается.
 */
bool guest_tls_fault(CPUState *cs, uint64_t addr, int is_load, uintptr_t ra);

#endif
