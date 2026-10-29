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
// Changed: 2026-10-05

/* guest_proc.h — /proc гостя: в /proc/<наш pid>/task только его собственные потоки,
 * в /proc/<наш pid>/fd — без служебных описателей стенда (патч 0094). */
#ifndef GUEST_PROC_H
#define GUEST_PROC_H

#include <stdbool.h>

/* Открыли каталог: если это /proc/<наш pid>/task или …/fd — запомнить дескриптор. */
void guest_proc_open(const char *path, int fd);

/* Дескриптор закрыт — забыть. */
void guest_proc_close(int fd);

/* Прятать ли эту запись каталога от гостя? */
bool guest_proc_hide_dent(int dirfd, const char *name);

/*
 * ★ Р-116: путь /proc о НАШЕМ процессе с хвостом entry — /proc/<self|thread-self|наш
 * pid>/[task/<поток этого процесса>/]<entry>. Для подмен «своего» (maps, stat, exe, fd…).
 */
bool guest_proc_is_mine(const char *path, const char *entry);

/* ★ Р-116: тот же путь о НАШЕМ процессе — его хвост после «/proc/<кто>/[task/<tid>/]»; иначе NULL. */
const char *guest_proc_mine_rest(const char *path);

/*
 * ★ Р-116: путь /proc о любом процессе: true — *pid (группа потоков) и *rest (хвост после
 * «/proc/<кто>/[task/<tid>/]»); поток не этого процесса — false (пусть ответит ядро).
 */
bool guest_proc_owner_path(const char *path, long *pid, const char **rest);

#endif /* GUEST_PROC_H */
