/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain_profile_storage.c
 *
 * PURPOSE:
 *   Verify bounded toolchain record storage independently of discovery or environment preparation.
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
#include "umicom/toolchain/profile.h"
static UmiToolchainProfile profile;
#define CHECK_TEXT(member) do { umi_toolchain_profile_init(&profile); \
    (void)memset(profile.member,'x',sizeof(profile.member)); \
    assert(!umi_toolchain_profile_storage_valid(&profile)); } while (0)
int main(void)
{
    assert(!umi_toolchain_profile_storage_valid(NULL));
    umi_toolchain_profile_init(&profile);
    assert(umi_toolchain_profile_storage_valid(&profile)); /* draft fields can be empty */
    CHECK_TEXT(profile_id);
    CHECK_TEXT(display_name);
    CHECK_TEXT(root);
    CHECK_TEXT(bin_directory);
    CHECK_TEXT(prefix_directory);
    CHECK_TEXT(generator);
    CHECK_TEXT(target_triple);
    CHECK_TEXT(tools[UMI_TOOL_GCC].path);
    CHECK_TEXT(tools[UMI_TOOL_GCC].version);
    umi_toolchain_profile_init(&profile); profile.structure_size = 0U;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    umi_toolchain_profile_init(&profile); profile.api_version = 0U;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    umi_toolchain_profile_init(&profile); profile.tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY+1U;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    umi_toolchain_profile_init(&profile); profile.tools[UMI_TOOL_GCC].kind = UMI_TOOL_CLANG;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    umi_toolchain_profile_init(&profile); profile.tools[UMI_TOOL_GCC].state = (UmiToolState)-1;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    umi_toolchain_profile_init(&profile); profile.tools[UMI_TOOL_GCC].state = (UmiToolState)99;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    umi_toolchain_profile_init(&profile); profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_FAILED;
    assert(umi_toolchain_profile_storage_valid(&profile)); /* validity is not usability */
    profile.tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY;
    assert(umi_toolchain_profile_storage_valid(&profile)); /* reserved ABI slots */
    return 0;
}
