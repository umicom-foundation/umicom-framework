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
/* Add compiler sysroot only after its inputs and available capacity have been checked. */
UmiStatus umi_compiler_sysroot_register(UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerSysroot *sysroot)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || sysroot == NULL || sysroot->sysroot_id[0] == '\0' || sysroot->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(catalogue->items[index].sysroot_id,sysroot->sysroot_id) == 0) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->count >= UMI_COMPILER_MAX_SYSROOTS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++] = *sysroot; catalogue->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the compiler sysroot resolve operation used by this module and its client
 * applications.
 */
const UmiCompilerSysroot *umi_compiler_sysroot_resolve(const UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerTarget *target)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || target == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->items[index].available && umi_compiler_target_compatible(&catalogue->items[index].target,target)) return &catalogue->items[index]; return NULL; }
