/*
 *  exit support for qemu
 *
 *  Copyright (c) 2018 Alex Bennée <alex.bennee@linaro.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include "qemu/osdep.h"
#include "accel/tcg/perf.h"
#include "gdbstub/syscalls.h"
#include "qemu.h"
#include "user-internals.h"
#include "qemu/plugin.h"
#include <sys/prctl.h>

#ifdef CONFIG_GCOV
extern void __gcov_dump(void);
#endif

/*
 * ★ КАК ПРОЦЕСС ГОСТЯ УШЁЛ — сам или от сигнала (регресс 25.09, Р-35).
 *
 * Диспетчер активностей о смерти процесса пишет одно и то же — «Process …
 * (pid N) has died», — ушёл ли тот сам (`System.exit(0)` у приёмника,
 * закончившего работу) или его убили. Кода выхода диспетчер не знает, а
 * зигота пишет только ненулевой. Летопись стенда звала смертью и выход по
 * своей воле: у одной прошивки — трижды за каждую загрузку.
 *
 * Знает точно только транслятор: выход идёт через exit/exit_group, а выход от
 * сигнала — через dump_core_and_abort, который выставляет guest_exit_signal.
 * Пишем одну строку «время_мс pid код|сигнал число имя» в файл из
 * GUEST_EXIT_LOG (его кладёт стенд); строку сверяет с «has died» летопись.
 *
 * ⚠️ SIGKILL сюда не доходит никогда — его ядро исполняет без нас. Поэтому
 * «записи нет» значит «убит снаружи», и это как раз смерть.
 * ⚠️ Файл открывается на каждый выход, с дозаписью: процессы гостя —
 * форки, и держать общий описатель через fork нельзя. Строка короче
 * PIPE_BUF и пишется одним write — строки разных процессов не смешиваются.
 */
int guest_exit_signal;

static void guest_exit_note(int code)
{
    const char *path = getenv("GUEST_EXIT_LOG");
    char name[17] = "";
    char line[128];
    struct timespec ts;
    int fd, n;

    if (!path || !*path) {
        return;
    }
    fd = open(path, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0600);
    if (fd < 0) {
        return;
    }
    clock_gettime(CLOCK_REALTIME, &ts);
    prctl(PR_GET_NAME, (unsigned long)name, 0, 0, 0);
    n = snprintf(line, sizeof line, "%lld %d %s %d %s\n",
                 (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000,
                 (int)getpid(),
                 guest_exit_signal ? "сигнал" : "код",
                 guest_exit_signal ? guest_exit_signal : (code & 0xff),
                 name);
    if (n > 0 && n < (int)sizeof line) {
        if (write(fd, line, n) < 0) {
            /* журнал — подсказка летописи, выходу он не помеха */
        }
    }
    close(fd);
}

void preexit_cleanup(CPUArchState *env, int code)
{
#ifdef CONFIG_GCOV
        __gcov_dump();
#endif
        guest_exit_note(code);
        gdb_exit(code);
        qemu_plugin_user_exit();
        perf_exit();
}
