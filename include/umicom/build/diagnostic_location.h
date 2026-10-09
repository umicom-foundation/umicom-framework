/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/diagnostic_location.h
 * PURPOSE: Resolve a selected compiler location against its captured build directories.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_DIAGNOSTIC_LOCATION_H
#define UMICOM_BUILD_DIAGNOSTIC_LOCATION_H
#include "umicom/build/live_diagnostics.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** A copied local source location, using one-based UTF-8 byte coordinates. */
    typedef struct UmiBuildDiagnosticLocation
    {
        char path[UMI_BUILD_PATH_CAPACITY];
        uint32_t line;
        uint32_t column;
    } UmiBuildDiagnosticLocation;
    /** Resolve one page-relative record without using the application's current
 * directory. Absolute compiler paths must name an existing regular file.
 * Relative paths are checked under the captured source and build directories;
 * two different existing candidates return INVALID_STATE rather than guessing.
 * Relative paths escaping both roots, URI schemes, pseudo-files and absent
 * source positions are refused. This reads filesystem metadata only; it does
 * not open an editor, execute a command or establish a filesystem sandbox.
 * Output is unchanged on failure. Call after checking the page's operation
 * identity against its live producer; the resolver cannot detect a stale page. */
    UmiStatus UmiBuildDiagnosticResolveLocation(const UmiBuildDiagnosticPage *page, size_t row,
                                                UmiBuildDiagnosticLocation *out_location);
#ifdef __cplusplus
}
#endif
#endif
