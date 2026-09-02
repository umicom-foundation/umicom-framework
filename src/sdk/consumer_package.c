/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/consumer_package.c
 *
 * PURPOSE:
 *   Describe and render the CMake package contract used by SDK consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/sdk/consumer_package.h"
#include "sdk_internal.h"

/*
 * Initialise sdk consumer package from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_sdk_consumer_package_init(UmiSdkConsumerPackage *package,
                                            const char *package_name,
                                            const char *namespace_name,
                                            const char *minimum_version,
                                            const char *config_directory)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || !umi_sdk_text(package_name) ||
        !umi_sdk_text(namespace_name) || !umi_sdk_text(minimum_version) ||
        !umi_sdk_text(config_directory)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(package, 0, sizeof(*package));
    status = umi_sdk_copy(package->package_name,
                             sizeof(package->package_name), package_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_copy(package->namespace_name,
                             sizeof(package->namespace_name), namespace_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_copy(package->minimum_version,
                             sizeof(package->minimum_version), minimum_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_copy(package->config_directory,
                             sizeof(package->config_directory), config_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    package->include_examples = 1;
    package->include_toolchain_file = 1;
    return UMI_STATUS_OK;
}

/*
 * Check that sdk consumer package satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_consumer_package_validate(
    const UmiSdkConsumerPackage *package)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_sdk_text(package->package_name) ||
        !umi_sdk_text(package->namespace_name) ||
        !umi_sdk_text(package->minimum_version) ||
        !umi_sdk_text(package->config_directory)) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the sdk consumer package render cmake operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_consumer_package_render_cmake(
    const UmiSdkConsumerPackage *package,
    const UmiSdkExportCatalogue *catalogue,
    char *output,
    size_t output_capacity)
{
    size_t used;
    size_t index;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || output_capacity == 0U ||
        umi_sdk_consumer_package_validate(package) != UMI_STATUS_OK ||
        !umi_sdk_export_catalogue_ready(catalogue)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(output, output_capacity,
                       "find_package(%s %s CONFIG REQUIRED)\n"
                       "target_link_libraries(${PROJECT_NAME} PRIVATE",
                       package->package_name, package->minimum_version);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= output_capacity) {
        output[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        written = snprintf(output + used, output_capacity - used, " %s",
                           catalogue->targets[index].target_name);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= output_capacity - used) {
            output[0] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    written = snprintf(output + used, output_capacity - used, ")\n");
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= output_capacity - used) {
        output[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
