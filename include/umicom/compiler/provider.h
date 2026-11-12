/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/provider.h
 *
 * PURPOSE:
 *   Publish the public provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_PROVIDER_H
#define UMICOM_COMPILER_PROVIDER_H
#include "umicom/compiler/plan.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the compiler provider descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiCompilerProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char provider_id[UMI_COMPILER_ID_CAPACITY];
    char display_name[UMI_COMPILER_NAME_CAPACITY];
    char executable[UMI_COMPILER_PATH_CAPACITY];
    char linker_executable[UMI_COMPILER_PATH_CAPACITY];
    char default_target[UMI_COMPILER_NAME_CAPACITY];
    UmiCompilerFamily family;
    uint32_t language_mask;
    bool available;
    bool supports_cross_compilation;
    bool supports_compilation_database;
    bool supports_response_files;
    bool supports_stable_c_abi;
} UmiCompilerProviderDescriptor;
/**
 * Represent the compiler provider data shared with callers of this public contract.
 */
typedef struct UmiCompilerProvider { UmiCompilerProviderDescriptor descriptor; uint64_t revision; } UmiCompilerProvider;
/**
 * Initialise compiler provider from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_provider_init(UmiCompilerProvider *provider,const UmiCompilerProviderDescriptor *descriptor);
/**
 * Provide the compiler provider supports operation used by this module and its client
 * applications.
 */
bool umi_compiler_provider_supports(const UmiCompilerProvider *provider,UmiCompilerLanguage language);
/** Query whether this planner maps an action for the declared family/language.
 * This is independent of available, executable existence or toolchain qualification.
 * Compile and assembly retain their existing mappings. C/C++ checks are mapped
 * for GCC, Clang and MSVC. Link/archive and other checks are currently unmapped. */
bool umi_compiler_provider_can_plan_action(const UmiCompilerProvider *provider,
                                          UmiCompilerLanguage language,
                                          UmiCompilerAction action);
/**
 * Provide the compiler provider plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_provider_plan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan);
/**
 * Provide the compiler family name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_family_name(UmiCompilerFamily family);
/**
 * Initialization preserves the destination on refusal and accepts a descriptor
 * borrowed from that provider. All descriptor text must terminate inside its arrays.
 * Planning rejects malformed descriptor storage before writing the plan. Request
 * validity and plan construction have their own contracts; no compiler is launched.
 */
/**
 * Build a complete plan in temporary heap storage. Every failure leaves out_plan
 * unchanged; successful publication rebases argument_view into out_plan.
 * No compiler process is started. Callers serialize inputs and output throughout.
 * Unmapped actions return NOT_IMPLEMENTED without replacing the old plan. A CHECK
 * plan omits generated object-output and debug-artifact switches; request validation
 * still requires the existing output_path field even though CHECK does not use it.
 * Explicit raw options are retained and can alter driver behavior. Inspect them
 * through the application policy before execution; this planner is not a sandbox.
 */
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
