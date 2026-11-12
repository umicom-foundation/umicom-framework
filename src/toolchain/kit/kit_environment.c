/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_environment.c
 *
 * PURPOSE:
 *   Implement deterministic child-process environment construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_environment.h"

#include <stdlib.h>

/* Provide the set if present operation used by this module and its client applications. */
static UmiStatus set_if_present(UmiEnvironmentPlan *plan,
                                const char *name,
                                const char *value)
{
    return value != NULL && value[0] != '\0'
        ? umi_environment_plan_set(plan, name, value)
        : UMI_STATUS_OK;
}

/*
 * Provide the toolchain kit environment plan operation used by this module and its client
 * applications.
 */
/* Admit the kit and stage all overrides before publishing the environment.
 * Retain incremental construction below for engineering review. */
#if 0
UmiStatus umi_toolchain_kit_environment_plan(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile != NULL) {
        status = umi_environment_plan_from_toolchain(profile, out_plan);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
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
#endif
UmiStatus umi_toolchain_kit_environment_plan(
    const UmiToolchainKitSnapshot *kit, const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan)
{
    if (!out_plan) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_toolchain_kit_validate(kit);
    if (status != UMI_STATUS_OK) return status;
    /* A selected kit adds several values to the profile. Keep this composition
     * private until every override fits, so the last usable plan survives. */
    UmiEnvironmentPlan *candidate = calloc(1U, sizeof *candidate);
    if (!candidate) return UMI_STATUS_OUT_OF_MEMORY;
    if (profile) status = umi_environment_plan_from_toolchain(profile, candidate);
    const struct { const char *name; const char *value; } overrides[] = {
        {"UMICOM_TOOLCHAIN_KIT", kit->id},
        {"UMICOM_TOOLCHAIN_PROFILE", kit->profile_id},
        {"UMICOM_SDK", kit->sdk_id},
        {"UMICOM_HOST_TRIPLE", kit->host_triple},
        {"UMICOM_TARGET_TRIPLE", kit->target_triple},
        {"UMICOM_SYSROOT", kit->sysroot},
        {"UMICOM_ENVIRONMENT_PROFILE", kit->environment_profile},
        {"UMICOM_CMAKE_TOOLCHAIN_FILE", kit->cmake_toolchain_file},
        {"CC", kit->c_compiler}, {"CXX", kit->cpp_compiler},
        {"AS", kit->assembler}, {"LD", kit->linker},
        {"CMAKE_GENERATOR", kit->cmake_generator},
        {"CMAKE_MAKE_PROGRAM", kit->ninja}, {"UMICOM_DEBUGGER", kit->debugger},
        {"UMICOM_GIT", kit->git}, {"UMICOM_PYTHON", kit->python},
        {"UMICOM_JAVA", kit->java}, {"UMICOM_JAVAC", kit->javac}
    };
    for (size_t i = 0; i < sizeof overrides / sizeof overrides[0] && status == UMI_STATUS_OK; ++i)
        status = set_if_present(candidate, overrides[i].name, overrides[i].value);
    if (status == UMI_STATUS_OK) status = umi_environment_plan_validate(candidate);
    if (status == UMI_STATUS_OK) {
        *out_plan = *candidate;
        /* The validated copy owns text at a new address; use Framework's
         * projection service to rebind its borrowed process entries. */
        (void)umi_environment_plan_variables(out_plan);
    }
    free(candidate);
    return status;
}
