/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_environment.c
 * PURPOSE: Implement deterministic child-process environment construction.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_environment.h"

static UmiStatus set_if_present(UmiEnvironmentPlan *plan,
                                const char *name,
                                const char *value)
{
    return value != NULL && value[0] != '\0'
        ? umi_environment_plan_set(plan, name, value)
        : UMI_STATUS_OK;
}

UmiStatus umi_toolchain_kit_environment_plan(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan)
{
    UmiStatus status;
    if (kit == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (profile != NULL) {
        status = umi_environment_plan_from_toolchain(profile, out_plan);
        if (status != UMI_STATUS_OK) return status;
    } else {
        umi_environment_plan_init(out_plan);
    }
#define SET(name, value)                                                          \
    do {                                                                           \
        status = set_if_present(out_plan, (name), (value));                        \
        if (status != UMI_STATUS_OK) return status;                                \
    } while (0)
    SET("UMICOM_TOOLCHAIN_KIT", kit->id);
    SET("UMICOM_TOOLCHAIN_PROFILE", kit->profile_id);
    SET("UMICOM_SDK", kit->sdk_id);
    SET("UMICOM_HOST_TRIPLE", kit->host_triple);
    SET("UMICOM_TARGET_TRIPLE", kit->target_triple);
    SET("UMICOM_SYSROOT", kit->sysroot);
    SET("UMICOM_ENVIRONMENT_PROFILE", kit->environment_profile);
    SET("UMICOM_CMAKE_TOOLCHAIN_FILE", kit->cmake_toolchain_file);
    SET("CC", kit->c_compiler);
    SET("CXX", kit->cpp_compiler);
    SET("AS", kit->assembler);
    SET("LD", kit->linker);
    SET("CMAKE_GENERATOR", kit->cmake_generator);
    SET("CMAKE_MAKE_PROGRAM", kit->ninja);
    SET("UMICOM_DEBUGGER", kit->debugger);
    SET("UMICOM_GIT", kit->git);
    SET("UMICOM_PYTHON", kit->python);
    SET("UMICOM_JAVA", kit->java);
    SET("UMICOM_JAVAC", kit->javac);
#undef SET
    return umi_environment_plan_validate(out_plan);
}
