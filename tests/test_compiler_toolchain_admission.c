/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_toolchain_admission.c
 *
 * PURPOSE:
 *   Reject failed and malformed selected tools without publishing partial compiler records.
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
static UmiToolchainProfile toolchain;
static UmiCompilerProfile profile;
static UmiCompilerProfile saved;
static UmiCompilerRegistry registry;
static UmiCompilerRegistry prior;

static void Fixture(void)
{
    umi_toolchain_profile_init(&toolchain);
    (void)strcpy(toolchain.display_name,"Selected tool");
    (void)strcpy(toolchain.target_triple,"x86_64-unknown-linux-gnu");
    toolchain.selected_c_compiler = UMI_TOOL_GCC;
    toolchain.tools[UMI_TOOL_GCC].state = UMI_TOOL_FOUND;
    (void)strcpy(toolchain.tools[UMI_TOOL_GCC].path,"reported-gcc");
    (void)memset(&profile,0x5a,sizeof(profile));
    (void)memcpy(&saved,&profile,sizeof(saved));
    umi_compiler_registry_init(&registry);
    (void)memcpy(&prior,&registry,sizeof(prior));
}

static void Refused(UmiStatus expected)
{
    size_t imported = 77U;
    assert(umi_compiler_profile_from_toolchain(&toolchain,"selected",&profile) == expected);
    assert(memcmp(&profile,&saved,sizeof(profile)) == 0);
    assert(umi_compiler_registry_import_toolchain(&registry,&toolchain,&imported) == expected);
    assert(imported == 0U);
    assert(memcmp(&registry,&prior,sizeof(registry)) == 0);
}

int main(void)
{
    Fixture();
    assert(umi_compiler_profile_from_toolchain(&toolchain,"selected",&profile) == UMI_STATUS_OK);
    assert(strcmp(profile.linker.driver,"reported-gcc") == 0);
    size_t imported = 0U;
    assert(umi_compiler_registry_import_toolchain(&registry,&toolchain,&imported) == UMI_STATUS_OK);
    assert(imported == 1U);
    (void)memcpy(&prior,&registry,sizeof(prior));
    imported = 77U;
    assert(umi_compiler_registry_import_toolchain(&registry,&toolchain,&imported) == UMI_STATUS_ALREADY_EXISTS);
    assert(imported == 0U && memcmp(&prior,&registry,sizeof(registry)) == 0);
    Fixture(); toolchain.tools[UMI_TOOL_GCC].state = UMI_TOOL_FAILED; Refused(UMI_STATUS_UNAVAILABLE);
    Fixture(); toolchain.tools[UMI_TOOL_GCC].state = UMI_TOOL_MISSING; Refused(UMI_STATUS_UNAVAILABLE);
    Fixture(); toolchain.tools[UMI_TOOL_GCC].state = (UmiToolState)99; Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); toolchain.tools[UMI_TOOL_GCC].kind = UMI_TOOL_CMAKE; Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); toolchain.selected_c_compiler = UMI_TOOL_CMAKE; Refused(UMI_STATUS_NOT_IMPLEMENTED);
    Fixture(); toolchain.tool_count = 0U; Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); toolchain.api_version = 0U; Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); (void)memset(toolchain.tools[UMI_TOOL_GCC].path,'x',sizeof(toolchain.tools[UMI_TOOL_GCC].path)); Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); (void)memset(toolchain.target_triple,'x',sizeof(toolchain.target_triple)); Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); (void)memset(toolchain.display_name,'x',sizeof(toolchain.display_name)); Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); (void)memset(toolchain.tools[UMI_TOOL_GCC].version,'x',sizeof(toolchain.tools[UMI_TOOL_GCC].version)); Refused(UMI_STATUS_INVALID_ARGUMENT);
    Fixture(); toolchain.tools[UMI_TOOL_GCC].path[0] = '\0'; Refused(UMI_STATUS_UNAVAILABLE);
    Fixture(); toolchain.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    assert(umi_compiler_profile_from_toolchain(&toolchain,"selected",&profile) == UMI_STATUS_OK);
    return 0;
}
