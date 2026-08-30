/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/profile.h
 *
 * PURPOSE:
 *   Publish the public profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_PROFILE_H
#define UMICOM_COMPILER_PROFILE_H
#include "umicom/compiler/linker_profile.h"
#include "umicom/compiler/policy.h"
#include "umicom/compiler/registry.h"
typedef struct UmiCompilerProfile {
    char profile_id[UMI_COMPILER_ID_CAPACITY];
    char display_name[UMI_COMPILER_NAME_CAPACITY];
    char provider_id[UMI_COMPILER_ID_CAPACITY];
    char standard[64U];
    UmiCompilerLanguage primary_language;
    UmiCompilerTarget target;
    UmiLinkerProfile linker;
    UmiCompilerPolicy policy;
    bool enabled;
    uint64_t revision;
} UmiCompilerProfile;
UmiStatus umi_compiler_profile_init(UmiCompilerProfile *profile,const char *profile_id,const char *display_name,const char *provider_id,UmiCompilerLanguage language,const UmiCompilerTarget *target);
UmiStatus umi_compiler_profile_validate(const UmiCompilerProfile *profile,const UmiCompilerRegistry *registry,char *out_reason,size_t capacity);
#endif
