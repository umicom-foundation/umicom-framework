/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/export_plan.c
 *
 * PURPOSE:
 *   Aggregate installed SDK paths, exported targets and consumer examples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/sdk/export_plan.h"
#include "sdk_internal.h"

static UmiStatus umi_sdk_path(char *output,
                                  size_t output_capacity,
                                  const char *prefix,
                                  const char *suffix)
{
    int written = snprintf(output, output_capacity, "%s/%s", prefix, suffix);
    if (written < 0 || (size_t)written >= output_capacity) {
        output[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_sdk_export_plan_init(UmiSdkExportPlan *plan,
                                       const char *install_prefix,
                                       const char *minimum_version)
{
    UmiStatus status;
    if (plan == NULL || !umi_sdk_text(install_prefix) ||
        !umi_sdk_text(minimum_version)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_sdk_copy(plan->install_prefix,
                             sizeof(plan->install_prefix), install_prefix);
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_path(plan->include_directory,
                             sizeof(plan->include_directory), install_prefix,
                             "include");
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_path(plan->library_directory,
                             sizeof(plan->library_directory), install_prefix,
                             "lib");
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_path(plan->examples_directory,
                             sizeof(plan->examples_directory), install_prefix,
                             "share/umicom/examples");
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_consumer_package_init(
        &plan->package, "UmicomFramework", "Umicom", minimum_version,
        "lib/cmake/UmicomFramework");
    if (status != UMI_STATUS_OK) return status;
    umi_sdk_export_catalogue_init(&plan->catalogue);
    return UMI_STATUS_OK;
}

UmiStatus umi_sdk_export_plan_add_target(UmiSdkExportPlan *plan,
                                             const char *component_id,
                                             const char *target_name,
                                             int required)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_sdk_export_catalogue_add(&plan->catalogue, component_id,
                                            target_name, required);
}

UmiStatus umi_sdk_export_plan_validate(const UmiSdkExportPlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_text(plan->install_prefix) ||
        !umi_sdk_text(plan->include_directory) ||
        !umi_sdk_text(plan->library_directory) ||
        !umi_sdk_text(plan->examples_directory) ||
        umi_sdk_consumer_package_validate(&plan->package) != UMI_STATUS_OK ||
        !umi_sdk_export_catalogue_ready(&plan->catalogue)) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
