/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/readiness.c
 *
 * PURPOSE:
 *   Implement lifecycle readiness without invoking an external process.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/readiness.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_developer_workbench_readiness_evaluate(
    const UmiDeveloperWorkbenchConfiguration *configuration,
    const UmiToolchainProfile *profile,
    UmiDeveloperWorkbenchReadiness *out_readiness)
{
    UmiStatus status;

    if (configuration == NULL || profile == NULL || out_readiness == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_readiness, 0, sizeof(*out_readiness));

    status = umi_developer_workbench_configuration_validate(
        configuration, out_readiness->reason, sizeof(out_readiness->reason));
    out_readiness->configuration_valid = status == UMI_STATUS_OK;

    out_readiness->toolchain_ready =
        profile->complete &&
        umi_toolchain_profile_c_compiler(profile) != NULL;

    if (!out_readiness->configuration_valid) {
        return UMI_STATUS_OK;
    }

    if (!out_readiness->toolchain_ready) {
        (void)snprintf(out_readiness->reason,
                       sizeof(out_readiness->reason),
                       "%s",
                       "Selected toolchain is incomplete.");
        return UMI_STATUS_OK;
    }

    out_readiness->can_configure = 1;
    out_readiness->can_build = 1;
    out_readiness->can_test = 1;
    out_readiness->can_install =
        configuration->build_directory[0] != '\0';
    out_readiness->can_package = 1;
    out_readiness->can_run =
        configuration->executable[0] != '\0';

    (void)snprintf(out_readiness->reason,
                   sizeof(out_readiness->reason),
                   "%s",
                   "Developer lifecycle is ready.");
    return UMI_STATUS_OK;
}
