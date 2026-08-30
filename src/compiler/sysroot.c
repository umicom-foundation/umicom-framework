/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/sysroot.c
 *
 * PURPOSE:
 *   Implement the sysroot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler sysroot catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/sysroot.h"
#include <string.h>
UmiStatus umi_compiler_sysroot_register(UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerSysroot *sysroot)
{ size_t index; if (catalogue == NULL || sysroot == NULL || sysroot->sysroot_id[0] == '\0' || sysroot->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].sysroot_id,sysroot->sysroot_id) == 0) return UMI_STATUS_ALREADY_EXISTS; if (catalogue->count >= UMI_COMPILER_MAX_SYSROOTS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++] = *sysroot; catalogue->revision += 1U; return UMI_STATUS_OK; }
const UmiCompilerSysroot *umi_compiler_sysroot_resolve(const UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerTarget *target)
{ size_t index; if (catalogue == NULL || target == NULL) return NULL; for (index = 0U; index < catalogue->count; ++index) if (catalogue->items[index].available && umi_compiler_target_compatible(&catalogue->items[index].target,target)) return &catalogue->items[index]; return NULL; }
