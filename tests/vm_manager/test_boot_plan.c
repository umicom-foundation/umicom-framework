/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_boot_plan.c
 * PURPOSE: Exercise boot target contracts and exact argument ownership without filesystem access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vm_manager/boot.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)
#ifdef _WIN32
#define ROOT_PATH "C:/VM practice/"
#else
#define ROOT_PATH "/tmp/VM practice/"
#endif

/* Deliberately nonexistent paths prove planning is independent of installed
 * QEMU and guest assets. Native review has separate file and child tests. */
static void Request(UmiVmBootRequest *request, UmiVmBootTarget target)
{
    (void)UmiVmBootRequestInit(request, target);
    strcpy(request->executable, ROOT_PATH "qemu");
    strcpy(request->workingDirectory, ROOT_PATH "runs");
    if (target == UMI_VM_BOOT_UMICOM_KERNEL) strcpy(request->firmware, ROOT_PATH "system.elf");
    else if (target == UMI_VM_BOOT_PC_ISO) strcpy(request->iso, ROOT_PATH "system.iso");
    else if (target == UMI_VM_BOOT_PC_DISK) strcpy(request->disk, ROOT_PATH "system.raw");
    else strcpy(request->kernel, ROOT_PATH "Image");
}

static const char *Value(const UmiVmBootPlan *plan, const char *key)
{
    for (size_t index = 0; index + 1U < UmiVmBootPlanArgumentCount(plan); ++index) {
        if (strcmp(UmiVmBootPlanArgument(plan, index), key) == 0)
            return UmiVmBootPlanArgument(plan, index + 1U);
    }
    return NULL;
}

static int Contains(const UmiVmBootPlan *plan, const char *text)
{
    for (size_t index = 0; index < UmiVmBootPlanArgumentCount(plan); ++index) {
        if (strcmp(UmiVmBootPlanArgument(plan, index), text) == 0) return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    UmiVmBootRequest request;
    UmiVmBootPlan *plan = NULL;
    const UmiVmBootTargetInfo *target = UmiVmBootTargetFind(name);
    if (target) {
        Request(&request, target->target);
        CHECK(UmiVmBootPlanCreate(&request, &plan) == UMI_STATUS_OK);
        CHECK(plan != NULL && UmiVmBootPlanArgumentCount(plan) <= UMI_CHANNEL_MAX_ARGUMENTS);
        CHECK(Contains(plan, "-S") && Contains(plan, "-no-reboot") && Contains(plan, "-nodefaults"));
        CHECK(strcmp(Value(plan, "-qmp"), "stdio") == 0);
        CHECK(strcmp(Value(plan, "-nic"), "none") == 0);
        CHECK(strcmp(Value(plan, "-monitor"), "none") == 0);
        CHECK(strcmp(Value(plan, "-accel"), "tcg") == 0);
        CHECK(strcmp(Value(plan, "-serial"), "chardev:console") == 0);
        if (target->target == UMI_VM_BOOT_UMICOM_KERNEL) {
            CHECK(strcmp(Value(plan, "-machine"), "virt,aclint=off") == 0);
            CHECK(strcmp(Value(plan, "-bios"), request.firmware) == 0);
            CHECK(Value(plan, "-kernel") == NULL && Value(plan, "-append") == NULL);
        } else if (target->target == UMI_VM_BOOT_LINUX_AARCH64) {
            CHECK(strcmp(Value(plan, "-cpu"), "cortex-a57") == 0);
            CHECK(strcmp(Value(plan, "-append"), "console=ttyAMA0") == 0);
            CHECK(Value(plan, "-bios") == NULL);
        } else if (target->target == UMI_VM_BOOT_LINUX_RISCV64) {
            CHECK(strcmp(Value(plan, "-bios"), "default") == 0);
            CHECK(strcmp(Value(plan, "-kernel"), request.kernel) == 0);
        } else if (target->target == UMI_VM_BOOT_LINUX_X86_64) {
            CHECK(strcmp(Value(plan, "-machine"), "q35") == 0);
            CHECK(strcmp(Value(plan, "-kernel"), request.kernel) == 0);
        } else if (target->target == UMI_VM_BOOT_PC_ISO) {
            CHECK(strstr(Value(plan, "-drive"), "format=raw,media=cdrom,readonly=on") != NULL);
            CHECK(Contains(plan, "ide-cd,bus=ide.0,drive=umicom_boot_cd,bootindex=1"));
        } else {
            CHECK(strstr(Value(plan, "-drive"), "format=raw,snapshot=on") != NULL);
            CHECK(Contains(plan, "virtio-blk-pci,drive=umicom_boot_disk"));
        }
        UmiVmBootPlanDestroy(plan);
        return 0;
    }
    Request(&request, UMI_VM_BOOT_UMICOM_KERNEL);
    if (strcmp(name, "catalogue") == 0) {
        CHECK(UmiVmBootTargetCount() == 6U);
        CHECK(UmiVmBootTargetFind(NULL) == NULL && UmiVmBootTargetFind("arm") == NULL);
        CHECK(UmiVmBootTargetAt(UmiVmBootTargetCount()) == NULL);
        for (size_t index = 0; index < UmiVmBootTargetCount(); ++index) {
            target = UmiVmBootTargetAt(index);
            CHECK(UmiVmBootTargetFind(target->name) == target);
        }
        return 0;
    }
    if (strcmp(name, "ownership") == 0) {
        CHECK(UmiVmBootPlanCreate(&request, &plan) == UMI_STATUS_OK);
        memset(&request, 'x', sizeof request);
        CHECK(strcmp(UmiVmBootPlanProgram(plan), ROOT_PATH "qemu") == 0);
        CHECK(strcmp(Value(plan, "-bios"), ROOT_PATH "system.elf") == 0);
        CHECK(UmiVmBootPlanArgument(plan, UmiVmBootPlanArgumentCount(plan)) == NULL);
        CHECK(UmiVmBootPlanArgument(NULL, 0) == NULL && UmiVmBootPlanArgumentCount(NULL) == 0U);
        UmiVmBootPlanDestroy(plan);
        UmiVmBootPlanDestroy(NULL);
        return 0;
    }
    if (strcmp(name, "drive-escaping") == 0) {
        strcpy(request.disk, ROOT_PATH "data,readonly=off,caf\xc3\xa9.raw");
        CHECK(UmiVmBootPlanCreate(&request, &plan) == UMI_STATUS_OK);
        CHECK(strcmp(Value(plan, "-drive"),
            "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file="
            ROOT_PATH "data,,readonly=off,,caf\xc3\xa9.raw") == 0);
        CHECK(Contains(plan, "virtio-blk-device,drive=umicom_boot_disk"));
        UmiVmBootPlanDestroy(plan);
        return 0;
    }
    if (strcmp(name, "linux-assets") == 0) {
        Request(&request, UMI_VM_BOOT_LINUX_RISCV64);
        strcpy(request.firmware, ROOT_PATH "opensbi.bin");
        strcpy(request.initrd, ROOT_PATH "initramfs");
        strcpy(request.disk, ROOT_PATH "root.raw");
        strcpy(request.commandLine, "console=ttyS0 root=/dev/vda rw");
        CHECK(UmiVmBootPlanCreate(&request, &plan) == UMI_STATUS_OK);
        CHECK(strcmp(Value(plan, "-bios"), request.firmware) == 0);
        CHECK(strcmp(Value(plan, "-initrd"), request.initrd) == 0);
        CHECK(strcmp(Value(plan, "-append"), request.commandLine) == 0);
        UmiVmBootPlanDestroy(plan);
        return 0;
    }
    if (strcmp(name, "null") == 0) {
        CHECK(UmiVmBootPlanCreate(NULL, &plan) == UMI_STATUS_INVALID_ARGUMENT && plan == NULL);
        CHECK(UmiVmBootPlanCreate(&request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiVmBootRequestInit(NULL, UMI_VM_BOOT_UMICOM_KERNEL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    /* Each rejection exercises a distinct boundary; plan creation must never
     * publish a partial object after validation fails. */
    if (strcmp(name, "missing-firmware") == 0) request.firmware[0] = 0;
    else if (strcmp(name, "relative-path") == 0) strcpy(request.firmware, "system.elf");
    else if (strcmp(name, "unterminated-path") == 0) memset(request.firmware, 'x', sizeof request.firmware);
    else if (strcmp(name, "control-path") == 0) strcpy(request.firmware, ROOT_PATH "bad\nname");
    else if (strcmp(name, "invalid-utf8") == 0) strcpy(request.firmware, ROOT_PATH "\xc0\xaf");
    else if (strcmp(name, "kernel-smp") == 0) request.processors = 2U;
    else if (strcmp(name, "memory") == 0) request.memoryMiB = 32769U;
    else if (strcmp(name, "kernel-memory") == 0) request.memoryMiB = 64U;
    else if (strcmp(name, "extra-initrd") == 0) strcpy(request.initrd, ROOT_PATH "unused");
    else if (strcmp(name, "unknown-target") == 0) request.target = (UmiVmBootTarget)99;
    else if (strcmp(name, "arm-firmware") == 0) {
        Request(&request, UMI_VM_BOOT_LINUX_AARCH64);
        strcpy(request.firmware, ROOT_PATH "unused");
    } else if (strcmp(name, "linux-missing-kernel") == 0) {
        Request(&request, UMI_VM_BOOT_LINUX_X86_64);
        request.kernel[0] = 0;
    } else if (strcmp(name, "command-control") == 0) {
        Request(&request, UMI_VM_BOOT_LINUX_X86_64);
        strcpy(request.commandLine, "console=ttyS0\nbad");
    } else if (strcmp(name, "command-unterminated") == 0) {
        Request(&request, UMI_VM_BOOT_LINUX_X86_64);
        memset(request.commandLine, 'a', sizeof request.commandLine);
    } else if (strcmp(name, "disk-iso-conflict") == 0) {
        Request(&request, UMI_VM_BOOT_PC_DISK);
        strcpy(request.iso, ROOT_PATH "unused.iso");
    } else return 2;
    CHECK(UmiVmBootPlanCreate(&request, &plan) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(plan == NULL);
    return 0;
}
