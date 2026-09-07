/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/application_manifest.c
 *
 * PURPOSE:
 *   Implement the application manifest behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/application_manifest.c
 *
 * PURPOSE:
 *   Implement the deterministic application manifest parser used by native
 *   tools, Studio, generated applications, product profiles, and suites.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/application_manifest.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

/* Provide the manifest trim operation used by this module and its client applications. */
static char *umi_manifest_trim(char *text)
{
    char *end;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }
    end = text + strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

/* Copy manifest into module-owned storage so callers keep ownership of their input values. */
static UmiStatus umi_manifest_copy(char *destination,
                                   size_t capacity,
                                   const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the manifest frontend flag operation used by this module and its client
 * applications.
 */
static unsigned umi_manifest_frontend_flag(const char *name)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "console") == 0) return UMI_FRONTEND_CONSOLE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "gtk") == 0 || strcmp(name, "gtk4") == 0)
        return UMI_FRONTEND_GTK4;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "web") == 0) return UMI_FRONTEND_WEB;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "qt") == 0 || strcmp(name, "qt6") == 0)
        return UMI_FRONTEND_QT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "wt") == 0) return UMI_FRONTEND_WT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "mobile") == 0) return UMI_FRONTEND_MOBILE;
    return UMI_FRONTEND_NONE;
}

/*
 * Provide the manifest parse inline aliases operation used by this module and its client
 * applications.
 */
static void umi_manifest_parse_inline_aliases(UmiApplicationManifest *manifest,
                                               char *value)
{
    char *cursor = value;
    char *token;

    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor == '[') {
        ++cursor;
    }
    token = strtok(cursor, ",]");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (token != NULL && manifest->alias_count < UMI_MANIFEST_MAX_ALIASES) {
        char *trimmed = umi_manifest_trim(token);
        /* Apply this branch only when its contract condition is satisfied. */
        if (*trimmed != '\0') {
            (void)umi_manifest_copy(
                manifest->aliases[manifest->alias_count],
                sizeof(manifest->aliases[0]),
                trimmed
            );
            manifest->alias_count += 1U;
        }
        token = strtok(NULL, ",]");
    }
}

/*
 * Initialise application manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_application_manifest_init(UmiApplicationManifest *manifest)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest != NULL) {
        (void)memset(manifest, 0, sizeof(*manifest));
    }
}

/*
 * Parse mutable, caller-staged text using the established manifest subset.
 * Explicit executable declarations are checked before any snapshot is published.
 */
static UmiStatus umi_manifest_parse_mutable(
    char *text,
    UmiApplicationManifest *out_manifest,
    UmiApplicationLaunchSpec *out_launch_spec)
{
    char *line;
    char *save_pointer = NULL;
    enum { SECTION_NONE, SECTION_APPLICATION, SECTION_FRAMEWORK,
           SECTION_FRONTENDS, SECTION_CAPABILITIES } section = SECTION_NONE;
    UmiStatus status;
    unsigned seen_executable = 0U;
    unsigned seen_native = 0U;
    unsigned seen_console = 0U;
    unsigned seen_schema = 0U;
    int flat_manifest = 0;

    line = strtok_r(text, "\n", &save_pointer);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (line != NULL) {
        const int top_level = !isspace((unsigned char)line[0]);
        char *trimmed = umi_manifest_trim(line);
        const size_t indentation = (size_t)(trimmed - line);
        size_t length = strlen(trimmed);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U && trimmed[length - 1U] == '\r') {
            trimmed[length - 1U] = '\0';
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*trimmed == '\0' || *trimmed == '#') {
            line = strtok_r(NULL, "\n", &save_pointer);
            continue;
        }

        /* Shape must be known before identity is read. A late schema must not
         * reinterpret earlier nested values as a different generated form. */
        if (!seen_schema && strncmp(trimmed, "schema:", 7U) != 0)
            return UMI_STATUS_PARSE_ERROR;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strncmp(trimmed, "schema:", 7U) == 0) {
            if (!top_level || seen_schema++) return UMI_STATUS_PARSE_ERROR;
            status = umi_manifest_copy(out_manifest->schema,
                sizeof(out_manifest->schema), umi_manifest_trim(trimmed + 7));
            if (status != UMI_STATUS_OK) return status;
            flat_manifest = strcmp(out_manifest->schema, "umicom.application/v1") == 0;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "application:") == 0) {
            if (flat_manifest || !top_level) return UMI_STATUS_PARSE_ERROR;
            section = SECTION_APPLICATION;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "framework:") == 0) {
            if (flat_manifest || !top_level) return UMI_STATUS_PARSE_ERROR;
            section = SECTION_FRAMEWORK;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "frontends:") == 0) {
            section = SECTION_FRONTENDS;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "capabilities:") == 0) {
            section = SECTION_CAPABILITIES;
        } else if (top_level && length > 0U && trimmed[length - 1U] == ':') {
            /* Unknown top-level sections cannot masquerade as application
             * declarations merely because they follow the application block. */
            section = SECTION_NONE;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (trimmed[0] == '-') {
            char *value = umi_manifest_trim(trimmed + 1);
            /* Apply this branch only when its contract condition is satisfied. */
            if (section == SECTION_FRONTENDS) {
                out_manifest->frontends |= umi_manifest_frontend_flag(value);
            } else /* Apply this branch only when its contract condition is satisfied. */ if (section == SECTION_CAPABILITIES &&
                       out_manifest->capability_count <
                           UMI_MANIFEST_MAX_CAPABILITIES) {
                (void)umi_manifest_copy(
                    out_manifest->capabilities[
                        out_manifest->capability_count],
                    sizeof(out_manifest->capabilities[0]),
                    value
                );
                out_manifest->capability_count += 1U;
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            char *separator = strchr(trimmed, ':');
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (separator == NULL && (section == SECTION_APPLICATION || flat_manifest) &&
                (strncmp(trimmed, "native_executable", 17U) == 0 ||
                 strncmp(trimmed, "console_executable", 18U) == 0 ||
                 strncmp(trimmed, "executable", 10U) == 0))
                return UMI_STATUS_PARSE_ERROR;
            if (separator != NULL) {
                char *key;
                char *value;
                *separator = '\0';
                key = umi_manifest_trim(trimmed);
                value = umi_manifest_trim(separator + 1);
                /* Do not mix flat identity declarations into a nested file.
                 * Unrelated top-level metadata still ends the preceding block
                 * and remains forward-compatible with the stable subset. */
                if (!flat_manifest && top_level) {
                    if (strcmp(key, "id") == 0 || strcmp(key, "name") == 0 ||
                        strcmp(key, "short_name") == 0 || strcmp(key, "version") == 0 ||
                        strcmp(key, "executable") == 0 ||
                        strcmp(key, "native_executable") == 0 ||
                        strcmp(key, "console_executable") == 0 ||
                        strcmp(key, "minimum_framework_version") == 0)
                        return UMI_STATUS_PARSE_ERROR;
                    section = SECTION_NONE;
                }
                if (((flat_manifest && !top_level) ||
                     (!flat_manifest && section == SECTION_APPLICATION && indentation != 2U)) &&
                    (strcmp(key, "executable") == 0 ||
                     strcmp(key, "native_executable") == 0 ||
                     strcmp(key, "console_executable") == 0))
                    return UMI_STATUS_PARSE_ERROR;

                /* Apply this branch only when its contract condition is satisfied. */
                if ((!flat_manifest && section == SECTION_APPLICATION && indentation == 2U) ||
                    (flat_manifest && top_level)) {
                    /* Flat generator documents and nested suite documents are
                     * separate accepted shapes; do not merge their identity. */
                    /* Use the stable identifier comparison to choose the matching record or policy. */
                    if (strcmp(key, "id") == 0)
                        (void)umi_manifest_copy(out_manifest->id,
                                                sizeof(out_manifest->id), value);
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "name") == 0)
                        (void)umi_manifest_copy(out_manifest->name,
                                                sizeof(out_manifest->name), value);
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "short_name") == 0)
                        (void)umi_manifest_copy(out_manifest->short_name,
                                                sizeof(out_manifest->short_name), value);
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "version") == 0)
                        (void)umi_manifest_copy(out_manifest->version,
                                                sizeof(out_manifest->version), value);
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "executable") == 0) {
                        if (seen_executable++) return UMI_STATUS_PARSE_ERROR;
                        status = umi_manifest_copy(out_manifest->executable,
                            sizeof(out_manifest->executable), value);
                        if (status != UMI_STATUS_OK) return status;
                    } else if (strcmp(key, "native_executable") == 0) {
                        if (seen_native++) return UMI_STATUS_PARSE_ERROR;
                        status = umi_manifest_copy(out_launch_spec->native_executable,
                            sizeof(out_launch_spec->native_executable), value);
                        if (status != UMI_STATUS_OK) return status;
                    } else if (strcmp(key, "console_executable") == 0) {
                        if (seen_console++) return UMI_STATUS_PARSE_ERROR;
                        status = umi_manifest_copy(out_launch_spec->console_executable,
                            sizeof(out_launch_spec->console_executable), value);
                        if (status != UMI_STATUS_OK) return status;
                    }
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "aliases") == 0)
                        umi_manifest_parse_inline_aliases(out_manifest, value);
                    else if (flat_manifest && strcmp(key, "minimum_framework_version") == 0) {
                        status = umi_manifest_copy(out_manifest->minimum_framework_version,
                            sizeof(out_manifest->minimum_framework_version), value);
                        if (status != UMI_STATUS_OK) return status;
                    }
                } else /* Apply this branch only when its contract condition is satisfied. */ if (section == SECTION_FRAMEWORK &&
                           strcmp(key, "minimum_version") == 0) {
                    (void)umi_manifest_copy(
                        out_manifest->minimum_framework_version,
                        sizeof(out_manifest->minimum_framework_version),
                        value
                    );
                }
            }
        }
        line = strtok_r(NULL, "\n", &save_pointer);
    }

    if ((seen_native && out_launch_spec->native_executable[0] == '\0') ||
        (seen_console && out_launch_spec->console_executable[0] == '\0'))
        return UMI_STATUS_PARSE_ERROR;
    status = umi_application_manifest_validate(out_manifest, NULL, 0U);
    return status == UMI_STATUS_OK
        ? umi_application_launch_spec_validate(out_manifest, out_launch_spec)
        : status;
}

/* Initialise the additive launch contract without changing legacy ABI fields. */
void umi_application_launch_spec_init(UmiApplicationLaunchSpec *launch_spec)
{
    if (launch_spec == NULL) return;
    (void)memset(launch_spec, 0, sizeof(*launch_spec));
    launch_spec->structure_size = sizeof(*launch_spec);
}

/* Restrict explicit names to one bounded, portable executable basename. Never
 * accept a path, argument string, quote, variable expansion or shell operator. */
static int umi_manifest_executable_token_valid(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    size_t length;
    if (end == NULL) return 0;
    length = (size_t)(end - text);
    if (length == 0U) return 1;
    if (!((text[0] >= 'A' && text[0] <= 'Z') ||
          (text[0] >= 'a' && text[0] <= 'z') ||
          (text[0] >= '0' && text[0] <= '9'))) return 0;
    for (size_t index = 0U; index < length; ++index) {
        const unsigned char value = (unsigned char)text[index];
        if (!((value >= 'A' && value <= 'Z') ||
              (value >= 'a' && value <= 'z') ||
              (value >= '0' && value <= '9') ||
              value == '_' || value == '-' || value == '.')) return 0;
    }
    return text[length - 1U] != '.';
}

/* Frontend availability is declared metadata, never proof of installation or
 * business readiness. Legacy manifests may omit both explicit declarations. */
UmiStatus umi_application_launch_spec_validate(
    const UmiApplicationManifest *manifest,
    const UmiApplicationLaunchSpec *launch_spec)
{
    if (manifest == NULL || launch_spec == NULL ||
        launch_spec->structure_size != sizeof(*launch_spec))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_manifest_executable_token_valid(launch_spec->native_executable,
            sizeof(launch_spec->native_executable)) ||
        !umi_manifest_executable_token_valid(launch_spec->console_executable,
            sizeof(launch_spec->console_executable))) return UMI_STATUS_PARSE_ERROR;
    if ((launch_spec->native_executable[0] != '\0' &&
         (manifest->frontends & UMI_FRONTEND_GTK4) == 0U) ||
        (launch_spec->console_executable[0] != '\0' &&
         (manifest->frontends & UMI_FRONTEND_CONSOLE) == 0U))
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

/* Stage parsing on owned scratch memory. No caller can accidentally use a
 * partially parsed native name after an error, and no large model uses stack. */
UmiStatus umi_application_manifest_parse_with_launch_spec(
    const char *text,
    UmiApplicationManifest *out_manifest,
    UmiApplicationLaunchSpec *out_launch_spec)
{
    UmiApplicationManifest *candidate;
    UmiApplicationLaunchSpec candidate_launch;
    char *mutable_text;
    size_t length;
    UmiStatus status;
    umi_application_manifest_init(out_manifest);
    umi_application_launch_spec_init(out_launch_spec);
    if (text == NULL || out_manifest == NULL || out_launch_spec == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if (length == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = calloc(1U, sizeof(*candidate));
    mutable_text = malloc(length + 1U);
    if (candidate == NULL || mutable_text == NULL) {
        free(candidate);
        free(mutable_text);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memcpy(mutable_text, text, length + 1U);
    umi_application_launch_spec_init(&candidate_launch);
    status = umi_manifest_parse_mutable(mutable_text, candidate, &candidate_launch);
    if (status == UMI_STATUS_OK) {
        *out_manifest = *candidate;
        *out_launch_spec = candidate_launch;
    }
    free(mutable_text);
    free(candidate);
    return status;
}

/* Read once through Framework filesystem services, then use the same strict
 * companion parser as in-memory callers and native contract regressions. */
UmiStatus umi_application_manifest_load_with_launch_spec(
    const char *path,
    UmiApplicationManifest *out_manifest,
    UmiApplicationLaunchSpec *out_launch_spec)
{
    char *text = NULL;
    size_t text_size = 0U;
    UmiStatus status;
    umi_application_manifest_init(out_manifest);
    umi_application_launch_spec_init(out_launch_spec);
    if (path == NULL || out_manifest == NULL || out_launch_spec == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_read_text(path, &text, &text_size);
    /* Embedded NUL bytes are not YAML text. Reject hidden trailing declarations
     * instead of silently parsing only the file's leading fragment. */
    if (status == UMI_STATUS_OK && memchr(text, '\0', text_size) != NULL)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = umi_application_manifest_parse_with_launch_spec(
            text, out_manifest, out_launch_spec);
    umi_fs_free_text(text);
    return status;
}

/* Keep the original loader and manifest layout source-compatible. Explicit
 * fields are validated when present but never replace legacy executable text. */
UmiStatus umi_application_manifest_load(
    const char *path,
    UmiApplicationManifest *out_manifest)
{
    UmiApplicationLaunchSpec launch_spec;
    return umi_application_manifest_load_with_launch_spec(
        path, out_manifest, &launch_spec);
}

/*
 * Check that application manifest satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_manifest_validate(
    const UmiApplicationManifest *manifest,
    char *out_message,
    size_t capacity)
{
    const char *message = "Application manifest is valid";
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(manifest->schema, "umicom.application.v1") != 0 &&
        strcmp(manifest->schema, "umicom.application/v1") != 0) {
        status = UMI_STATUS_PARSE_ERROR;
        message = "Unsupported application manifest schema";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (manifest->id[0] == '\0' || manifest->name[0] == '\0' ||
               manifest->version[0] == '\0' ||
               manifest->executable[0] == '\0') {
        status = UMI_STATUS_PARSE_ERROR;
        message = "Application identity fields are incomplete";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (manifest->frontends == UMI_FRONTEND_NONE) {
        status = UMI_STATUS_PARSE_ERROR;
        message = "At least one application frontend is required";
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}

/*
 * Provide the application manifest has capability operation used by this module and its
 * client applications.
 */
int umi_application_manifest_has_capability(
    const UmiApplicationManifest *manifest,
    const char *capability)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || capability == NULL) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->capability_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(manifest->capabilities[index], capability) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Provide the application frontend text operation used by this module and its client
 * applications.
 */
const char *umi_application_frontend_text(unsigned frontend_flag)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (frontend_flag) {
        case UMI_FRONTEND_CONSOLE: return "console";
        case UMI_FRONTEND_GTK4: return "gtk4";
        case UMI_FRONTEND_WEB: return "web";
        case UMI_FRONTEND_QT: return "qt6";
        case UMI_FRONTEND_WT: return "wt";
        case UMI_FRONTEND_MOBILE: return "mobile";
        default: return "unknown";
    }
}
