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
 * Read application manifest into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_application_manifest_load(
    const char *path,
    UmiApplicationManifest *out_manifest)
{
    char *text = NULL;
    char *line;
    char *save_pointer = NULL;
    enum { SECTION_NONE, SECTION_APPLICATION, SECTION_FRAMEWORK,
           SECTION_FRONTENDS, SECTION_CAPABILITIES } section = SECTION_NONE;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_application_manifest_init(out_manifest);
    status = umi_fs_read_text(path, &text, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    line = strtok_r(text, "\n", &save_pointer);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (line != NULL) {
        char *trimmed = umi_manifest_trim(line);
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

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strncmp(trimmed, "schema:", 7U) == 0) {
            (void)umi_manifest_copy(out_manifest->schema,
                                    sizeof(out_manifest->schema),
                                    umi_manifest_trim(trimmed + 7));
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "application:") == 0) {
            section = SECTION_APPLICATION;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "framework:") == 0) {
            section = SECTION_FRAMEWORK;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "frontends:") == 0) {
            section = SECTION_FRONTENDS;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(trimmed, "capabilities:") == 0) {
            section = SECTION_CAPABILITIES;
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
            if (separator != NULL) {
                char *key;
                char *value;
                *separator = '\0';
                key = umi_manifest_trim(trimmed);
                value = umi_manifest_trim(separator + 1);
                /* Apply this branch only when its contract condition is satisfied. */
                if (section == SECTION_APPLICATION) {
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
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "executable") == 0)
                        (void)umi_manifest_copy(out_manifest->executable,
                                                sizeof(out_manifest->executable), value);
                    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "aliases") == 0)
                        umi_manifest_parse_inline_aliases(out_manifest, value);
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

    umi_fs_free_text(text);
    return umi_application_manifest_validate(out_manifest, NULL, 0U);
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
    if (strcmp(manifest->schema, "umicom.application.v1") != 0) {
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
