/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/linker_profile.h
 *
 * PURPOSE:
 *   Publish the public linker profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_LINKER_PROFILE_H
#define UMICOM_COMPILER_LINKER_PROFILE_H
#include "umicom/compiler/target.h"
#define UMI_LINKER_MAX_PATHS 24U
#define UMI_LINKER_MAX_LIBRARIES 48U
/**
 * Represent the linker profile data shared with callers of this public contract.
 */
typedef struct UmiLinkerProfile {
    char profile_id[UMI_COMPILER_ID_CAPACITY];
    char driver[UMI_COMPILER_PATH_CAPACITY];
    UmiCompilerTarget target;
    UmiCompilerObjectFormat object_format;
    char library_paths[UMI_LINKER_MAX_PATHS][UMI_COMPILER_PATH_CAPACITY];
    char libraries[UMI_LINKER_MAX_LIBRARIES][UMI_COMPILER_NAME_CAPACITY];
    size_t library_path_count;
    size_t library_count;
    bool static_runtime;
    bool strip_symbols;
    uint64_t revision;
} UmiLinkerProfile;
/**
 * Initialise linker profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_linker_profile_init(UmiLinkerProfile *profile,const char *profile_id,const char *driver,const UmiCompilerTarget *target);
/**
 * Provide the linker profile add path operation used by this module and its client
 * applications.
 */
UmiStatus umi_linker_profile_add_path(UmiLinkerProfile *profile,const char *path);
/**
 * Provide the linker profile add library operation used by this module and its client
 * applications.
 */
UmiStatus umi_linker_profile_add_library(UmiLinkerProfile *profile,const char *library);
/**
 * Check that linker profile satisfies its contract before another service relies on it.
 */
UmiStatus umi_linker_profile_validate(const UmiLinkerProfile *profile,char *out_reason,size_t capacity);
#endif
