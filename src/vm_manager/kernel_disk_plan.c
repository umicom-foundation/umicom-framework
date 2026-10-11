/*-----------------------------------------------------------------------------
 * Umicom Framework - bounded transformation of owned QEMU arguments.
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 *
 * The normal VM BootDrive produces a temporary overlay. Only the tightly
 * scoped Kernel qualification service, AFTER copying a known immutable
 * fixture into a fresh scratch directory, calls this function. No arbitrary
 * user-supplied QEMU options or device paths are accepted here.
 *---------------------------------------------------------------------------*/
#include "kernel_disk_plan.h"
#include <stdio.h>
#include <string.h>

#define UMI_KERNEL_DRIVE_PREFIX "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file="
#define UMI_KERNEL_DEVICE_ORIGINAL "virtio-blk-device,drive=umicom_boot_disk"
#define UMI_KERNEL_DEVICE_QUALIFIED \
    "virtio-blk-device,drive=umicom_boot_disk,write-cache=on," \
    "logical_block_size=512,physical_block_size=512,num-queues=1"

static char *PlanArgument(void *vector, size_t index, size_t capacity)
{
    return (char *)vector + index * capacity;
}

int UmiVmKernelDiskPatchArguments(void *arguments, size_t *argumentCount,
    size_t argumentCapacity, size_t itemCapacity, int readOnly)
{
    if (!arguments || !argumentCount || (readOnly != 0 && readOnly != 1) ||
        itemCapacity < 160U || itemCapacity > 16384U ||
        argumentCapacity < 4U || *argumentCount < 4U ||
        *argumentCount > argumentCapacity - 2U) return 0;
    const size_t count = *argumentCount;
    size_t drive = count, device = count;
    for (size_t i = 0U; i < count; ++i) {
        char *const arg = PlanArgument(arguments, i, itemCapacity);
        if (!memchr(arg, '\0', itemCapacity)) return 0;
        if (strcmp(arg, "-drive") == 0 && i + 1U < count &&
            strncmp(PlanArgument(arguments, i + 1U, itemCapacity),
                UMI_KERNEL_DRIVE_PREFIX, sizeof UMI_KERNEL_DRIVE_PREFIX - 1U) == 0) {
            if (drive != count) return 0;
            drive = i + 1U;
        }
        if (strcmp(arg, UMI_KERNEL_DEVICE_ORIGINAL) == 0) {
            if (device != count) return 0;
            device = i;
        }
        if (strcmp(arg, "-global") == 0 ||
            strcmp(arg, "virtio-mmio.force-legacy=false") == 0) {
            /* Do not accept a potentially conflicting pre-existing policy. */
            return 0;
        }
    }
    if (drive == count || device == count) return 0;
    const char *const old = PlanArgument(arguments, drive, itemCapacity);
    const char *const escapedPath = old + sizeof UMI_KERNEL_DRIVE_PREFIX - 1U;
    if (!*escapedPath) return 0;
    char rewritten[16384];
    const int length = snprintf(rewritten, sizeof rewritten,
        readOnly ? "if=none,id=umicom_boot_disk,format=raw,readonly=on,"
                   "cache=writeback,rerror=report,file=%s" :
                   "if=none,id=umicom_boot_disk,format=raw,readonly=off,"
                   "cache=writeback,rerror=report,werror=report,file=%s",
        escapedPath);
    if (length <= 0 || (size_t)length >= itemCapacity ||
        strlen(UMI_KERNEL_DEVICE_QUALIFIED) >= itemCapacity ||
        strlen("virtio-mmio.force-legacy=false") >= itemCapacity) return 0;
    /* Commit only after every size and structural check succeeded. */
    memcpy(PlanArgument(arguments, drive, itemCapacity), rewritten, (size_t)length + 1U);
    memcpy(PlanArgument(arguments, device, itemCapacity),
        UMI_KERNEL_DEVICE_QUALIFIED, sizeof UMI_KERNEL_DEVICE_QUALIFIED);
    memcpy(PlanArgument(arguments, count, itemCapacity), "-global", sizeof "-global");
    memcpy(PlanArgument(arguments, count + 1U, itemCapacity),
        "virtio-mmio.force-legacy=false", sizeof "virtio-mmio.force-legacy=false");
    *argumentCount = count + 2U;
    return 1;
}
