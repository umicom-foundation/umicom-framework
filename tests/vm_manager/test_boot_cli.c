/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_boot_cli.c
 * PURPOSE: Check command parsing without launching a process or reading guest inputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vm_manager/boot.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define ROOT_PATH "C:/VM practice/"
#else
#define ROOT_PATH "/tmp/VM practice/"
#endif

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    /* Mutable argv arrays match the public CLI contract. The callee borrows
     * their strings and must not rewrite them. The paths need not exist. */
    char *options[] = {"plan", "--target", "umicom-kernel", "--qemu", ROOT_PATH "qemu",
        "--directory", ROOT_PATH "runs", "--firmware", ROOT_PATH "system.elf",
        "--memory", "128", "--cpus", "1", NULL, NULL, NULL, NULL};
    int count = 13, expected = 2;
    if (strcmp(argv[1], "plan") == 0) expected = 0;
    else if (strcmp(argv[1], "duplicate") == 0) {
        options[count++] = "--memory"; options[count++] = "256";
    } else if (strcmp(argv[1], "unknown") == 0) {
        options[count++] = "--monitor"; options[count++] = "tcp:0.0.0.0:4444";
    } else if (strcmp(argv[1], "missing-value") == 0) --count;
    else if (strcmp(argv[1], "missing-review") == 0) options[0] = "run";
    else if (strcmp(argv[1], "bad-number") == 0) options[10] = "128garbage";
    else if (strcmp(argv[1], "overflow") == 0) options[10] = "184467440737095516160";
    else if (strcmp(argv[1], "duplicate-target") == 0) {
        options[count++] = "--target"; options[count++] = "linux-riscv64";
    } else if (strcmp(argv[1], "unused-option") == 0) {
        options[count++] = "--kernel"; options[count++] = ROOT_PATH "Image";
    } else if (strcmp(argv[1], "plan-with-review") == 0) {
        options[count++] = "--expect";
        options[count++] = "0000000000000000000000000000000000000000000000000000000000000000";
    } else return 2;
    int actual = UmiVmBootMain(count, options);
    if (actual != expected) {
        fprintf(stderr, "CLI exit %d, expected %d\n", actual, expected);
        return 1;
    }
    return 0;
}
