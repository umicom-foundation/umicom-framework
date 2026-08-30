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
UmiStatus umi_linker_profile_init(UmiLinkerProfile *profile,const char *profile_id,const char *driver,const UmiCompilerTarget *target);
UmiStatus umi_linker_profile_add_path(UmiLinkerProfile *profile,const char *path);
UmiStatus umi_linker_profile_add_library(UmiLinkerProfile *profile,const char *library);
UmiStatus umi_linker_profile_validate(const UmiLinkerProfile *profile,char *out_reason,size_t capacity);
#endif
