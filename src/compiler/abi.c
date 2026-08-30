/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/abi.c
 *
 * PURPOSE:
 *   Implement the abi behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Native ABI profiles | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/abi.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_compiler_abi_init(UmiCompilerAbiProfile *profile,const char *abi_id,UmiCompilerAbiKind kind,const UmiCompilerTarget *target)
{
    int length;
    if (profile == NULL || abi_id == NULL || abi_id[0] == '\0' || target == NULL || kind == UMI_COMPILER_ABI_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(profile,0,sizeof(*profile)); length = snprintf(profile->abi_id,sizeof(profile->abi_id),"%s",abi_id); if (length < 0 || (size_t)length >= sizeof(profile->abi_id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    profile->kind = kind; profile->target = *target; profile->pointer_width = target->pointer_width; profile->stable = kind == UMI_COMPILER_ABI_C || kind == UMI_COMPILER_ABI_SYSTEM; profile->exceptions = kind == UMI_COMPILER_ABI_CPP; profile->rtti = kind == UMI_COMPILER_ABI_CPP; return UMI_STATUS_OK;
}
bool umi_compiler_abi_compatible(const UmiCompilerAbiProfile *producer,const UmiCompilerAbiProfile *consumer) { if (producer == NULL || consumer == NULL || !umi_compiler_target_compatible(&producer->target,&consumer->target) || producer->pointer_width != consumer->pointer_width) return false; if (producer->kind == consumer->kind) return true; return producer->kind == UMI_COMPILER_ABI_C && consumer->kind == UMI_COMPILER_ABI_C; }
const char *umi_compiler_abi_name(UmiCompilerAbiKind kind) { switch (kind) { case UMI_COMPILER_ABI_C: return "Stable C ABI"; case UMI_COMPILER_ABI_CPP: return "C++ ABI"; case UMI_COMPILER_ABI_SYSTEM: return "System ABI"; case UMI_COMPILER_ABI_UAI: return "UAI ABI"; default: return "Unknown ABI"; } }
