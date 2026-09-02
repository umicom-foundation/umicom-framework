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
/**
 * Represent the compiler sysroot data shared with callers of this public contract.
 */
typedef struct UmiCompilerSysroot { char sysroot_id[UMI_COMPILER_ID_CAPACITY]; char path[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerTarget target; bool available; } UmiCompilerSysroot;
/**
 * Represent the compiler sysroot catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiCompilerSysrootCatalogue { UmiCompilerSysroot items[UMI_COMPILER_MAX_SYSROOTS]; size_t count; uint64_t revision; } UmiCompilerSysrootCatalogue;
/**
 * Add compiler sysroot only after its inputs and available capacity have been checked.
 */
UmiStatus umi_compiler_sysroot_register(UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerSysroot *sysroot);
/**
 * Provide the compiler sysroot resolve operation used by this module and its client
 * applications.
 */
const UmiCompilerSysroot *umi_compiler_sysroot_resolve(const UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerTarget *target);
#endif
