/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/dependency.c
 *
 * PURPOSE:
 *   Query the validated pkg-config executable for required Framework and
 *   application packages without depending on shell-specific path setup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/dependency.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/process.h"

UmiStatus umi_dependency_check(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiDependencyRequirement *requirements,
    size_t requirement_count,
    UmiDependencyReport *out_report)
{
    const UmiToolInfo *pkg_config;
    size_t index;

    if (profile == NULL || requirements == NULL || out_report == NULL ||
        requirement_count > UMI_DEPENDENCY_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    pkg_config = umi_toolchain_profile_tool(profile, UMI_TOOL_PKG_CONFIG);
    if (pkg_config == NULL || pkg_config->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }

    for (index = 0U; index < requirement_count; ++index) {
        const char *exists_arguments[2];
        const char *version_arguments[2];
        const char *minimum_arguments[2];
        char minimum_option[UMI_DEPENDENCY_TEXT_CAPACITY];
        UmiProcessRequest request;
        UmiProcessResult result;
        UmiDependencyStatus *item = &out_report->items[out_report->count++];

        (void)snprintf(item->package_name,
                       sizeof(item->package_name),
                       "%s",
                       requirements[index].package_name);
        item->required = requirements[index].required;

        exists_arguments[0] = "--exists";
        exists_arguments[1] = requirements[index].package_name;
        (void)memset(&request, 0, sizeof(request));
        request.program = pkg_config->path;
        request.arguments = exists_arguments;
        request.argument_count = 2U;
        request.environment = environment != NULL
            ? umi_environment_plan_variables(environment)
            : NULL;
        request.environment_count = environment != NULL ? environment->count : 0U;
        request.capture_stdout = 1;
        request.capture_stderr = 1;
        (void)memset(&result, 0, sizeof(result));
        result.exit_code = -1;
        (void)umi_process_execute(&request, &result);
        item->available = result.exit_code == 0;

        if (item->available) {
            version_arguments[0] = "--modversion";
            version_arguments[1] = requirements[index].package_name;
            request.arguments = version_arguments;
            (void)memset(&result, 0, sizeof(result));
            result.exit_code = -1;
            (void)umi_process_execute(&request, &result);
            {
                size_t version_length = strcspn(result.output, "\r\n");
                if (version_length >= sizeof(item->version)) {
                    version_length = sizeof(item->version) - 1U;
                }
                (void)memcpy(item->version, result.output, version_length);
                item->version[version_length] = '\0';
            }
            if (requirements[index].minimum_version != NULL &&
                requirements[index].minimum_version[0] != '\0') {
                (void)snprintf(
                    minimum_option,
                    sizeof(minimum_option),
                    "--atleast-version=%s",
                    requirements[index].minimum_version);
                minimum_arguments[0] = minimum_option;
                minimum_arguments[1] = requirements[index].package_name;
                request.arguments = minimum_arguments;
                (void)memset(&result, 0, sizeof(result));
                result.exit_code = -1;
                (void)umi_process_execute(&request, &result);
                item->available = result.exit_code == 0;
            }
        }
        if (!item->available && item->required) {
            out_report->required_missing += 1U;
        }
    }
    return out_report->required_missing == 0U
        ? UMI_STATUS_OK
        : UMI_STATUS_NOT_FOUND;
}
