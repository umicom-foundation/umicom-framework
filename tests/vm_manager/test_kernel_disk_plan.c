/*-----------------------------------------------------------------------------
 *
 * File: tests/vm_manager/test_kernel_disk_plan.c
 *
 * PURPOSE: Check scratch-disk argument policies and refusal without changing caller data.
 *
 * LICENCE: MIT
 *
 * Umicom Framework - native C23 Kernel scratch-disk QEMU policy regression.
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 *
 * Deliberately uses a fixed, fully owned argv buffer; no process or image is
 * opened. Tests verify that ordinary arguments are unchanged on refusal.
 *---------------------------------------------------------------------------*/
#include "kernel_disk_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARGS 32U
#define CAP 4096U
#define REQUIRE(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

static char arguments[ARGS][CAP];
static size_t number;
static size_t assertions;

static void Reset(void)
{
    memset(arguments, 0, sizeof arguments);
    const char *const base[] = {
        "-machine", "virt,aclint=off", "-drive",
        "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file=C:/new scratch/disk.raw",
        "-device", "virtio-blk-device,drive=umicom_boot_disk", "-S", "-no-reboot"
    };
    number = sizeof base / sizeof base[0];
    for (size_t i = 0U; i < number; ++i) strcpy(arguments[i], base[i]);
}

static int Check(int value)
{
    ++assertions;
    return value;
}

static int TestSuccess(void)
{
    Reset();
    REQUIRE(Check(UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    REQUIRE(Check(number == 10U));
    REQUIRE(Check(strstr(arguments[3], "readonly=off") != NULL));
    REQUIRE(Check(strstr(arguments[3], "snapshot=on") == NULL));
    REQUIRE(Check(strstr(arguments[3], "cache=writeback") != NULL));
    REQUIRE(Check(strstr(arguments[3], "file=C:/new scratch/disk.raw") != NULL));
    REQUIRE(Check(strstr(arguments[5], "write-cache=on") != NULL));
    REQUIRE(Check(strcmp(arguments[8], "-global") == 0));
    REQUIRE(Check(strcmp(arguments[9], "virtio-mmio.force-legacy=false") == 0));
    Reset();
    REQUIRE(Check(UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 1)));
    REQUIRE(Check(strstr(arguments[3], "readonly=on") != NULL));
    REQUIRE(Check(strstr(arguments[3], "werror=report") == NULL));
    REQUIRE(Check(strstr(arguments[3], "cache=writeback") != NULL));
    /* An ordinary snapshot plan is NOT changed until the qualifier calls
     * the helper with trusted owned storage. */
    Reset();
    REQUIRE(Check(strstr(arguments[3], "snapshot=on") != NULL));
    return 0;
}

static int Refusal(void)
{
    char before[ARGS][CAP];
    size_t original = number;
    memcpy(before, arguments, sizeof arguments);
    REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    REQUIRE(Check(number == original));
    REQUIRE(Check(memcmp(before, arguments, sizeof arguments) == 0));
    return 0;
}

static int TestFailures(void)
{
    Reset(); strcpy(arguments[3], "if=none,id=wrong,file=other"); REQUIRE(Refusal() == 0);
    Reset(); strcpy(arguments[5], "virtio-blk-device,drive=unknown"); REQUIRE(Refusal() == 0);
    Reset(); strcpy(arguments[3], "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file="); REQUIRE(Refusal() == 0);
    Reset(); strcpy(arguments[7], "-global"); REQUIRE(Refusal() == 0);
    Reset(); strcpy(arguments[7], "virtio-mmio.force-legacy=false"); REQUIRE(Refusal() == 0);
    Reset(); strcpy(arguments[6], "-drive"); strcpy(arguments[7], arguments[3]); REQUIRE(Refusal() == 0);
    Reset(); strcpy(arguments[6], arguments[5]); REQUIRE(Refusal() == 0);
    Reset(); memset(arguments[3], 'X', CAP); REQUIRE(Refusal() == 0);
    Reset(); REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, NULL, ARGS, CAP, 0)));
    Reset(); REQUIRE(Check(!UmiVmKernelDiskPatchArguments(NULL, &number, ARGS, CAP, 0)));
    Reset(); REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 2)));
    Reset(); REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, 128U, 0)));
    Reset(); REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, 20000U, 0)));
    Reset(); REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, 9U, CAP, 0)));
    Reset(); number = 2U; REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    Reset(); number = ARGS + 1U; REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    Reset(); number = ARGS - 1U; REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    Reset(); number = ARGS - 2U; REQUIRE(Check(UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    /* Distinguish comma escaping inside an already validated QEMU filename
     * from a QEMU option injection attempt. Existing path escape is retained. */
    Reset(); strcpy(arguments[3],
        "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file=C:/scratch/a,,b.raw");
    REQUIRE(Check(UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, CAP, 0)));
    REQUIRE(Check(strstr(arguments[3], "file=C:/scratch/a,,b.raw") != NULL));
    /* Failed-size preflight may not mutate earlier argument elements. */
    Reset();
    char before[ARGS][CAP];
    memcpy(before, arguments, sizeof arguments);
    REQUIRE(Check(!UmiVmKernelDiskPatchArguments(arguments, &number, ARGS, 159U, 0)));
    REQUIRE(Check(memcmp(before, arguments, sizeof arguments) == 0));
    return 0;
}

int main(void)
{
    REQUIRE(TestSuccess() == 0);
    REQUIRE(TestFailures() == 0);
    printf("UMICOM_KERNEL_DISK_PLAN_READY assertions=%zu\n", assertions);
    return 0;
}
