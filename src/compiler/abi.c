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
#include "umicom/compiler/abi.h"
#include <stdio.h>
#include <string.h>
#include "catalogue_validation.h"
/*
 * Initialise compiler abi from caller-provided values so later operations receive a known
 * state.
 */
/* Shared bounded record admission and staged publication protect direct compiler clients and borrowed initialization inputs; existing classification and compatibility policy remain. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_abi_init(UmiCompilerAbiProfile *profile,const char *abi_id,UmiCompilerAbiKind kind,const UmiCompilerTarget *target)
{
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || abi_id == NULL || abi_id[0] == '\0' || target == NULL || kind == UMI_COMPILER_ABI_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(profile,0,sizeof(*profile)); length = snprintf(profile->abi_id,sizeof(profile->abi_id),"%s",abi_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length < 0 || (size_t)length >= sizeof(profile->abi_id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    profile->kind = kind; profile->target = *target; profile->pointer_width = target->pointer_width; profile->stable = kind == UMI_COMPILER_ABI_C || kind == UMI_COMPILER_ABI_SYSTEM; profile->exceptions = kind == UMI_COMPILER_ABI_CPP; profile->rtti = kind == UMI_COMPILER_ABI_CPP; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_abi_init(UmiCompilerAbiProfile *profile,const char *abi_id,UmiCompilerAbiKind kind,const UmiCompilerTarget *target)
{
    if (profile == NULL || abi_id == NULL || abi_id[0] == '\0' ||
        !UmiCompilerCatalogueTarget(target) || kind == UMI_COMPILER_ABI_UNKNOWN)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiCompilerCatalogueText(abi_id,UMI_COMPILER_ID_CAPACITY,true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Capture both borrowed inputs before replacing any part of the profile. */
    UmiCompilerAbiProfile candidate = {0};
    (void)snprintf(candidate.abi_id,sizeof(candidate.abi_id),"%s",abi_id);
    candidate.kind = kind;
    candidate.target = *target;
    candidate.pointer_width = target->pointer_width;
    candidate.stable = kind == UMI_COMPILER_ABI_C || kind == UMI_COMPILER_ABI_SYSTEM;
    candidate.exceptions = kind == UMI_COMPILER_ABI_CPP;
    candidate.rtti = kind == UMI_COMPILER_ABI_CPP;
    *profile = candidate;
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler abi compatible operation used by this module and its client
 * applications.
 */
/* Shared bounded record admission and staged publication protect direct compiler clients and borrowed initialization inputs; existing classification and compatibility policy remain. The previous implementation is retained for review. */
#if 0
bool umi_compiler_abi_compatible(const UmiCompilerAbiProfile *producer,const UmiCompilerAbiProfile *consumer) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (producer == NULL || consumer == NULL || !umi_compiler_target_compatible(&producer->target,&consumer->target) || producer->pointer_width != consumer->pointer_width) return false; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (producer->kind == consumer->kind) return true; return producer->kind == UMI_COMPILER_ABI_C && consumer->kind == UMI_COMPILER_ABI_C; }
#endif
bool umi_compiler_abi_compatible(const UmiCompilerAbiProfile *producer,const UmiCompilerAbiProfile *consumer)
{
    if (!UmiCompilerCatalogueAbi(producer) || !UmiCompilerCatalogueAbi(consumer) ||
        !umi_compiler_target_compatible(&producer->target,&consumer->target) ||
        producer->pointer_width != consumer->pointer_width) return false;
    if (producer->kind == consumer->kind) return true;
    return producer->kind == UMI_COMPILER_ABI_C && consumer->kind == UMI_COMPILER_ABI_C;
}
/* Provide the compiler abi name operation used by this module and its client applications. */
const char *umi_compiler_abi_name(UmiCompilerAbiKind kind) { /* Select the behaviour associated with the requested command or state value. */ switch (kind) { case UMI_COMPILER_ABI_C: return "Stable C ABI"; case UMI_COMPILER_ABI_CPP: return "C++ ABI"; case UMI_COMPILER_ABI_SYSTEM: return "System ABI"; case UMI_COMPILER_ABI_UAI: return "UAI ABI"; default: return "Unknown ABI"; } }
