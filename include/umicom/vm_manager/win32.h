/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vm_manager/win32.h
 * PURPOSE: Present shared virtual-machine management through the Windows adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_VM_MANAGER_WIN32_H
#define UMICOM_VM_MANAGER_WIN32_H
#ifdef __cplusplus
extern "C" {
#endif
    /* Native presentation adapter. checkOnly constructs then destroys controls;
                 * it does not open a database, create a virtual disk or launch a process. */
    int UmiVmWin32Run(void *instance,int show,int checkOnly);
#ifdef __cplusplus
}
#endif
#endif
