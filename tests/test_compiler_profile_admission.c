/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_profile_admission.c
 *
 * PURPOSE:
 *   Verify bounded compiler profiles, nested linker admission and failure-preserving construction.
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
#include <stddef.h>
#include <string.h>
#include "umicom/compiler/compiler.h"

static UmiCompilerProfile profile, saved;
static UmiCompilerRegistry registry, saved_registry;
static UmiCompilerTarget target;
typedef struct Field { size_t offset,capacity; } Field;
static const Field fields[] = {
    {offsetof(UmiCompilerProfile,profile_id),sizeof(profile.profile_id)},
    {offsetof(UmiCompilerProfile,display_name),sizeof(profile.display_name)},
    {offsetof(UmiCompilerProfile,provider_id),sizeof(profile.provider_id)},
    {offsetof(UmiCompilerProfile,standard),sizeof(profile.standard)}
};

static void Seed(void)
{
    assert(umi_compiler_target_parse("x86_64-unknown-linux-gnu",&target) == UMI_STATUS_OK);
    UmiCompilerProvider provider = umi_compiler_gcc_provider("driver",NULL);
    umi_compiler_registry_init(&registry);
    assert(umi_compiler_registry_register(&registry,&provider) == UMI_STATUS_OK);
    assert(umi_compiler_profile_init(&profile,"debug","Debug","gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_OK);
}

static void Save(void)
{
    (void)memcpy(&saved,&profile,sizeof(profile));
    (void)memcpy(&saved_registry,&registry,sizeof(registry));
}
static void Same(void)
{
    assert(memcmp(&saved,&profile,sizeof(profile)) == 0);
    assert(memcmp(&saved_registry,&registry,sizeof(registry)) == 0);
}

static void Construction(void)
{
    Seed();
    assert(umi_compiler_profile_init(&profile,profile.profile_id,profile.display_name,
        profile.provider_id,profile.primary_language,&profile.target) == UMI_STATUS_OK);
    assert(strcmp(profile.profile_id,"debug") == 0 && strcmp(profile.display_name,"Debug") == 0);
    assert(strcmp(profile.provider_id,"gcc") == 0 && strcmp(profile.target.triple,target.triple) == 0);
    assert(strcmp(profile.linker.profile_id,"debug") == 0 && strcmp(profile.linker.driver,"gcc") == 0);
    assert(profile.revision == 1U && profile.enabled);
    assert(profile.policy.debug_information && profile.policy.stack_protection);
    char id[UMI_COMPILER_ID_CAPACITY], name[UMI_COMPILER_NAME_CAPACITY];
    (void)memset(id,'i',sizeof(id)); (void)memset(name,'n',sizeof(name)); Save();
    assert(umi_compiler_profile_init(&profile,id,"Name","gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_profile_init(&profile,"id",name,"gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_profile_init(&profile,"id","Name",id,UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_CAPACITY_EXCEEDED); Same();
    assert(umi_compiler_profile_init(&profile,"id","Name","gcc",UMI_COMPILER_LANGUAGE_UNKNOWN,&target) == UMI_STATUS_INVALID_ARGUMENT); Same();
    assert(umi_compiler_profile_init(&profile,"id","Name","gcc",(UmiCompilerLanguage)(UMI_COMPILER_LANGUAGE_UAI+1),&target) == UMI_STATUS_INVALID_ARGUMENT); Same();
    assert(umi_compiler_profile_init(&profile,"id",NULL,"gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_INVALID_ARGUMENT); Same();
    (void)memset(target.environment,'x',sizeof(target.environment));
    assert(umi_compiler_profile_init(&profile,"id","Name","gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_INVALID_ARGUMENT); Same();
}

static void Storage(void)
{
    for (size_t i = 0U; i < sizeof(fields)/sizeof(fields[0]); ++i) {
        Seed(); (void)memset((char *)&profile+fields[i].offset,'x',fields[i].capacity); Save();
        assert(!umi_compiler_profile_storage_valid(&profile));
        assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    }
    Seed(); (void)memset(profile.target.vendor,'x',sizeof(profile.target.vendor)); Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); profile.linker.library_count = SIZE_MAX; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); profile.linker.library_path_count = SIZE_MAX; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); (void)memset(profile.linker.target.triple,'x',sizeof(profile.linker.target.triple)); Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); registry.count = SIZE_MAX; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
    Seed(); UmiCompilerProvider second = umi_compiler_clang_provider("clang-driver",NULL);
    assert(umi_compiler_registry_register(&registry,&second) == UMI_STATUS_OK);
    (void)memset(registry.items[1].descriptor.default_target,'x',sizeof(registry.items[1].descriptor.default_target)); Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_INVALID_STATE); Same();
}

static void Semantics(void)
{
    Seed(); char reason[128]; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,reason,sizeof(reason)) == UMI_STATUS_OK);
    assert(strcmp(reason,"ok") == 0); Same();
    profile.revision = UINT64_MAX; profile.linker.revision = UINT64_MAX; registry.revision = UINT64_MAX; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,NULL,0U) == UMI_STATUS_OK); Same();
    Seed(); (void)strcpy(profile.provider_id,"missing"); Save();
    assert(umi_compiler_profile_validate(&profile,&registry,reason,sizeof(reason)) == UMI_STATUS_NOT_FOUND); Same();
    Seed(); profile.primary_language = UMI_COMPILER_LANGUAGE_RUST; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,reason,sizeof(reason)) == UMI_STATUS_NOT_IMPLEMENTED); Same();
    Seed(); profile.primary_language = UMI_COMPILER_LANGUAGE_UNKNOWN; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,reason,sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT); Same();
    Seed(); profile.policy.sanitizers = UMI_COMPILER_SANITIZER_THREAD | UMI_COMPILER_SANITIZER_ADDRESS; Save();
    assert(umi_compiler_profile_validate(&profile,&registry,reason,sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    assert(strcmp(reason,"thread and address sanitizers cannot be combined") == 0); Same();
    char short_reason[2] = {'x','x'};
    assert(umi_compiler_profile_validate(&profile,&registry,short_reason,sizeof(short_reason)) == UMI_STATUS_INVALID_STATE);
    assert(short_reason[0] == 't' && short_reason[1] == '\0'); Same();
}
int main(void)
{
    assert(!umi_compiler_profile_storage_valid(NULL));
    Construction(); Storage(); Semantics();
    return 0;
}
