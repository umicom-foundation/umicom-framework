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
/* Umicom Framework | Linker profiles | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/linker_profile.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise linker profile from caller-provided values so later operations receive a
 * known state.
 */
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
/*
 * Provide the linker profile add path operation used by this module and its client
 * applications.
 */
UmiStatus umi_linker_profile_add_path(UmiLinkerProfile *profile,const char *path) { int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile->library_path_count >= UMI_LINKER_MAX_PATHS) return UMI_STATUS_CAPACITY_EXCEEDED; length = snprintf(profile->library_paths[profile->library_path_count],sizeof(profile->library_paths[0]),"%s",path); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= sizeof(profile->library_paths[0])) return UMI_STATUS_CAPACITY_EXCEEDED; profile->library_path_count += 1U; profile->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the linker profile add library operation used by this module and its client
 * applications.
 */
UmiStatus umi_linker_profile_add_library(UmiLinkerProfile *profile,const char *library) { int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL || library == NULL || library[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile->library_count >= UMI_LINKER_MAX_LIBRARIES) return UMI_STATUS_CAPACITY_EXCEEDED; length = snprintf(profile->libraries[profile->library_count],sizeof(profile->libraries[0]),"%s",library); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= sizeof(profile->libraries[0])) return UMI_STATUS_CAPACITY_EXCEEDED; profile->library_count += 1U; profile->revision += 1U; return UMI_STATUS_OK; }
/* Check that linker profile satisfies its contract before another service relies on it. */
UmiStatus umi_linker_profile_validate(const UmiLinkerProfile *profile,char *out_reason,size_t capacity) { int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile->profile_id[0] == '\0' || profile->driver[0] == '\0' || profile->target.triple[0] == '\0' || profile->object_format == UMI_COMPILER_OBJECT_UNKNOWN) { length = snprintf(out_reason,capacity,"Linker profile is incomplete"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; } length = snprintf(out_reason,capacity,"Linker profile is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
