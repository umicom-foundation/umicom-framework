/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/suite.c
 *
 * PURPOSE:
 *   Implement deterministic application-suite composition and write a portable
 *   suite manifest consumed by native tools and future Studio suite views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/suite.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

void umi_suite_init(UmiSuite *suite, const char *id, const char *name)
{
    if (suite == NULL) {
        return;
    }
    (void)memset(suite, 0, sizeof(*suite));
    if (id != NULL) {
        (void)snprintf(suite->id, sizeof(suite->id), "%s", id);
    }
    if (name != NULL) {
        (void)snprintf(suite->name, sizeof(suite->name), "%s", name);
    }
}

UmiStatus umi_suite_add(UmiSuite *suite,
                        const UmiSuiteApplication *application)
{
    size_t index;

    if (suite == NULL || application == NULL ||
        application->id[0] == '\0' || application->path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < suite->application_count; ++index) {
        if (strcmp(suite->applications[index].id, application->id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (suite->application_count >= UMI_SUITE_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    suite->applications[suite->application_count++] = *application;
    return UMI_STATUS_OK;
}

const UmiSuiteApplication *umi_suite_find(const UmiSuite *suite,
                                          const char *application_id)
{
    size_t index;
    if (suite == NULL || application_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < suite->application_count; ++index) {
        if (strcmp(suite->applications[index].id, application_id) == 0) {
            return &suite->applications[index];
        }
    }
    return NULL;
}

UmiStatus umi_suite_validate(const UmiSuite *suite,
                             char *out_message,
                             size_t capacity)
{
    const char *message = "Suite is valid";
    UmiStatus status = UMI_STATUS_OK;
    size_t index;

    if (suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (suite->id[0] == '\0' || suite->name[0] == '\0') {
        status = UMI_STATUS_PARSE_ERROR;
        message = "Suite identity is incomplete";
    } else if (suite->application_count == 0U) {
        status = UMI_STATUS_PARSE_ERROR;
        message = "Suite contains no applications";
    } else {
        for (index = 0U; index < suite->application_count; ++index) {
            const UmiSuiteApplication *application = &suite->applications[index];
            if (application->id[0] == '\0' ||
                application->path[0] == '\0') {
                status = UMI_STATUS_PARSE_ERROR;
                message = "Suite application is incomplete";
                break;
            }
        }
    }

    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}

UmiStatus umi_suite_write_manifest(const UmiSuite *suite,
                                   const char *path)
{
    char text[32768];
    size_t used = 0U;
    size_t index;
    int written;
    UmiStatus status;

    if (suite == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_suite_validate(suite, NULL, 0U);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    written = snprintf(text,
                       sizeof(text),
                       "schema: umicom.suite.v1\n\nsuite:\n"
                       "  id: %s\n  name: %s\n\napplications:\n",
                       suite->id,
                       suite->name);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    for (index = 0U; index < suite->application_count; ++index) {
        const UmiSuiteApplication *application = &suite->applications[index];
        written = snprintf(text + used,
                           sizeof(text) - used,
                           "  - id: %s\n    name: %s\n    path: %s\n"
                           "    executable: %s\n    enabled: %s\n",
                           application->id,
                           application->name,
                           application->path,
                           application->executable,
                           application->enabled ? "true" : "false");
        if (written < 0 || (size_t)written >= sizeof(text) - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }

    return umi_fs_write_text(path, text);
}
