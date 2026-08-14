/* Umicom Framework | Compiler provider registry | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_REGISTRY_H
#define UMICOM_COMPILER_REGISTRY_H
#include "umicom/compiler/provider.h"
typedef struct UmiCompilerRegistry { UmiCompilerProvider items[UMI_COMPILER_MAX_PROVIDERS]; size_t count; uint64_t revision; } UmiCompilerRegistry;
void umi_compiler_registry_init(UmiCompilerRegistry *registry);
UmiStatus umi_compiler_registry_register(UmiCompilerRegistry *registry,const UmiCompilerProvider *provider);
const UmiCompilerProvider *umi_compiler_registry_find(const UmiCompilerRegistry *registry,const char *provider_id);
const UmiCompilerProvider *umi_compiler_registry_resolve(const UmiCompilerRegistry *registry,UmiCompilerLanguage language,bool require_available);
const UmiCompilerProvider *umi_compiler_registry_at(const UmiCompilerRegistry *registry,size_t index);
#endif
