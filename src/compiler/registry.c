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
/* Umicom Framework | Compiler registry | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/registry.h"
#include <string.h>
/*
 * Initialise compiler registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_compiler_registry_init(UmiCompilerRegistry *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { (void)memset(registry,0,sizeof(*registry)); registry->revision = 1U; } }
/* Add compiler registry only after its inputs and available capacity have been checked. */
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
/*
 * Find compiler registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCompilerProvider *umi_compiler_registry_find(const UmiCompilerRegistry *registry,const char *provider_id) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL || provider_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < registry->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(registry->items[index].descriptor.provider_id,provider_id) == 0) return &registry->items[index]; return NULL; }
/*
 * Provide the compiler registry resolve operation used by this module and its client
 * applications.
 */
const UmiCompilerProvider *umi_compiler_registry_resolve(const UmiCompilerRegistry *registry,UmiCompilerLanguage language,bool require_available) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < registry->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_compiler_provider_supports(&registry->items[index],language) && (!require_available || registry->items[index].descriptor.available)) return &registry->items[index]; return NULL; }
/*
 * Find compiler registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCompilerProvider *umi_compiler_registry_at(const UmiCompilerRegistry *registry,size_t index) { return registry == NULL || index >= registry->count ? NULL : &registry->items[index]; }
