/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/runtime_library.c
 *
 * PURPOSE:
 *   Implement the runtime library behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/runtime_library.h"
#include <string.h>
#include "catalogue_validation.h"

static bool UmiCompilerRuntimeRecordValid(const UmiCompilerRuntimeLibrary *runtime)
{
    return runtime != NULL &&
        UmiCompilerCatalogueText(runtime->runtime_id,sizeof(runtime->runtime_id),true) &&
        UmiCompilerCatalogueText(runtime->path,sizeof(runtime->path),true) &&
        runtime->language > UMI_COMPILER_LANGUAGE_UNKNOWN && runtime->language <= UMI_COMPILER_LANGUAGE_UAI &&
        UmiCompilerCatalogueAbi(&runtime->abi);
}

bool umi_compiler_runtime_catalogue_valid(const UmiCompilerRuntimeCatalogue *catalogue)
{
    if (catalogue == NULL || catalogue->count > UMI_COMPILER_MAX_RUNTIME_LIBRARIES) return false;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (!UmiCompilerRuntimeRecordValid(&catalogue->items[index])) return false;
    return true;
}

/* Add compiler runtime only after its inputs and available capacity have been checked. */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_runtime_register(UmiCompilerRuntimeCatalogue *catalogue,const UmiCompilerRuntimeLibrary *runtime)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || runtime == NULL || runtime->runtime_id[0] == '\0' || runtime->path[0] == '\0' || runtime->language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(catalogue->items[index].runtime_id,runtime->runtime_id) == 0) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->count >= UMI_COMPILER_MAX_RUNTIME_LIBRARIES) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++] = *runtime; catalogue->revision += 1U; return UMI_STATUS_OK; }
#endif
UmiStatus umi_compiler_runtime_register(UmiCompilerRuntimeCatalogue *catalogue,const UmiCompilerRuntimeLibrary *runtime)
{
    if (catalogue == NULL || !UmiCompilerRuntimeRecordValid(runtime)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_runtime_catalogue_valid(catalogue)) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->items[index].runtime_id,runtime->runtime_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count == UMI_COMPILER_MAX_RUNTIME_LIBRARIES || catalogue->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiCompilerRuntimeLibrary staged = *runtime;
    catalogue->items[catalogue->count] = staged;
    catalogue->count += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler runtime resolve operation used by this module and its client
 * applications.
 */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
const UmiCompilerRuntimeLibrary *umi_compiler_runtime_resolve(const UmiCompilerRuntimeCatalogue *catalogue,UmiCompilerLanguage language,const UmiCompilerAbiProfile *abi)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || abi == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->items[index].available && catalogue->items[index].language == language && umi_compiler_abi_compatible(&catalogue->items[index].abi,abi)) return &catalogue->items[index]; return NULL; }
#endif
const UmiCompilerRuntimeLibrary *umi_compiler_runtime_resolve(const UmiCompilerRuntimeCatalogue *catalogue,UmiCompilerLanguage language,const UmiCompilerAbiProfile *abi)
{
    if (!umi_compiler_runtime_catalogue_valid(catalogue) || !UmiCompilerCatalogueAbi(abi) ||
        language <= UMI_COMPILER_LANGUAGE_UNKNOWN || language > UMI_COMPILER_LANGUAGE_UAI) return NULL;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (catalogue->items[index].available && catalogue->items[index].language == language &&
            umi_compiler_abi_compatible(&catalogue->items[index].abi,abi)) return &catalogue->items[index];
    return NULL;
}
