/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/tool_location.c
 * PURPOSE: Keep tool program selection consistent across all build phases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/tool_location.h"
#include "umicom/platform/path.h"
#include "umicom/platform/process_search_path.h"
#include <stdio.h>
#include <string.h>
UmiStatus UmiBuildToolProgram(const UmiBuildProfile *profile, const char *name, char *out,
                              size_t capacity)
{
    if (name == NULL || (strcmp(name, "cmake") != 0 && strcmp(name, "ctest") != 0 &&
                         strcmp(name, "cpack") != 0 && strcmp(name, "ninja") != 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    /* Platform owns host filename rules; build providers restrict their
     * supported tools without duplicating Windows suffix or PATH handling. */
    return UmiProcessToolProgram(profile->tool_directory, name, out, capacity);
}
