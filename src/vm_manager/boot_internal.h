/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_internal.h
 * PURPOSE: Keep owned boot arguments private to the shared VM service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VM_MANAGER_BOOT_INTERNAL_H
#define UMICOM_VM_MANAGER_BOOT_INTERNAL_H
#include "internal.h"
#include "umicom/vm_manager/boot.h"
/* A path can double in size when its commas are escaped for QEMU's drive
 * grammar. Store that one argument separately from the caller's original path. */
#define VM_BOOT_ARGUMENT_BYTES 4096U
struct UmiVmBootPlan {
    UmiVmBootRequest request;
    size_t argumentCount;
    char arguments[UMI_CHANNEL_MAX_ARGUMENTS][VM_BOOT_ARGUMENT_BYTES];
};
/* Both CLI entry points use the same session console and ownership policy. */
int VmSessionConsole(UmiVmSession *session);
/* CLI composition reuses one request parser and plan renderer. */
int VmBootParseRequest(int argc, char **argv, UmiVmBootRequest *request, const char **expected);
void VmBootPrintPlan(const UmiVmBootPlan *plan);
int VmBootProfilesMain(int argc, char **argv);
#endif
