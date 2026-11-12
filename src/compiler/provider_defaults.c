/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/provider_defaults.c
 *
 * PURPOSE:
 *   Implement the provider defaults behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "provider_defaults.h"
#include "catalogue_validation.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the compiler provider make operation used by this module and its client
 * applications.
 */
/* Bound the complete input before constructing a provider so truncated executable or target text cannot acquire a valid identity. Null and empty executable defaults remain supported. The previous implementation is retained for review. */
#if 0
UmiCompilerProvider umi_compiler_provider_make(const char *id,const char *name,const char *executable,const char *target,UmiCompilerFamily family,uint32_t languages,bool cross,bool compilation_database,bool response_files,bool stable_c_abi)
{
    UmiCompilerProvider provider;
    UmiCompilerProviderDescriptor descriptor;
    (void)memset(&provider,0,sizeof(provider)); (void)memset(&descriptor,0,sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor); descriptor.api_version = UMI_COMPILER_API_VERSION;
    (void)snprintf(descriptor.provider_id,sizeof(descriptor.provider_id),"%s",id);
    (void)snprintf(descriptor.display_name,sizeof(descriptor.display_name),"%s",name);
    (void)snprintf(descriptor.executable,sizeof(descriptor.executable),"%s",executable != NULL ? executable : "");
    (void)snprintf(descriptor.linker_executable,sizeof(descriptor.linker_executable),"%s",executable != NULL ? executable : "");
    (void)snprintf(descriptor.default_target,sizeof(descriptor.default_target),"%s",target != NULL ? target : "");
    descriptor.family = family; descriptor.language_mask = languages; descriptor.available = executable != NULL && executable[0] != '\0'; descriptor.supports_cross_compilation = cross; descriptor.supports_compilation_database = compilation_database; descriptor.supports_response_files = response_files; descriptor.supports_stable_c_abi = stable_c_abi;
    (void)umi_compiler_provider_init(&provider,&descriptor);
    return provider;
}
#endif
UmiCompilerProvider umi_compiler_provider_make(const char *id,const char *name,const char *executable,const char *target,UmiCompilerFamily family,uint32_t languages,bool cross,bool compilation_database,bool response_files,bool stable_c_abi)
{
    UmiCompilerProvider provider;
    UmiCompilerProviderDescriptor descriptor;
    (void)memset(&provider,0,sizeof(provider));
    (void)memset(&descriptor,0,sizeof(descriptor));
    const char *driver = executable != NULL ? executable : "";
    const char *triple = target != NULL ? target : "";
    /* A factory has no status output. An invalid, zero-initialized provider is
     * its refusal value; a shortened executable must never become available.
     * Keep null/empty driver support for declared but unavailable toolchains. */
    if (!UmiCompilerCatalogueText(id,sizeof(descriptor.provider_id),true) ||
        !UmiCompilerCatalogueText(name,sizeof(descriptor.display_name),false) ||
        !UmiCompilerCatalogueText(driver,sizeof(descriptor.executable),false) ||
        !UmiCompilerCatalogueText(triple,sizeof(descriptor.default_target),false))
        return provider;
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_COMPILER_API_VERSION;
    (void)memcpy(descriptor.provider_id,id,strlen(id) + 1U);
    (void)memcpy(descriptor.display_name,name,strlen(name) + 1U);
    (void)memcpy(descriptor.executable,driver,strlen(driver) + 1U);
    (void)memcpy(descriptor.linker_executable,driver,strlen(driver) + 1U);
    (void)memcpy(descriptor.default_target,triple,strlen(triple) + 1U);
    descriptor.family = family;
    descriptor.language_mask = languages;
    descriptor.available = driver[0] != '\0';
    descriptor.supports_cross_compilation = cross;
    descriptor.supports_compilation_database = compilation_database;
    descriptor.supports_response_files = response_files;
    descriptor.supports_stable_c_abi = stable_c_abi;
    (void)umi_compiler_provider_init(&provider,&descriptor);
    return provider;
}
