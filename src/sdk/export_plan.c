/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/export_plan.c
 *
 * PURPOSE:
 *   Aggregate installed SDK paths, exported targets and consumer examples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/sdk/export_plan.h"
#include "sdk_internal.h"

/* Provide the sdk path operation used by this module and its client applications. */
static UmiStatus umi_sdk_path(char *output,
                                  size_t output_capacity,
                                  const char *prefix,
                                  const char *suffix)
{
    int written = snprintf(output, output_capacity, "%s/%s", prefix, suffix);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= output_capacity) {
        output[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise sdk export plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_sdk_export_plan_init(UmiSdkExportPlan *plan,
                                       const char *install_prefix,
                                       const char *minimum_version)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || !umi_sdk_text(install_prefix) ||
        !umi_sdk_text(minimum_version)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_sdk_copy(plan->install_prefix,
                             sizeof(plan->install_prefix), install_prefix);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_path(plan->include_directory,
                             sizeof(plan->include_directory), install_prefix,
                             "include");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_path(plan->library_directory,
                             sizeof(plan->library_directory), install_prefix,
                             "lib");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_path(plan->examples_directory,
                             sizeof(plan->examples_directory), install_prefix,
                             "share/umicom/examples");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_consumer_package_init(
        &plan->package, "UmicomFramework", "Umicom", minimum_version,
        "lib/cmake/UmicomFramework");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_sdk_export_catalogue_init(&plan->catalogue);
    return UMI_STATUS_OK;
}

/*
 * Provide the sdk export plan add target operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_export_plan_add_target(UmiSdkExportPlan *plan,
                                             const char *component_id,
                                             const char *target_name,
                                             int required)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_sdk_export_catalogue_add(&plan->catalogue, component_id,
                                            target_name, required);
}

/* Check that sdk export plan satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_export_plan_validate(const UmiSdkExportPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
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
