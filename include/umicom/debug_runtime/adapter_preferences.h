/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/adapter_preferences.h
 * PURPOSE: Validate and persist a local native debugger choice without executing it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_ADAPTER_PREFERENCES_H
#define UMICOM_DEBUG_RUNTIME_ADAPTER_PREFERENCES_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDebugAdapterPreferences
    {
        char kind[16];
        char executable[1024];
    } UmiDebugAdapterPreferences;
    /* Store "gdb" or "lldb" plus an absolute UTF-8 executable path, or empty for
 * the adapter's normal PATH discovery. Validation checks syntax, not whether
 * an executable exists, supports DAP or is trusted. No arguments or shell
 * fragments are accepted as a separate field. All calls are non-executing. */
    UmiStatus UmiDebugAdapterPreferencesValidate(const UmiDebugAdapterPreferences *preferences);
    /* JSON is a small local preferences document, not a debugger launch request.
 * Complete validation precedes publication; outputs are unchanged on failure.
 * Encode capacity includes the terminator; outSize excludes it. Decode uses
 * exactly size input bytes. Inputs and outputs must not overlap. */
    UmiStatus UmiDebugAdapterPreferencesEncode(const UmiDebugAdapterPreferences *preferences, char *out,
                                               size_t capacity, size_t *outSize);
    UmiStatus UmiDebugAdapterPreferencesDecode(const void *bytes, size_t size,
                                               UmiDebugAdapterPreferences *out);
    /* Resolve a per-user configuration path without creating directories. The
 * optional absolute baseOverride isolates portable hosts and test fixtures.
 * A saved adapter belongs to this user/application, not to a source checkout. */
    UmiStatus UmiDebugAdapterPreferencesPath(const char *applicationDirectory, const char *baseOverride,
                                             char *out, size_t capacity);
    /* Explicit local I/O. Save replaces this owned settings file using
 * UmiLocalFileReplace; its trusted parent must already exist. Load accepts a
 * regular file up to 8192 bytes. Neither changes a debugger or grants trust.
 * Disk access may block; interactive hosts should use a worker. */
    UmiStatus UmiDebugAdapterPreferencesLoad(const char *path, UmiDebugAdapterPreferences *out);
    UmiStatus UmiDebugAdapterPreferencesSave(const char *path, const UmiDebugAdapterPreferences *preferences);
#ifdef __cplusplus
}
#endif
#endif
