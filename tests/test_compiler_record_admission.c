/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_record_admission.c
 *
 * PURPOSE:
 *   Verify bounded compiler record inputs, direct comparisons and atomic aliased initialization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <string.h>

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

static void TargetRecords(void)
{
    UmiCompilerTarget target, before;
    assert(umi_compiler_target_parse("x86_64-pc-windows-gnu",&target) == UMI_STATUS_OK);
    (void)memcpy(&before,&target,sizeof(before));
    const char *bad_text[] = {NULL,"","x86_64","x86_64-pc"};
    for (size_t index = 0U; index < sizeof(bad_text)/sizeof(bad_text[0]); ++index) {
        const UmiStatus expected = index < 2U ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_PARSE_ERROR;
        assert(umi_compiler_target_parse(bad_text[index],&target) == expected);
        assert(memcmp(&target,&before,sizeof(target)) == 0);
    }
    char unclosed[UMI_COMPILER_NAME_CAPACITY];
    (void)memset(unclosed,'x',sizeof(unclosed));
    assert(umi_compiler_target_parse(unclosed,&target) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&target,&before,sizeof(target)) == 0);
    assert(umi_compiler_target_parse(target.triple,&target) == UMI_STATUS_OK);
    assert(strcmp(target.triple,before.triple) == 0);
    assert(umi_compiler_target_compatible(&target,&before));
    assert(umi_compiler_target_parse("aarch64-apple-darwin",&target) == UMI_STATUS_OK);
    assert(strcmp(target.environment,"unknown") == 0);
    assert(target.object_format == UMI_COMPILER_OBJECT_MACH_O && target.pointer_width == 64U);
    assert(umi_compiler_target_parse("wasm32-unknown-unknown",&target) == UMI_STATUS_OK);
    assert(target.object_format == UMI_COMPILER_OBJECT_WASM && target.pointer_width == 32U);
    assert(umi_compiler_target_parse("x86_64-unknown-linux-gnu",&target) == UMI_STATUS_OK);
    assert(target.object_format == UMI_COMPILER_OBJECT_ELF);
    for (unsigned field = 0U; field < 5U; ++field) {
        UmiCompilerTarget bad = target;
        CorruptTarget(&bad,field);
        assert(!umi_compiler_target_compatible(&target,&bad));
        assert(!umi_compiler_target_compatible(&bad,&target));
    }
    assert(!umi_compiler_target_compatible(NULL,&target));
    assert(!umi_compiler_target_compatible(&target,NULL));
    assert(umi_compiler_target_parse("x86_64-unknown-linux-gnu",NULL) == UMI_STATUS_INVALID_ARGUMENT);
}

static void AbiRecords(void)
{
    UmiCompilerTarget target;
    UmiCompilerAbiProfile profile, before;
    assert(umi_compiler_target_parse("x86_64-unknown-linux-gnu",&target) == UMI_STATUS_OK);
    assert(umi_compiler_abi_init(&profile,"stable-c",UMI_COMPILER_ABI_C,&target) == UMI_STATUS_OK);
    assert(profile.stable && !profile.exceptions && !profile.rtti);
    (void)memcpy(&before,&profile,sizeof(before));
    assert(umi_compiler_abi_init(&profile,NULL,UMI_COMPILER_ABI_C,&target) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&profile,&before,sizeof(profile)) == 0);
    assert(umi_compiler_abi_init(&profile,"",UMI_COMPILER_ABI_C,&target) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&profile,&before,sizeof(profile)) == 0);
    assert(umi_compiler_abi_init(&profile,"c",UMI_COMPILER_ABI_UNKNOWN,&target) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&profile,&before,sizeof(profile)) == 0);
    assert(umi_compiler_abi_init(&profile,"c",UMI_COMPILER_ABI_C,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&profile,&before,sizeof(profile)) == 0);
    char unclosed[UMI_COMPILER_ID_CAPACITY];
    (void)memset(unclosed,'x',sizeof(unclosed));
    assert(umi_compiler_abi_init(&profile,unclosed,UMI_COMPILER_ABI_C,&target) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&profile,&before,sizeof(profile)) == 0);
    for (unsigned field = 0U; field < 5U; ++field) {
        UmiCompilerTarget bad = target;
        CorruptTarget(&bad,field);
        assert(umi_compiler_abi_init(&profile,"c",UMI_COMPILER_ABI_C,&bad) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&profile,&before,sizeof(profile)) == 0);
        UmiCompilerAbiProfile other = profile;
        other.target = bad;
        assert(!umi_compiler_abi_compatible(&profile,&other));
        assert(!umi_compiler_abi_compatible(&other,&profile));
    }
    UmiCompilerAbiProfile other = profile;
    (void)memset(other.abi_id,'x',sizeof(other.abi_id));
    assert(!umi_compiler_abi_compatible(&profile,&other));
    assert(!umi_compiler_abi_compatible(&other,&profile));
    assert(umi_compiler_abi_init(&profile,profile.abi_id,UMI_COMPILER_ABI_CPP,&profile.target) == UMI_STATUS_OK);
    assert(strcmp(profile.abi_id,"stable-c") == 0);
    assert(strcmp(profile.target.triple,target.triple) == 0);
    assert(profile.exceptions && profile.rtti && !profile.stable);
    assert(!umi_compiler_abi_compatible(&profile,&before));
    other = profile;
    assert(umi_compiler_abi_compatible(&profile,&other));
    other.pointer_width = 32U;
    assert(!umi_compiler_abi_compatible(&profile,&other));
    assert(!umi_compiler_abi_compatible(NULL,&profile));
    assert(!umi_compiler_abi_compatible(&profile,NULL));
    assert(umi_compiler_abi_init(NULL,"c",UMI_COMPILER_ABI_C,&target) == UMI_STATUS_INVALID_ARGUMENT);
    char longest[UMI_COMPILER_ID_CAPACITY];
    (void)memset(longest,'a',sizeof(longest) - 1U); longest[sizeof(longest) - 1U] = '\0';
    assert(umi_compiler_abi_init(&profile,longest,UMI_COMPILER_ABI_C,&target) == UMI_STATUS_OK);
    assert(strcmp(profile.abi_id,longest) == 0);
}

static void CorruptDescriptor(UmiCompilerProviderDescriptor *value,unsigned field)
{
    switch (field) {
    case 0U: (void)memset(value->provider_id,'x',sizeof(value->provider_id)); break;
    case 1U: (void)memset(value->display_name,'x',sizeof(value->display_name)); break;
    case 2U: (void)memset(value->executable,'x',sizeof(value->executable)); break;
    case 3U: (void)memset(value->linker_executable,'x',sizeof(value->linker_executable)); break;
    case 4U: (void)memset(value->default_target,'x',sizeof(value->default_target)); break;
    default: assert(false); break;
    }
}

static void ProviderRecords(void)
{
    const UmiCompilerProvider good = umi_compiler_gcc_provider("gcc","x86_64-unknown-linux-gnu");
    UmiCompilerProvider owner = good, before;
    UmiCompilerRequest request;
    UmiCompilerPlan plan, plan_before;
    assert(umi_compiler_request_init(&request,"demo","main",UMI_COMPILER_LANGUAGE_C,"main.c","main.o") == UMI_STATUS_OK);
    umi_compiler_plan_init(&plan);
    assert(umi_compiler_plan_add_argument(&plan,"retained") == UMI_STATUS_OK);
    for (unsigned field = 0U; field < 5U; ++field) {
        UmiCompilerProvider bad = good;
        CorruptDescriptor(&bad.descriptor,field);
        (void)memcpy(&before,&owner,sizeof(before));
        assert(umi_compiler_provider_init(&owner,&bad.descriptor) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
        (void)memcpy(&plan_before,&plan,sizeof(plan_before));
        assert(umi_compiler_provider_plan(&bad,&request,&plan) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
    }
    (void)memcpy(&before,&owner,sizeof(before));
    assert(umi_compiler_provider_init(&owner,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    assert(umi_compiler_provider_init(NULL,&good.descriptor) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_compiler_provider_init(&owner,&owner.descriptor) == UMI_STATUS_OK);
    assert(strcmp(owner.descriptor.provider_id,good.descriptor.provider_id) == 0);
    assert(strcmp(owner.descriptor.executable,"gcc") == 0 && owner.revision == 1U);
    for (unsigned fault = 0U; fault < 6U; ++fault) {
        UmiCompilerProviderDescriptor bad = good.descriptor;
        switch (fault) {
        case 0U: bad.struct_size = 0U; break;
        case 1U: bad.api_version = 0U; break;
        case 2U: bad.provider_id[0] = '\0'; break;
        case 3U: bad.display_name[0] = '\0'; break;
        case 4U: bad.family = UMI_COMPILER_FAMILY_UNKNOWN; break;
        case 5U: bad.language_mask = 0U; break;
        default: assert(false); break;
        }
        (void)memcpy(&before,&owner,sizeof(before));
        assert(umi_compiler_provider_init(&owner,&bad) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&owner,&before,sizeof(owner)) == 0);
    }
}

int main(void)
{
    TargetRecords();
    AbiRecords();
    ProviderRecords();
    return 0;
}
