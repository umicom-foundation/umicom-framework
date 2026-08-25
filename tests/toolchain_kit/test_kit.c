/* Umicom Framework | toolchain kit contract test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit.h"

int main(void)
{
    UmiToolchainProfile profile;
    UmiSdkSnapshot sdk;
    UmiToolchainKitSnapshot kit;
    umi_toolchain_profile_init(&profile);
    (void)strcpy(profile.profile_id, "windows-ucrt64-gcc");
    (void)strcpy(profile.display_name, "Windows UCRT64 GCC");
    (void)strcpy(profile.target_triple, "x86_64-w64-mingw32");
    (void)strcpy(profile.generator, "Ninja");
    profile.family = UMI_TOOLCHAIN_MSYS2_UCRT64;
    profile.selected_c_compiler = UMI_TOOL_GCC;
    profile.selected_cpp_compiler = UMI_TOOL_GXX;
    profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_GXX].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_CMAKE].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_CTEST].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_NINJA].state = UMI_TOOL_VALIDATED;
    profile.tools[UMI_TOOL_GIT].state = UMI_TOOL_VALIDATED;
    (void)strcpy(profile.tools[UMI_TOOL_GCC].path, "C:/msys64/ucrt64/bin/gcc.exe");
    (void)strcpy(profile.tools[UMI_TOOL_GXX].path, "C:/msys64/ucrt64/bin/g++.exe");
    (void)strcpy(profile.tools[UMI_TOOL_CMAKE].path, "C:/msys64/ucrt64/bin/cmake.exe");
    (void)strcpy(profile.tools[UMI_TOOL_CTEST].path, "C:/msys64/ucrt64/bin/ctest.exe");
    (void)strcpy(profile.tools[UMI_TOOL_NINJA].path, "C:/msys64/ucrt64/bin/ninja.exe");
    (void)strcpy(profile.tools[UMI_TOOL_GIT].path, "C:/Program Files/Git/cmd/git.exe");
    profile.complete = 1;
    profile.c23_capable = 1;

    (void)memset(&sdk, 0, sizeof(sdk));
    (void)strcpy(sdk.id, "msys2-ucrt64");
    sdk.available = 1;
    assert(umi_toolchain_kit_from_profile(&kit, &profile, &sdk,
                                          "x86_64-w64-mingw32") == UMI_STATUS_OK);
    assert(umi_toolchain_kit_validate(&kit) == UMI_STATUS_OK);
    assert(kit.state == UMI_TOOLCHAIN_KIT_READY);
    assert(umi_toolchain_kit_supports(&kit,
        UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD |
        UMI_TOOLCHAIN_KIT_CAPABILITY_C23 |
        UMI_TOOLCHAIN_KIT_CAPABILITY_SDK));
    assert(!umi_toolchain_kit_is_cross_compile(&kit));
    return 0;
}
