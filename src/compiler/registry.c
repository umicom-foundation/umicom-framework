/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/registry.c
 *
 * PURPOSE:
 *   Implement the registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/registry.h"
#include <string.h>
#include "catalogue_validation.h"

bool umi_compiler_registry_valid(const UmiCompilerRegistry *registry)
{
    if (registry == NULL || registry->count > UMI_COMPILER_MAX_PROVIDERS) return false;
    for (size_t index = 0U; index < registry->count; ++index)
        if (!UmiCompilerCatalogueProvider(&registry->items[index])) return false;
    return true;
}

/*
 * Initialise compiler registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_compiler_registry_init(UmiCompilerRegistry *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { (void)memset(registry,0,sizeof(*registry)); registry->revision = 1U; } }
/* Add compiler registry only after its inputs and available capacity have been checked. */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_registry_register(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider == NULL || provider->descriptor.provider_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[index].descriptor.provider_id,provider->descriptor.provider_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_COMPILER_MAX_PROVIDERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *provider; registry->revision += 1U; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_registry_register(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider)
{
    if (registry == NULL || !UmiCompilerCatalogueProvider(provider)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_registry_valid(registry)) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < registry->count; ++index)
        if (strcmp(registry->items[index].descriptor.provider_id,provider->descriptor.provider_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count == UMI_COMPILER_MAX_PROVIDERS || registry->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiCompilerProvider staged = *provider;
    registry->items[registry->count] = staged;
    registry->count += 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Find compiler registry while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
const UmiCompilerProvider *umi_compiler_registry_find(const UmiCompilerRegistry *registry,const char *provider_id) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL || provider_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < registry->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(registry->items[index].descriptor.provider_id,provider_id) == 0) return &registry->items[index]; return NULL; }
#endif
const UmiCompilerProvider *umi_compiler_registry_find(const UmiCompilerRegistry *registry,const char *provider_id)
{
    if (!umi_compiler_registry_valid(registry) ||
        !UmiCompilerCatalogueText(provider_id,UMI_COMPILER_ID_CAPACITY,true)) return NULL;
    for (size_t index = 0U; index < registry->count; ++index)
        if (strcmp(registry->items[index].descriptor.provider_id,provider_id) == 0) return &registry->items[index];
    return NULL;
}
/*
 * Provide the compiler registry resolve operation used by this module and its client
 * applications.
 */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
const UmiCompilerProvider *umi_compiler_registry_resolve(const UmiCompilerRegistry *registry,UmiCompilerLanguage language,bool require_available) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < registry->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_compiler_provider_supports(&registry->items[index],language) && (!require_available || registry->items[index].descriptor.available)) return &registry->items[index]; return NULL; }
#endif
const UmiCompilerProvider *umi_compiler_registry_resolve(const UmiCompilerRegistry *registry,UmiCompilerLanguage language,bool require_available)
{
    if (!umi_compiler_registry_valid(registry)) return NULL;
    for (size_t index = 0U; index < registry->count; ++index)
        if (umi_compiler_provider_supports(&registry->items[index],language) &&
            (!require_available || registry->items[index].descriptor.available)) return &registry->items[index];
    return NULL;
}
/*
 * Find compiler registry while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Bounded catalogue admission replaces unchecked lookup and revision mutation; successful selection and duplicate ordering remain unchanged. The previous implementation is retained for review. */
#if 0
const UmiCompilerProvider *umi_compiler_registry_at(const UmiCompilerRegistry *registry,size_t index) { return registry == NULL || index >= registry->count ? NULL : &registry->items[index]; }
#endif
const UmiCompilerProvider *umi_compiler_registry_at(const UmiCompilerRegistry *registry,size_t index)
{
    return !umi_compiler_registry_valid(registry) || index >= registry->count ? NULL : &registry->items[index];
}


UmiStatus umi_compiler_registry_upsert(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider)
{
    /* Retain register's add-only duplicate contract. Explicit replacement is a
     * separate operation so existing callers cannot overwrite a provider by accident. */
    UmiStatus status = umi_compiler_registry_register(registry,provider);
    if (status != UMI_STATUS_ALREADY_EXISTS) return status;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].descriptor.provider_id,provider->descriptor.provider_id) != 0)
            continue;
        (void)memmove(&registry->items[index],provider,sizeof(*provider));
        registry->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
