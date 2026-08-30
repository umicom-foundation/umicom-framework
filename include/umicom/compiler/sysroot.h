/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/sysroot.h
 *
 * PURPOSE:
 *   Publish the public sysroot contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_SYSROOT_H
#define UMICOM_COMPILER_SYSROOT_H
#include "umicom/compiler/target.h"
#define UMI_COMPILER_MAX_SYSROOTS 16U
typedef struct UmiCompilerSysroot { char sysroot_id[UMI_COMPILER_ID_CAPACITY]; char path[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerTarget target; bool available; } UmiCompilerSysroot;
typedef struct UmiCompilerSysrootCatalogue { UmiCompilerSysroot items[UMI_COMPILER_MAX_SYSROOTS]; size_t count; uint64_t revision; } UmiCompilerSysrootCatalogue;
UmiStatus umi_compiler_sysroot_register(UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerSysroot *sysroot);
const UmiCompilerSysroot *umi_compiler_sysroot_resolve(const UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerTarget *target);
#endif
