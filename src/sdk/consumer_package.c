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
#include <stdint.h>
#include "../base/value_construction_internal.h"

/*
 * Initialise sdk consumer package from caller-provided values so later operations receive
 * a known state.
 */
/* The previous constructor cleared the live value before all copies succeeded and could erase borrowed field inputs. The shared staged-value wrapper now publishes a complete replacement only on success; the original defaults and implementation remain here for review. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus umi_sdk_consumer_package_build_value(UmiSdkConsumerPackage *package,
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


/* The shared staged constructor owns the temporary inline value. It publishes
 * only after every copy succeeds, so input strings may borrow old fields. */
UMI_DEFINE_STAGED_VALUE_INIT(umi_sdk_consumer_package_init, UmiSdkConsumerPackage, package,
    (UmiSdkConsumerPackage *package, const char *package_name, const char *namespace_name,
     const char *minimum_version, const char *config_directory),
    umi_sdk_consumer_package_build_value(candidate, package_name, namespace_name,
        minimum_version, config_directory))

/*
 * Check that sdk consumer package satisfies its contract before another service relies on
 * it.
 */
/* The previous first-byte checks did not establish NUL termination within fixed fields. Bounded validation now prevents later readers from leaving those fields. Preserve the earlier validation for review; this structural check does not validate generated CMake syntax. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus umi_sdk_consumer_package_validate(const UmiSdkConsumerPackage *package)
{
    if (package == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_bounded_text(package->package_name, sizeof package->package_name) ||
        !umi_sdk_bounded_text(package->namespace_name, sizeof package->namespace_name) ||
        !umi_sdk_bounded_text(package->minimum_version, sizeof package->minimum_version) ||
        !umi_sdk_bounded_text(package->config_directory, sizeof package->config_directory))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the sdk consumer package render cmake operation used by this module and its
 * client applications.
 */
/* The previous renderer inserted unrestricted field text into CMake commands and cleared live output on capacity failure. Literal-argument validation and complete staged output now preserve accepted ordinary formatting while refusing ambiguous syntax. Retain the earlier renderer for review. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Generated commands accept literal names, not caller-provided CMake code.
 * Keep the grammar explicit and locale-independent so list expansion, variable
 * references, generator expressions and command delimiters never enter fields. */
static int umi_sdk_cmake_name(const char *text, int allow_namespace)
{
    size_t index = 0U;
    int segment_start = 1;
    for (; text[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)text[index];
        int identifier = (value >= 'A' && value <= 'Z') ||
            (value >= 'a' && value <= 'z') || (value >= '0' && value <= '9') || value == '_';
        if (identifier) { segment_start = 0; continue; }
        if (!segment_start && (value == '.' || value == '+' || value == '-')) continue;
        if (allow_namespace && !segment_start && value == ':' && text[index + 1U] == ':') {
            ++index;
            segment_start = 1;
            continue;
        }
        return 0;
    }
    return index != 0U && !segment_start;
}

static int umi_sdk_cmake_version(const char *text)
{
    unsigned parts = 1U;
    int needs_digit = 1;
    for (size_t index = 0U; text[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)text[index];
        if (value >= '0' && value <= '9') { needs_digit = 0; continue; }
        if (value != '.' || needs_digit || parts == 4U) return 0;
        ++parts;
        needs_digit = 1;
    }
    return !needs_digit;
}

static int umi_sdk_cmake_link_keyword(const char *text)
{
    static const char *const keywords[] = {
        "PRIVATE", "PUBLIC", "INTERFACE", "LINK_PRIVATE", "LINK_PUBLIC",
        "LINK_INTERFACE_LIBRARIES", "debug", "optimized", "general"
    };
    for (size_t index = 0U; index < sizeof keywords / sizeof keywords[0]; ++index)
        if (strcmp(text, keywords[index]) == 0) return 1;
    return 0;
}

UmiStatus umi_sdk_consumer_package_cmake_size(const UmiSdkConsumerPackage *package,
    const UmiSdkExportCatalogue *catalogue, size_t *out_bytes)
{
    if (out_bytes == NULL || umi_sdk_consumer_package_validate(package) != UMI_STATUS_OK ||
        !umi_sdk_export_catalogue_ready(catalogue))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_cmake_name(package->package_name, 0) ||
        !umi_sdk_cmake_version(package->minimum_version))
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The model limits each field and the catalogue length. Still check sums so
     * later capacity changes cannot silently turn sizing into wrapped arithmetic. */
    size_t bytes = sizeof("find_package(  CONFIG REQUIRED)\n"
        "target_link_libraries(${PROJECT_NAME} PRIVATE)\n");
    const size_t package_bytes = strlen(package->package_name);
    const size_t version_bytes = strlen(package->minimum_version);
    if (package_bytes > SIZE_MAX - bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes += package_bytes;
    if (version_bytes > SIZE_MAX - bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes += version_bytes;
    for (size_t index = 0U; index < catalogue->count; ++index) {
        const char *name = catalogue->targets[index].target_name;
        if (!umi_sdk_cmake_name(name, 1) || umi_sdk_cmake_link_keyword(name))
            return UMI_STATUS_INVALID_ARGUMENT;
        const size_t length = strlen(name);
        if (length == SIZE_MAX || length + 1U > SIZE_MAX - bytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        bytes += length + 1U;
    }
    *out_bytes = bytes;
    return UMI_STATUS_OK;
}

static UmiStatus umi_sdk_consumer_package_render_value(
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

/* Format into owned temporary storage. A failed render cannot erase the last
 * reviewed output or expose a partially formed build command to its caller. */
UmiStatus umi_sdk_consumer_package_render_cmake(const UmiSdkConsumerPackage *package,
    const UmiSdkExportCatalogue *catalogue, char *output, size_t output_capacity)
{
    if (output == NULL || output_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t required = 0U;
    UmiStatus status = umi_sdk_consumer_package_cmake_size(package, catalogue, &required);
    if (status != UMI_STATUS_OK) return status;
    if (required > output_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *candidate = malloc(required);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_sdk_consumer_package_render_value(package, catalogue, candidate, required);
    if (status == UMI_STATUS_OK) memcpy(output, candidate, required);
    free(candidate);
    return status;
}
