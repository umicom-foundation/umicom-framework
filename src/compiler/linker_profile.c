/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/linker_profile.c
 *
 * PURPOSE:
 *   Implement the linker profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/linker_profile.h"
#include <stdio.h>
#include <string.h>
#include "catalogue_validation.h"
#include "../base/value_construction_internal.h"

bool umi_linker_profile_storage_valid(const UmiLinkerProfile *profile)
{
    if (profile == NULL || profile->library_path_count > UMI_LINKER_MAX_PATHS ||
        profile->library_count > UMI_LINKER_MAX_LIBRARIES ||
        !UmiCompilerCatalogueText(profile->profile_id,sizeof(profile->profile_id),false) ||
        !UmiCompilerCatalogueText(profile->driver,sizeof(profile->driver),false) ||
        !UmiCompilerCatalogueTarget(&profile->target)) return false;
    for (size_t index = 0U; index < profile->library_path_count; ++index)
        if (!UmiCompilerCatalogueText(profile->library_paths[index],sizeof(profile->library_paths[index]),true)) return false;
    for (size_t index = 0U; index < profile->library_count; ++index)
        if (!UmiCompilerCatalogueText(profile->libraries[index],sizeof(profile->libraries[index]),true)) return false;
    return true;
}

/*
 * Initialise linker profile from caller-provided values so later operations receive a
 * known state.
 */
/* Reuse Framework staged-value construction to preserve the previous profile and borrowed identifier, driver and target inputs until the replacement succeeds. The previous implementation is retained for review. */
#if 0
UmiStatus umi_linker_profile_init(UmiLinkerProfile *profile,const char *profile_id,const char *driver,const UmiCompilerTarget *target)
{
    int first; int second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile_id == NULL || driver == NULL || target == NULL || profile_id[0] == '\0' || driver[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(profile,0,sizeof(*profile)); first = snprintf(profile->profile_id,sizeof(profile->profile_id),"%s",profile_id); second = snprintf(profile->driver,sizeof(profile->driver),"%s",driver); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (first < 0 || second < 0 || (size_t)first >= sizeof(profile->profile_id) || (size_t)second >= sizeof(profile->driver)) return UMI_STATUS_CAPACITY_EXCEEDED;
    profile->target = *target; profile->object_format = target->object_format; profile->revision = 1U; return UMI_STATUS_OK;
}
#endif
/* The existing shared wrapper allocates this large inline profile off-stack.
 * This builder reads all borrowed inputs before publication by that wrapper. */
static UmiStatus UmiLinkerProfileBuild(UmiLinkerProfile *profile,const char *profile_id,const char *driver,const UmiCompilerTarget *target)
{
    if (profile_id == NULL || driver == NULL || profile_id[0] == '\0' || driver[0] == '\0' ||
        !UmiCompilerCatalogueTarget(target)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiCompilerCatalogueText(profile_id,sizeof(profile->profile_id),true) ||
        !UmiCompilerCatalogueText(driver,sizeof(profile->driver),true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(profile,0,sizeof(*profile));
    (void)memcpy(profile->profile_id,profile_id,strlen(profile_id) + 1U);
    (void)memcpy(profile->driver,driver,strlen(driver) + 1U);
    profile->target = *target;
    profile->object_format = target->object_format;
    profile->revision = 1U;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_linker_profile_init, UmiLinkerProfile, profile,
    (UmiLinkerProfile *profile, const char *profile_id, const char *driver, const UmiCompilerTarget *target),
    UmiLinkerProfileBuild(candidate, profile_id, driver, target))
/*
 * Provide the linker profile add path operation used by this module and its client
 * applications.
 */
/* Admit all active profile storage, slot capacity and revision capacity before copying an overlap-safe addition; refusal preserves every owner byte. The previous implementation is retained for review. */
#if 0
UmiStatus umi_linker_profile_add_path(UmiLinkerProfile *profile,const char *path) { int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile->library_path_count >= UMI_LINKER_MAX_PATHS) return UMI_STATUS_CAPACITY_EXCEEDED; length = snprintf(profile->library_paths[profile->library_path_count],sizeof(profile->library_paths[0]),"%s",path); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= sizeof(profile->library_paths[0])) return UMI_STATUS_CAPACITY_EXCEEDED; profile->library_path_count += 1U; profile->revision += 1U; return UMI_STATUS_OK; }
#endif
UmiStatus umi_linker_profile_add_path(UmiLinkerProfile *profile,const char *path)
{
    if (profile == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_linker_profile_storage_valid(profile)) return UMI_STATUS_INVALID_STATE;
    if (profile->library_path_count == UMI_LINKER_MAX_PATHS || profile->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!UmiCompilerCatalogueText(path,sizeof(profile->library_paths[0]),true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memmove(profile->library_paths[profile->library_path_count],path,strlen(path) + 1U);
    profile->library_path_count += 1U;
    profile->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the linker profile add library operation used by this module and its client
 * applications.
 */
/* Admit all active profile storage, slot capacity and revision capacity before copying an overlap-safe addition; refusal preserves every owner byte. The previous implementation is retained for review. */
#if 0
UmiStatus umi_linker_profile_add_library(UmiLinkerProfile *profile,const char *library) { int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL || library == NULL || library[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile->library_count >= UMI_LINKER_MAX_LIBRARIES) return UMI_STATUS_CAPACITY_EXCEEDED; length = snprintf(profile->libraries[profile->library_count],sizeof(profile->libraries[0]),"%s",library); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= sizeof(profile->libraries[0])) return UMI_STATUS_CAPACITY_EXCEEDED; profile->library_count += 1U; profile->revision += 1U; return UMI_STATUS_OK; }
#endif
UmiStatus umi_linker_profile_add_library(UmiLinkerProfile *profile,const char *library)
{
    if (profile == NULL || library == NULL || library[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_linker_profile_storage_valid(profile)) return UMI_STATUS_INVALID_STATE;
    if (profile->library_count == UMI_LINKER_MAX_LIBRARIES || profile->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!UmiCompilerCatalogueText(library,sizeof(profile->libraries[0]),true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memmove(profile->libraries[profile->library_count],library,strlen(library) + 1U);
    profile->library_count += 1U;
    profile->revision += 1U;
    return UMI_STATUS_OK;
}
/* Check that linker profile satisfies its contract before another service relies on it. */
/* Check bounded profile storage before applying the established completeness policy; validation does not probe a linker or certify target compatibility. The previous implementation is retained for review. */
#if 0
UmiStatus umi_linker_profile_validate(const UmiLinkerProfile *profile,char *out_reason,size_t capacity) { int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile->profile_id[0] == '\0' || profile->driver[0] == '\0' || profile->target.triple[0] == '\0' || profile->object_format == UMI_COMPILER_OBJECT_UNKNOWN) { length = snprintf(out_reason,capacity,"Linker profile is incomplete"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; } length = snprintf(out_reason,capacity,"Linker profile is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
#endif
UmiStatus umi_linker_profile_validate(const UmiLinkerProfile *profile,char *out_reason,size_t capacity)
{
    if (profile == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "Linker profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (!umi_linker_profile_storage_valid(profile)) {
        reason = "Linker profile storage is malformed"; status = UMI_STATUS_INVALID_STATE;
    } else if (profile->profile_id[0] == '\0' || profile->driver[0] == '\0' ||
        profile->target.triple[0] == '\0' || profile->object_format == UMI_COMPILER_OBJECT_UNKNOWN) {
        reason = "Linker profile is incomplete"; status = UMI_STATUS_INVALID_STATE;
    }
    const int length = snprintf(out_reason,capacity,"%s",reason);
    return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : status;
}
