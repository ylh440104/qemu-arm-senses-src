#ifndef QEMU_PATH_H
#define QEMU_PATH_H

void init_paths(const char *prefix);
const char *path(const char *pathname);
/* Корень, заданный ключом -L: нужен, чтобы передать его дочернему qemu. */
const char *path_prefix(void);
/*
 * ★ Гость не выходит из своего дерева (регресс 24.09, Р-2): относительные
 * имена — от рабочего каталога гостя, вызовы с каталогом-описателем — от его
 * гостевого пути, ответы readlink — гостевыми путями.
 */
const char *path_at(int dirfd, const char *pathname);
int path_chdir(const char *pathname);
void path_fchdir_done(int fd);
ssize_t path_getcwd(char *buf, size_t size);
ssize_t path_link_to_guest(char *buf, ssize_t len, size_t cap);
const char *path_link_target(const char *target);
char *path_to_guest(const char *host);
/* ★ Патч 0094: описатель с этой целью — служебное стенда, гостю не показывать. */
bool path_is_stand_plumbing(const char *host);
/*
 * ★ Сами ссылки /proc/<процесс>/root и …/cwd (Р-24): readlink отдаёт их цель
 * гостевым путём, lstat видит ссылку, а не каталог, в который она ведёт.
 */
int path_proc_readlink_at(int dirfd, const char *pathname, char **target);
const char *path_nofollow_at(int dirfd, const char *pathname);

#endif
