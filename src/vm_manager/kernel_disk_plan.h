/*-----------------------------------------------------------------------------
 * Umicom Framework - owned Kernel scratch-disk QEMU argument policy.
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 *
 * This pure C23 helper is shared with deterministic tests. It rewrites ONLY
 * the VM Manager's already-validated, internally owned argument vector,
 * after a checked copy has been made. It must never receive argv from a
 * caller or raw QEMU options. The normal snapshot=on path is untouched.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VM_MANAGER_KERNEL_DISK_PLAN_H
#define UMICOM_VM_MANAGER_KERNEL_DISK_PLAN_H

#include <stddef.h>

/* Return 1 after an all-or-nothing mutation of the private argument vector;
 * return 0 without changing it for malformed/missing/duplicate or too-short
 * inputs. `arguments` is a contiguous count*itemCapacity byte array. */
int UmiVmKernelDiskPatchArguments(void *arguments, size_t *argumentCount,
    size_t argumentCapacity, size_t itemCapacity, int readOnly);

#endif /* UMICOM_VM_MANAGER_KERNEL_DISK_PLAN_H */
