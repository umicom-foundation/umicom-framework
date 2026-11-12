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
#include "umicom/compiler/sysroot.h"
#include <string.h>
#include "catalogue_validation.h"

static bool UmiCompilerSysrootRecordValid(const UmiCompilerSysroot *sysroot)
{
    return sysroot != NULL &&
        UmiCompilerCatalogueText(sysroot->sysroot_id,sizeof(sysroot->sysroot_id),true) &&
        UmiCompilerCatalogueText(sysroot->path,sizeof(sysroot->path),true) &&
        UmiCompilerCatalogueTarget(&sysroot->target);
}

/* Public count and rows can be restored or edited by a caller. Validate the
 * complete active set before exposing even an earlier matching borrowed row. */
bool umi_compiler_sysroot_catalogue_valid(const UmiCompilerSysrootCatalogue *catalogue)
{
    if (catalogue == NULL || catalogue->count > UMI_COMPILER_MAX_SYSROOTS) return false;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (!UmiCompilerSysrootRecordValid(&catalogue->items[index])) return false;
    return true;
}

/* Add compiler sysroot only after its inputs and available capacity have been checked. */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_sysroot_register(UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerSysroot *sysroot)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || sysroot == NULL || sysroot->sysroot_id[0] == '\0' || sysroot->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(catalogue->items[index].sysroot_id,sysroot->sysroot_id) == 0) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->count >= UMI_COMPILER_MAX_SYSROOTS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++] = *sysroot; catalogue->revision += 1U; return UMI_STATUS_OK; }
#endif
UmiStatus umi_compiler_sysroot_register(UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerSysroot *sysroot)
{
    if (catalogue == NULL || !UmiCompilerSysrootRecordValid(sysroot)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_sysroot_catalogue_valid(catalogue)) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->items[index].sysroot_id,sysroot->sysroot_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count == UMI_COMPILER_MAX_SYSROOTS || catalogue->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A caller may borrow a record from this owner. Stage it before publication. */
    const UmiCompilerSysroot staged = *sysroot;
    catalogue->items[catalogue->count] = staged;
    catalogue->count += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler sysroot resolve operation used by this module and its client
 * applications.
 */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
const UmiCompilerSysroot *umi_compiler_sysroot_resolve(const UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerTarget *target)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || target == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->items[index].available && umi_compiler_target_compatible(&catalogue->items[index].target,target)) return &catalogue->items[index]; return NULL; }
#endif
const UmiCompilerSysroot *umi_compiler_sysroot_resolve(const UmiCompilerSysrootCatalogue *catalogue,const UmiCompilerTarget *target)
{
    if (!umi_compiler_sysroot_catalogue_valid(catalogue) || !UmiCompilerCatalogueTarget(target)) return NULL;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (catalogue->items[index].available && umi_compiler_target_compatible(&catalogue->items[index].target,target))
            return &catalogue->items[index];
    return NULL;
}
