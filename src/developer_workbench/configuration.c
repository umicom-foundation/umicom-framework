/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/configuration.c
 *
 * PURPOSE:
 *   Implement reusable developer-workbench lifecycle configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/configuration.h"

#include <stdio.h>
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static void write_message(char *out_message,
                          size_t capacity,
                          const char *text)
{
    if (out_message == NULL || capacity == 0U) return;
    (void)snprintf(out_message, capacity, "%s", text != NULL ? text : "");
}

void umi_developer_workbench_configuration_init(
    UmiDeveloperWorkbenchConfiguration *configuration,
    const char *configuration_id,
    const char *title)
{
    if (configuration == NULL) return;

    (void)memset(configuration, 0, sizeof(*configuration));
    configuration->structure_size = (uint32_t)sizeof(*configuration);
    configuration->api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION;
    copy_text(configuration->configuration_id,
              sizeof(configuration->configuration_id),
              configuration_id);
    copy_text(configuration->title,
              sizeof(configuration->title),
              title);
    copy_text(configuration->configuration,
              sizeof(configuration->configuration),
              "Debug");
    copy_text(configuration->package_target,
              sizeof(configuration->package_target),
              "package");
    configuration->jobs = 2;
    configuration->revision = 1U;
}

UmiStatus umi_developer_workbench_configuration_validate(
    const UmiDeveloperWorkbenchConfiguration *configuration,
    char *out_message,
    size_t message_capacity)
{
    if (configuration == NULL ||
        configuration->structure_size != sizeof(*configuration) ||
        configuration->api_version != UMI_DEVELOPER_WORKBENCH_API_VERSION ||
        configuration->configuration_id[0] == '\0' ||
        configuration->title[0] == '\0' ||
        configuration->source_root[0] == '\0' ||
        configuration->jobs < 0) {
        write_message(out_message,
                      message_capacity,
                      "Developer configuration is incomplete.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (configuration->preset[0] == '\0' &&
        configuration->build_directory[0] == '\0') {
        write_message(out_message,
                      message_capacity,
                      "A preset or build directory is required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    write_message(out_message,
                  message_capacity,
                  "Developer configuration is valid.");
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_configuration_to_build_request(
    const UmiDeveloperWorkbenchConfiguration *configuration,
    UmiBuildRequest *out_request)
{
    UmiStatus status;

    if (configuration == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_configuration_validate(
        configuration, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;

    umi_build_request_init(out_request);
    out_request->source_root = configuration->source_root;
    out_request->build_directory =
        configuration->build_directory[0] != '\0'
            ? configuration->build_directory
            : NULL;
    out_request->preset =
        configuration->preset[0] != '\0'
            ? configuration->preset
            : NULL;
    out_request->executable =
        configuration->executable[0] != '\0'
            ? configuration->executable
            : NULL;
    out_request->configuration =
        configuration->configuration[0] != '\0'
            ? configuration->configuration
            : NULL;
    out_request->install_prefix =
        configuration->install_prefix[0] != '\0'
            ? configuration->install_prefix
            : NULL;
    out_request->package_target =
        configuration->package_target[0] != '\0'
            ? configuration->package_target
            : NULL;
    out_request->jobs = configuration->jobs;
    out_request->timeout_ms = configuration->timeout_ms;
    return UMI_STATUS_OK;
}
