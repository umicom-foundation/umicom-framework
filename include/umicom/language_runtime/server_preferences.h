/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server_preferences.h
 * PURPOSE: Persist an explicit per-language server choice in user-local application settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVER_PREFERENCES_H
#define UMICOM_LANGUAGE_RUNTIME_SERVER_PREFERENCES_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageServerPreferences
    {
        char language_id[64];
        char executable[1024];
        char arguments[2048];
    } UmiLanguageServerPreferences;
    /* Validate terminated UTF-8 fields and a nonempty absolute executable path.
 * Language IDs are case-sensitive and may contain printable Unicode. Argument
 * spelling uses Framework's literal, shell-independent argument grammar.
 * This checks syntax only: it does not discover, trust or start an executable.
 * Store ordinary process flags here, not credentials; this is a local JSON
 * preferences record, not an encrypted secret store. */
    UmiStatus UmiLanguageServerPreferencesValidate(const UmiLanguageServerPreferences *preferences);
    /* Strict JSON with format, language, executable and arguments fields.
 * Unknown/missing/duplicate fields, malformed text and unsupported format are
 * refused. Outputs remain unchanged on failure. Inputs must not overlap the
 * destination. Encode requires room for NUL; out_size excludes that terminator.
 * Decode accepts an exact byte span, at most 32768 bytes. */
    UmiStatus UmiLanguageServerPreferencesEncode(const UmiLanguageServerPreferences *preferences, char *out,
                                                 size_t capacity, size_t *out_size);
    UmiStatus UmiLanguageServerPreferencesDecode(const void *bytes, size_t size,
                                                 UmiLanguageServerPreferences *out);
    /* Resolve one per-language file below the user's application config folder.
 * Hexadecimal filename encoding distinguishes case-sensitive language IDs even
 * on case-insensitive filesystems and never interprets an ID as a path.
 * No directories are created. An absolute base_override supports portable
 * hosts and isolated tests. Output is unchanged on error. */
    UmiStatus UmiLanguageServerPreferencesPath(const char *application_directory, const char *base_override,
                                               const char *language_id, char *out, size_t capacity);
    /* Explicit local I/O: read a regular file, or atomically replace an owned
 * settings file through UmiLocalFileReplace. Save requires its trusted parent
 * to exist and inherits that service's concurrency/metadata/durability limits.
 * Load verifies the expected language before returning a draft. Neither call
 * starts a server or changes a registry. Use a worker for filesystem access. */
    UmiStatus UmiLanguageServerPreferencesLoad(const char *path, const char *expected_language_id,
                                               UmiLanguageServerPreferences *out);
    UmiStatus UmiLanguageServerPreferencesSave(const char *path,
                                               const UmiLanguageServerPreferences *preferences);
#ifdef __cplusplus
}
#endif
#endif
