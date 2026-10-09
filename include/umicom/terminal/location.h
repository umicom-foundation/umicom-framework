/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/location.h
 * PURPOSE: Choose a visible, explicit working directory for a terminal session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_LOCATION_H
#define UMICOM_TERMINAL_LOCATION_H
#include "umicom/terminal/session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Validate an existing absolute directory and return its normalized spelling.
 * Relative paths are refused rather than interpreted against application storage.
 * Windows accepts local drive paths, not device or network namespaces. The
 * directory is inspected but never created. out_directory is cleared on failure.
 * This is path selection, not a filesystem sandbox or a symlink permission check. */
    UmiStatus UmiTerminalDirectorySelect(const char *path, char *out_directory, size_t capacity);
    /** Change only the chosen session's directory. An active or closed session is
 * refused; failure leaves the previous location intact. No process-wide current
 * directory, environment, command history, or other terminal is changed. */
    UmiStatus UmiTerminalSessionChooseDirectory(UmiTerminalSession *session, const char *path);
#ifdef __cplusplus
}
#endif
#endif
