/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_catalogue_admission.c
 *
 * PURPOSE:
 *   Exercise real compiler catalogue owners at text, extent, alias and revision boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Required checks remain active in optimized owner qualification builds. */
#include "support/assertions.h"
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static UmiCompilerSysroot Sysroot(void)
{
    UmiCompilerSysroot value = {0};
    (void)strcpy(value.sysroot_id,"sdk");
    (void)strcpy(value.path,"/sdk");
    assert(umi_compiler_target_parse("x86_64-unknown-linux-gnu",&value.target) == UMI_STATUS_OK);
    value.available = true;
    return value;
}

static UmiCompilerRuntimeLibrary Runtime(void)
{
    const UmiCompilerSysroot sysroot = Sysroot();
    UmiCompilerRuntimeLibrary value = {0};
    (void)strcpy(value.runtime_id,"runtime");
    (void)strcpy(value.path,"/sdk/lib/runtime.a");
    value.language = UMI_COMPILER_LANGUAGE_C;
    value.available = true;
    assert(umi_compiler_abi_init(&value.abi,"c",UMI_COMPILER_ABI_C,&sysroot.target) == UMI_STATUS_OK);
    return value;
}

/* Each fixed text field receives an independent missing-terminator fixture. */
static void CorruptTarget(UmiCompilerTarget *target,unsigned field)
{
    switch (field) {
    case 0U: (void)memset(target->triple,'x',sizeof(target->triple)); break;
    case 1U: (void)memset(target->architecture,'x',sizeof(target->architecture)); break;
    case 2U: (void)memset(target->vendor,'x',sizeof(target->vendor)); break;
    case 3U: (void)memset(target->operating_system,'x',sizeof(target->operating_system)); break;
    case 4U: (void)memset(target->environment,'x',sizeof(target->environment)); break;
    default: assert(false); break;
    }
}

static void CorruptProvider(UmiCompilerProvider *provider,unsigned fault)
{
    UmiCompilerProviderDescriptor *value = &provider->descriptor;
    switch (fault) {
    case 0U: (void)memset(value->provider_id,'x',sizeof(value->provider_id)); break;
    case 1U: (void)memset(value->display_name,'x',sizeof(value->display_name)); break;
    case 2U: (void)memset(value->executable,'x',sizeof(value->executable)); break;
    case 3U: (void)memset(value->linker_executable,'x',sizeof(value->linker_executable)); break;
    case 4U: (void)memset(value->default_target,'x',sizeof(value->default_target)); break;
    case 5U: value->provider_id[0] = '\0'; break;
    default: assert(false); break;
    }
}

static void CorruptSysroot(UmiCompilerSysroot *value,unsigned fault)
{
    if (fault == 0U) (void)memset(value->sysroot_id,'x',sizeof(value->sysroot_id));
    else if (fault == 1U) (void)memset(value->path,'x',sizeof(value->path));
    else if (fault < 7U) CorruptTarget(&value->target,fault - 2U);
    else if (fault == 7U) value->sysroot_id[0] = '\0';
    else if (fault == 8U) value->path[0] = '\0';
    else assert(false);
}

static void CorruptRuntime(UmiCompilerRuntimeLibrary *value,unsigned fault)
{
    if (fault == 0U) (void)memset(value->runtime_id,'x',sizeof(value->runtime_id));
    else if (fault == 1U) (void)memset(value->path,'x',sizeof(value->path));
    else if (fault == 2U) (void)memset(value->abi.abi_id,'x',sizeof(value->abi.abi_id));
    else if (fault < 8U) CorruptTarget(&value->abi.target,fault - 3U);
    else if (fault == 8U) value->runtime_id[0] = '\0';
    else if (fault == 9U) value->path[0] = '\0';
    else if (fault == 10U) value->language = UMI_COMPILER_LANGUAGE_UNKNOWN;
    else if (fault == 11U) value->language = (UmiCompilerLanguage)-1;
    else if (fault == 12U) value->language = (UmiCompilerLanguage)(UMI_COMPILER_LANGUAGE_UAI + 1);
    else assert(false);
}

static void ProviderAdmission(void)
{
    const UmiCompilerProvider good = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu");
    UmiCompilerRegistry owner, before;
    assert(!umi_compiler_registry_valid(NULL));
    assert(umi_compiler_registry_register(NULL,&good) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_registry_find(NULL,"gcc") == NULL);
    assert(umi_compiler_registry_resolve(NULL,UMI_COMPILER_LANGUAGE_C,true) == NULL);
    assert(umi_compiler_registry_at(NULL,0U) == NULL);
    for (unsigned fault = 0U; fault < 6U; ++fault) {
        UmiCompilerProvider bad = good;
        CorruptProvider(&bad,fault);
        umi_compiler_registry_init(&owner);
        (void)memcpy(&before,&owner,sizeof(before));
        assert(umi_compiler_registry_register(&owner,&bad) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
        owner.items[0] = good;
        owner.items[1] = bad;
        owner.count = 2U;
        (void)memcpy(&before,&owner,sizeof(before));
        assert(!umi_compiler_registry_valid(&owner));
        assert(umi_compiler_registry_find(&owner,good.descriptor.provider_id) == NULL);
        assert(umi_compiler_registry_resolve(&owner,UMI_COMPILER_LANGUAGE_C,true) == NULL);
        assert(umi_compiler_registry_at(&owner,0U) == NULL);
        assert(umi_compiler_registry_register(&owner,&good) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
    const size_t bad_counts[] = {UMI_COMPILER_MAX_PROVIDERS + 1U,SIZE_MAX};
    for (size_t index = 0U; index < sizeof(bad_counts)/sizeof(bad_counts[0]); ++index) {
        umi_compiler_registry_init(&owner);
        owner.count = bad_counts[index];
        (void)memcpy(&before,&owner,sizeof(before));
        assert(!umi_compiler_registry_valid(&owner));
        assert(umi_compiler_registry_register(&owner,&good) == UMI_STATUS_INVALID_STATE);
        assert(umi_compiler_registry_at(&owner,UMI_COMPILER_MAX_PROVIDERS) == NULL);
        assert(umi_compiler_registry_find(&owner,good.descriptor.provider_id) == NULL);
        assert(umi_compiler_registry_resolve(&owner,UMI_COMPILER_LANGUAGE_C,true) == NULL);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
    umi_compiler_registry_init(&owner);
    owner.revision = UINT64_MAX - 1U;
    assert(umi_compiler_registry_register(&owner,&good) == UMI_STATUS_OK);
    assert(owner.revision == UINT64_MAX && owner.count == 1U);
    (void)memcpy(&before,&owner,sizeof(before));
    assert(umi_compiler_registry_register(&owner,&owner.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    UmiCompilerProvider next = good;
    (void)strcpy(next.descriptor.provider_id,"next");
    assert(umi_compiler_registry_register(&owner,&next) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_registry_find(&owner,good.descriptor.provider_id) == &owner.items[0]);
    assert(umi_compiler_registry_at(&owner,0U) == &owner.items[0]);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    char unclosed[UMI_COMPILER_ID_CAPACITY];
    (void)memset(unclosed,'x',sizeof(unclosed));
    assert(umi_compiler_registry_find(&owner,unclosed) == NULL);
    assert(umi_compiler_registry_find(&owner,"") == NULL);
    assert(umi_compiler_registry_find(&owner,NULL) == NULL);
    assert(umi_compiler_registry_at(&owner,SIZE_MAX) == NULL);
    assert(umi_compiler_registry_register(&owner,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    owner.items[0].descriptor.available = false;
    assert(umi_compiler_registry_resolve(&owner,UMI_COMPILER_LANGUAGE_C,true) == NULL);
    assert(umi_compiler_registry_resolve(&owner,UMI_COMPILER_LANGUAGE_C,false) == &owner.items[0]);
    assert(umi_compiler_registry_resolve(&owner,(UmiCompilerLanguage)-1,false) == NULL);
    umi_compiler_registry_init(&owner);
    for (size_t index = 0U; index < UMI_COMPILER_MAX_PROVIDERS; ++index) {
        next = good;
        (void)snprintf(next.descriptor.provider_id,sizeof(next.descriptor.provider_id),"provider-%zu",index);
        assert(umi_compiler_registry_register(&owner,&next) == UMI_STATUS_OK);
    }
    (void)memcpy(&before,&owner,sizeof(before));
    assert(umi_compiler_registry_register(&owner,&good) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_registry_register(&owner,&owner.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_compiler_registry_at(&owner,UMI_COMPILER_MAX_PROVIDERS - 1U) == &owner.items[UMI_COMPILER_MAX_PROVIDERS - 1U]);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
}

static void SysrootAdmission(void)
{
    const UmiCompilerSysroot good = Sysroot();
    UmiCompilerSysrootCatalogue owner = {0}, before;
    assert(!umi_compiler_sysroot_catalogue_valid(NULL));
    assert(umi_compiler_sysroot_register(NULL,&good) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_sysroot_resolve(NULL,&good.target) == NULL);
    for (unsigned fault = 0U; fault < 9U; ++fault) {
        UmiCompilerSysroot bad = good;
        CorruptSysroot(&bad,fault);
        (void)memset(&owner,0,sizeof(owner));
        (void)memcpy(&before,&owner,sizeof(before));
        assert(umi_compiler_sysroot_register(&owner,&bad) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
        owner.items[0] = good; owner.items[1] = bad; owner.count = 2U;
        (void)memcpy(&before,&owner,sizeof(before));
        assert(!umi_compiler_sysroot_catalogue_valid(&owner));
        assert(umi_compiler_sysroot_resolve(&owner,&good.target) == NULL);
        assert(umi_compiler_sysroot_register(&owner,&good) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
    const size_t bad_counts[] = {UMI_COMPILER_MAX_SYSROOTS + 1U,SIZE_MAX};
    for (size_t index = 0U; index < sizeof(bad_counts)/sizeof(bad_counts[0]); ++index) {
        (void)memset(&owner,0,sizeof(owner)); owner.count = bad_counts[index]; (void)memcpy(&before,&owner,sizeof(before));
        assert(!umi_compiler_sysroot_catalogue_valid(&owner));
        assert(umi_compiler_sysroot_register(&owner,&good) == UMI_STATUS_INVALID_STATE);
        assert(umi_compiler_sysroot_resolve(&owner,&good.target) == NULL);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
    (void)memset(&owner,0,sizeof(owner)); owner.revision = UINT64_MAX - 1U;
    assert(umi_compiler_sysroot_register(&owner,&good) == UMI_STATUS_OK);
    assert(owner.count == 1U && owner.revision == UINT64_MAX);
    (void)memcpy(&before,&owner,sizeof(before));
    UmiCompilerSysroot next = good;
    (void)strcpy(next.sysroot_id,"next");
    assert(umi_compiler_sysroot_register(&owner,&next) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_sysroot_register(&owner,&owner.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_compiler_sysroot_resolve(&owner,&good.target) == &owner.items[0]);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    for (unsigned field = 0U; field < 5U; ++field) {
        UmiCompilerTarget bad = good.target;
        CorruptTarget(&bad,field);
        assert(umi_compiler_sysroot_resolve(&owner,&bad) == NULL);
    }
    assert(umi_compiler_sysroot_resolve(&owner,NULL) == NULL);
    assert(umi_compiler_sysroot_register(&owner,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    owner.items[0].available = false;
    assert(umi_compiler_sysroot_resolve(&owner,&good.target) == NULL);
    (void)memset(&owner,0,sizeof(owner));
    for (size_t index = 0U; index < UMI_COMPILER_MAX_SYSROOTS; ++index) {
        next = good;
        (void)snprintf(next.sysroot_id,sizeof(next.sysroot_id),"sdk-%zu",index);
        assert(umi_compiler_sysroot_register(&owner,&next) == UMI_STATUS_OK);
    }
    (void)memcpy(&before,&owner,sizeof(before));
    assert(umi_compiler_sysroot_register(&owner,&good) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_sysroot_register(&owner,&owner.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
}

static void RuntimeAdmission(void)
{
    const UmiCompilerRuntimeLibrary good = Runtime();
    UmiCompilerRuntimeCatalogue owner = {0}, before;
    assert(!umi_compiler_runtime_catalogue_valid(NULL));
    assert(umi_compiler_runtime_register(NULL,&good) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_runtime_resolve(NULL,good.language,&good.abi) == NULL);
    for (unsigned fault = 0U; fault < 13U; ++fault) {
        UmiCompilerRuntimeLibrary bad = good;
        CorruptRuntime(&bad,fault);
        (void)memset(&owner,0,sizeof(owner)); (void)memcpy(&before,&owner,sizeof(before));
        assert(umi_compiler_runtime_register(&owner,&bad) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
        owner.items[0] = good; owner.items[1] = bad; owner.count = 2U; (void)memcpy(&before,&owner,sizeof(before));
        assert(!umi_compiler_runtime_catalogue_valid(&owner));
        assert(umi_compiler_runtime_resolve(&owner,good.language,&good.abi) == NULL);
        assert(umi_compiler_runtime_register(&owner,&good) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
    const size_t bad_counts[] = {UMI_COMPILER_MAX_RUNTIME_LIBRARIES + 1U,SIZE_MAX};
    for (size_t index = 0U; index < sizeof(bad_counts)/sizeof(bad_counts[0]); ++index) {
        (void)memset(&owner,0,sizeof(owner)); owner.count = bad_counts[index]; (void)memcpy(&before,&owner,sizeof(before));
        assert(!umi_compiler_runtime_catalogue_valid(&owner));
        assert(umi_compiler_runtime_register(&owner,&good) == UMI_STATUS_INVALID_STATE);
        assert(umi_compiler_runtime_resolve(&owner,good.language,&good.abi) == NULL);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
    (void)memset(&owner,0,sizeof(owner)); owner.revision = UINT64_MAX - 1U;
    assert(umi_compiler_runtime_register(&owner,&good) == UMI_STATUS_OK);
    assert(owner.count == 1U && owner.revision == UINT64_MAX);
    (void)memcpy(&before,&owner,sizeof(before));
    UmiCompilerRuntimeLibrary next = good;
    (void)strcpy(next.runtime_id,"next");
    assert(umi_compiler_runtime_register(&owner,&next) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_runtime_register(&owner,&owner.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_compiler_runtime_resolve(&owner,good.language,&good.abi) == &owner.items[0]);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    for (unsigned field = 0U; field < 6U; ++field) {
        UmiCompilerAbiProfile bad = good.abi;
        if (field == 0U) (void)memset(bad.abi_id,'x',sizeof(bad.abi_id));
        else CorruptTarget(&bad.target,field - 1U);
        assert(umi_compiler_runtime_resolve(&owner,good.language,&bad) == NULL);
    }
    assert(umi_compiler_runtime_resolve(&owner,good.language,NULL) == NULL);
    assert(umi_compiler_runtime_register(&owner,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_runtime_resolve(&owner,(UmiCompilerLanguage)-1,&good.abi) == NULL);
    assert(umi_compiler_runtime_resolve(&owner,UMI_COMPILER_LANGUAGE_UNKNOWN,&good.abi) == NULL);
    assert(umi_compiler_runtime_resolve(&owner,(UmiCompilerLanguage)(UMI_COMPILER_LANGUAGE_UAI + 1),&good.abi) == NULL);
    owner.items[0].available = false;
    assert(umi_compiler_runtime_resolve(&owner,good.language,&good.abi) == NULL);
    (void)memset(&owner,0,sizeof(owner));
    for (size_t index = 0U; index < UMI_COMPILER_MAX_RUNTIME_LIBRARIES; ++index) {
        next = good;
        (void)snprintf(next.runtime_id,sizeof(next.runtime_id),"runtime-%zu",index);
        assert(umi_compiler_runtime_register(&owner,&next) == UMI_STATUS_OK);
    }
    (void)memcpy(&before,&owner,sizeof(before));
    assert(umi_compiler_runtime_register(&owner,&good) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_compiler_runtime_register(&owner,&owner.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
}

int main(void)
{
    ProviderAdmission();
    SysrootAdmission();
    RuntimeAdmission();
    return 0;
}
