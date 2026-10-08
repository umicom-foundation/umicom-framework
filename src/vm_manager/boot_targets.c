/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_targets.c
 * PURPOSE: Describe guest boot protocols and reject incompatible input combinations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "boot_internal.h"

/* These are deliberate board/protocol choices. More QEMU architectures can be
 * added after their firmware and device requirements have an explicit policy. */
static const UmiVmBootTargetInfo BootTargets[] = {
    {UMI_VM_BOOT_UMICOM_KERNEL, "umicom-kernel", "riscv64", "qemu-system-riscv64",
     "Umicom system ELF as RV64 firmware; virt with ACLINT disabled"},
    {UMI_VM_BOOT_LINUX_X86_64, "linux-x86_64", "x86_64", "qemu-system-x86_64",
     "Direct Linux kernel boot on q35"},
    {UMI_VM_BOOT_LINUX_RISCV64, "linux-riscv64", "riscv64", "qemu-system-riscv64",
     "Direct Linux kernel boot on virt with OpenSBI firmware"},
    {UMI_VM_BOOT_LINUX_AARCH64, "linux-aarch64", "aarch64", "qemu-system-aarch64",
     "Direct Linux kernel boot on virt with a Cortex-A57 CPU"},
    {UMI_VM_BOOT_PC_ISO, "pc-iso", "x86_64", "qemu-system-x86_64",
     "BIOS-compatible x86-64 ISO through an explicit SATA CD-ROM"},
    {UMI_VM_BOOT_PC_DISK, "pc-disk", "x86_64", "qemu-system-x86_64",
     "BIOS-compatible x86-64 raw disk through a temporary overlay"}
};

size_t UmiVmBootTargetCount(void)
{
    return sizeof BootTargets / sizeof BootTargets[0];
}

const UmiVmBootTargetInfo *UmiVmBootTargetAt(size_t index)
{
    return index < UmiVmBootTargetCount() ? &BootTargets[index] : NULL;
}

const UmiVmBootTargetInfo *UmiVmBootTargetFind(const char *name)
{
    if (!name) return NULL;
    for (size_t index = 0; index < UmiVmBootTargetCount(); ++index) {
        if (strcmp(name, BootTargets[index].name) == 0) return &BootTargets[index];
    }
    return NULL;
}

/* Centralising defaults keeps Studio, the OS desktop and CLI plans identical.
 * Unknown targets leave the caller's request untouched. */
UmiStatus UmiVmBootRequestInit(UmiVmBootRequest *request, UmiVmBootTarget target)
{
    if (!request || target < UMI_VM_BOOT_UMICOM_KERNEL || target > UMI_VM_BOOT_PC_DISK)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(request, 0, sizeof *request);
    request->target = target;
    request->memoryMiB = target == UMI_VM_BOOT_UMICOM_KERNEL ? 128U : 1024U;
    request->processors = 1U;
    if (target >= UMI_VM_BOOT_LINUX_X86_64 && target <= UMI_VM_BOOT_LINUX_AARCH64) {
        strcpy(request->commandLine, target == UMI_VM_BOOT_LINUX_AARCH64
            ? "console=ttyAMA0" : "console=ttyS0");
    }
    return UMI_STATUS_OK;
}

/* Validate termination before any strlen or UTF-8 operation. Empty optional
 * paths are allowed, but relative paths and terminal control bytes are not. */
static int BootPath(const char *path, size_t capacity)
{
    if (!memchr(path, 0, capacity)) return 0;
    for (size_t index = 0; path[index]; ++index) {
        unsigned char byte = (unsigned char)path[index];
        if (byte < 32U || byte == 127U) return 0;
    }
    return VmPath(path, 1);
}

UmiStatus UmiVmBootRequestValidate(const UmiVmBootRequest *request)
{
    if (!request || request->target < UMI_VM_BOOT_UMICOM_KERNEL ||
        request->target > UMI_VM_BOOT_PC_DISK) return UMI_STATUS_INVALID_ARGUMENT;
    const char *paths[] = {request->executable, request->workingDirectory,
        request->firmware, request->kernel, request->initrd, request->disk, request->iso};
    for (size_t index = 0; index < sizeof paths / sizeof paths[0]; ++index) {
        if (!BootPath(paths[index], UMI_VM_PATH)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!request->executable[0] || !request->workingDirectory[0] ||
        request->memoryMiB < 16U || request->memoryMiB > 32768U ||
        !request->processors || request->processors > 16U ||
        !memchr(request->commandLine, 0, sizeof request->commandLine) ||
        !VmUtf8(request->commandLine, sizeof request->commandLine))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0; request->commandLine[index]; ++index) {
        unsigned char byte = (unsigned char)request->commandLine[index];
        if (byte < 32U || byte == 127U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Refuse unused fields rather than displaying a reviewed setting that the
     * emulator would never receive. Kernel mode follows its single-hart contract. */
    if (request->target == UMI_VM_BOOT_UMICOM_KERNEL) {
        if (!request->firmware[0] || request->kernel[0] || request->initrd[0] ||
            request->iso[0] || request->commandLine[0] || request->processors != 1U ||
            request->memoryMiB < 128U) return UMI_STATUS_INVALID_ARGUMENT;
    } else if (request->target == UMI_VM_BOOT_PC_ISO || request->target == UMI_VM_BOOT_PC_DISK) {
        if (request->kernel[0] || request->initrd[0] || request->commandLine[0] ||
            (request->target == UMI_VM_BOOT_PC_ISO && !request->iso[0]) ||
            (request->target == UMI_VM_BOOT_PC_DISK && (!request->disk[0] || request->iso[0])))
            return UMI_STATUS_INVALID_ARGUMENT;
    } else {
        if (!request->kernel[0] || request->iso[0] ||
            (request->target == UMI_VM_BOOT_LINUX_AARCH64 && request->firmware[0]))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
