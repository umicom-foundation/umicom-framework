/* Umicom Framework | Compiler registry | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/registry.h"
#include <string.h>
void umi_compiler_registry_init(UmiCompilerRegistry *registry) { if (registry != NULL) { (void)memset(registry,0,sizeof(*registry)); registry->revision = 1U; } }
UmiStatus umi_compiler_registry_register(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider)
{
    size_t index;
    if (registry == NULL || provider == NULL || provider->descriptor.provider_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < registry->count; ++index) if (strcmp(registry->items[index].descriptor.provider_id,provider->descriptor.provider_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_COMPILER_MAX_PROVIDERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *provider; registry->revision += 1U; return UMI_STATUS_OK;
}
const UmiCompilerProvider *umi_compiler_registry_find(const UmiCompilerRegistry *registry,const char *provider_id) { size_t index; if (registry == NULL || provider_id == NULL) return NULL; for (index = 0U; index < registry->count; ++index) if (strcmp(registry->items[index].descriptor.provider_id,provider_id) == 0) return &registry->items[index]; return NULL; }
const UmiCompilerProvider *umi_compiler_registry_resolve(const UmiCompilerRegistry *registry,UmiCompilerLanguage language,bool require_available) { size_t index; if (registry == NULL) return NULL; for (index = 0U; index < registry->count; ++index) if (umi_compiler_provider_supports(&registry->items[index],language) && (!require_available || registry->items[index].descriptor.available)) return &registry->items[index]; return NULL; }
const UmiCompilerProvider *umi_compiler_registry_at(const UmiCompilerRegistry *registry,size_t index) { return registry == NULL || index >= registry->count ? NULL : &registry->items[index]; }
