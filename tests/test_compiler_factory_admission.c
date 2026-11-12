/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_factory_admission.c
 *
 * PURPOSE:
 *   Verify complete text identities and explicit refusal through every compiler provider factory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/compiler/compiler.h"
#include "../src/compiler/provider_defaults.h"

typedef UmiCompilerProvider (*Factory)(const char *,const char *);
static const Factory factories[] = {
    umi_compiler_gcc_provider, umi_compiler_clang_provider, umi_compiler_msvc_provider,
    umi_compiler_rust_provider, umi_compiler_zig_provider, umi_compiler_assembly_provider,
    umi_compiler_uai_provider, umi_compiler_umicc_provider
};

static void Refused(const UmiCompilerProvider *provider)
{
    /* Check fields, not structure padding copied by a C return-by-value ABI. */
    assert(provider->revision == 0U);
    assert(provider->descriptor.struct_size == 0U);
    assert(provider->descriptor.api_version == 0U);
    assert(provider->descriptor.provider_id[0] == '\0');
    assert(provider->descriptor.executable[0] == '\0');
    assert(provider->descriptor.linker_executable[0] == '\0');
    assert(provider->descriptor.default_target[0] == '\0');
    assert(!provider->descriptor.available);
    assert(!umi_compiler_provider_supports(provider,UMI_COMPILER_LANGUAGE_C));
}

int main(void)
{
    char executable[UMI_COMPILER_PATH_CAPACITY + 1U];
    char target[UMI_COMPILER_NAME_CAPACITY + 1U];
    (void)memset(executable,'x',sizeof(executable));
    (void)memset(target,'t',sizeof(target));
    for (size_t row = 0U; row < sizeof(factories)/sizeof(factories[0]); ++row) {
        executable[UMI_COMPILER_PATH_CAPACITY - 1U] = '\0';
        target[UMI_COMPILER_NAME_CAPACITY - 1U] = '\0';
        UmiCompilerProvider provider = factories[row](executable,target);
        assert(provider.revision == 1U && provider.descriptor.available);
        assert(strcmp(provider.descriptor.executable,executable) == 0);
        assert(strcmp(provider.descriptor.linker_executable,executable) == 0);
        assert(strcmp(provider.descriptor.default_target,target) == 0);
        executable[UMI_COMPILER_PATH_CAPACITY - 1U] = 'x';
        executable[UMI_COMPILER_PATH_CAPACITY] = '\0';
        provider = factories[row](executable,target);
        Refused(&provider);
        /* No terminator within the documented bound is also refused. */
        executable[UMI_COMPILER_PATH_CAPACITY] = 'x';
        provider = factories[row](executable,target);
        Refused(&provider);
        target[UMI_COMPILER_NAME_CAPACITY - 1U] = 't';
        target[UMI_COMPILER_NAME_CAPACITY] = '\0';
        provider = factories[row]("driver",target);
        Refused(&provider);
        target[UMI_COMPILER_NAME_CAPACITY] = 't';
        provider = factories[row]("driver",target);
        Refused(&provider);
        provider = factories[row](NULL,NULL);
        assert(provider.revision == 1U && !provider.descriptor.available);
        assert(provider.descriptor.provider_id[0] != '\0');
        assert(provider.descriptor.default_target[0] == '\0');
        provider = factories[row]("","");
        assert(provider.revision == 1U && !provider.descriptor.available);
    }
    char id[UMI_COMPILER_ID_CAPACITY];
    char name[UMI_COMPILER_NAME_CAPACITY];
    (void)memset(id,'i',sizeof(id));
    (void)memset(name,'n',sizeof(name));
    UmiCompilerProvider invalid[] = {
        umi_compiler_provider_make(NULL,"name","driver","",UMI_COMPILER_FAMILY_GCC,2U,true,true,true,true),
        umi_compiler_provider_make("","name","driver","",UMI_COMPILER_FAMILY_GCC,2U,true,true,true,true),
        umi_compiler_provider_make(id,"name","driver","",UMI_COMPILER_FAMILY_GCC,2U,true,true,true,true),
        umi_compiler_provider_make("id",NULL,"driver","",UMI_COMPILER_FAMILY_GCC,2U,true,true,true,true),
        umi_compiler_provider_make("id",name,"driver","",UMI_COMPILER_FAMILY_GCC,2U,true,true,true,true)
    };
    for (size_t i = 0U; i < sizeof(invalid)/sizeof(invalid[0]); ++i) Refused(&invalid[i]);
    return 0;
}
