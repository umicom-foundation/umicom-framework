/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/tool_location.h
 * PURPOSE: Resolve build tools from an explicitly reviewed project tool folder.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_TOOL_LOCATION_H
#define UMICOM_BUILD_TOOL_LOCATION_H
#include "umicom/build/profile.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * Select cmake, ctest, cpack or ninja from profile.tool_directory. An empty
     * directory returns the conventional bare program name. An explicit directory
     * returns an absolute host executable path and never falls back to PATH.
     * The output is unchanged on failure. No tool is started or checked for
     * existence. name is one of the four lower-case conventional names above.
     */
    UmiStatus UmiBuildToolProgram(const UmiBuildProfile *profile, const char *name, char *out,
                                  size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
