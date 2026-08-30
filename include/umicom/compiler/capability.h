/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/capability.h
 *
 * PURPOSE:
 *   Publish the public capability contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_CAPABILITY_H
#define UMICOM_COMPILER_CAPABILITY_H
#include "umicom/compiler/profile.h"
typedef struct UmiCompilerCapabilitySnapshot {
    char provider_id[UMI_COMPILER_ID_CAPACITY];
    char profile_id[UMI_COMPILER_ID_CAPACITY];
    UmiCompilerFamily family;
    uint32_t language_mask;
    char target[UMI_COMPILER_NAME_CAPACITY];
    bool available;
    bool cross_compilation;
    bool response_files;
    bool compilation_database;
    bool stable_c_abi;
    bool debug_information;
    bool sanitizers;
    bool link_time_optimisation;
} UmiCompilerCapabilitySnapshot;
UmiStatus umi_compiler_capability_capture(const UmiCompilerProvider *provider,const UmiCompilerProfile *profile,UmiCompilerCapabilitySnapshot *out_snapshot);
bool umi_compiler_capability_satisfies(const UmiCompilerCapabilitySnapshot *snapshot,UmiCompilerLanguage language,bool require_cross,bool require_c_abi);
#endif
