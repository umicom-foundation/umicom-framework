/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/session_child.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/prctl.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr, "session child: %s\n", #c); return 1; } } while (0)
int main(void)
{
    REQUIRE(getuid() != 0 && geteuid() == getuid());
    REQUIRE(getenv("UMICOM_DESKTOP_SESSION") && !strcmp(getenv("UMICOM_DESKTOP_SESSION"), "1"));
    REQUIRE(getenv("XDG_CURRENT_DESKTOP") && !strcmp(getenv("XDG_CURRENT_DESKTOP"), "Umicom"));
    REQUIRE(!getenv("UMICOM_TEST_SECRET") && !getenv("LD_LIBRARY_PATH") && !getenv("LD_PRELOAD"));
    REQUIRE(getenv("PATH") && !strcmp(getenv("PATH"), "/usr/local/bin:/usr/bin:/bin"));
    REQUIRE(prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0) == 1);
    char cwd[4096]; REQUIRE(getcwd(cwd, sizeof cwd)); REQUIRE(getenv("HOME") && !strcmp(cwd, getenv("HOME")));
    DIR *directory = opendir("/proc/self/fd"); REQUIRE(directory);
    int own = dirfd(directory), extra = 0; struct dirent *entry;
    while ((entry = readdir(directory))) {
        char *end = NULL; long descriptor = strtol(entry->d_name, &end, 10);
        if (end != entry->d_name && *end == 0 && descriptor >= 3 && descriptor != own) ++extra;
    }
    closedir(directory); REQUIRE(extra == 0);
    printf("SESSION_CHILD uid=%lu backend=%s pid=%lu\n", (unsigned long)getuid(), getenv("XDG_SESSION_TYPE"), (unsigned long)getpid());
    return 0;
}
