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
/* Umicom Framework | Compiler provider defaults | Sammy Hegab | Umicom Foundation | MIT */
#include "provider_defaults.h"
#include <stdio.h>
#include <string.h>
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
