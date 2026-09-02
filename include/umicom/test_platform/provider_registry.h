/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/provider_registry.h
 *
 * PURPOSE:
 *   Describe and select provider-neutral test discovery and execution adapters.
 *   CTest, native C runners, Python, Java and future external providers can be
 *   represented without exposing their private process or protocol types.
 *
 * OWNERSHIP:
 *   Registries copy every descriptor by value. Returned descriptors are value
 *   snapshots; callers never receive pointers to mutable registry storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_PROVIDER_REGISTRY_H
#define UMICOM_TEST_PLATFORM_PROVIDER_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_PROVIDER_API_VERSION 1U
#define UMI_TEST_PLATFORM_PROVIDER_CAPACITY 64U
#define UMI_TEST_PLATFORM_PROVIDER_ID_CAPACITY 128U
#define UMI_TEST_PLATFORM_PROVIDER_LABEL_CAPACITY 256U
#define UMI_TEST_PLATFORM_PROVIDER_FRAMEWORK_CAPACITY 512U
#define UMI_TEST_PLATFORM_PROVIDER_PATH_CAPACITY 1024U
#define UMI_TEST_PLATFORM_PROVIDER_ARGUMENT_CAPACITY 1024U

/**
 * List the named test platform provider kind values accepted by this public contract.
 */
typedef enum UmiTestPlatformProviderKind {
    UMI_TEST_PLATFORM_PROVIDER_UNKNOWN = 0,
    UMI_TEST_PLATFORM_PROVIDER_CTEST = 1,
    UMI_TEST_PLATFORM_PROVIDER_NATIVE = 2,
    UMI_TEST_PLATFORM_PROVIDER_PYTHON = 3,
    UMI_TEST_PLATFORM_PROVIDER_JAVA = 4,
    UMI_TEST_PLATFORM_PROVIDER_JAVASCRIPT = 5,
    UMI_TEST_PLATFORM_PROVIDER_EXTERNAL = 6
} UmiTestPlatformProviderKind;

/**
 * List the named test platform provider capability values accepted by this public
 * contract.
 */
typedef enum UmiTestPlatformProviderCapability {
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_NONE = 0U,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DISCOVERY = 1U << 0,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_INCREMENTAL_DISCOVERY = 1U << 1,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_EXECUTION = 1U << 2,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DEBUG = 1U << 3,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_COVERAGE = 1U << 4,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_BENCHMARK = 1U << 5,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_ATTACHMENTS = 1U << 6,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_STRUCTURED_REPORTS = 1U << 7,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_CONTINUOUS_TESTING = 1U << 8,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_SHARDING = 1U << 9,
    UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_RETRY = 1U << 10
} UmiTestPlatformProviderCapability;

/**
 * Represent the test platform provider descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_TEST_PLATFORM_PROVIDER_ID_CAPACITY];
    char label[UMI_TEST_PLATFORM_PROVIDER_LABEL_CAPACITY];
    char frameworks[UMI_TEST_PLATFORM_PROVIDER_FRAMEWORK_CAPACITY];
    char executable[UMI_TEST_PLATFORM_PROVIDER_PATH_CAPACITY];
    char default_arguments[UMI_TEST_PLATFORM_PROVIDER_ARGUMENT_CAPACITY];
    char working_directory[UMI_TEST_PLATFORM_PROVIDER_PATH_CAPACITY];
    UmiTestPlatformProviderKind kind;
    uint32_t capabilities;
    int32_t priority;
    int enabled;
    int trusted;
    int available;
    int built_in;
    uint64_t revision;
} UmiTestPlatformProviderDescriptor;

/**
 * Represent the test platform provider query data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformProviderQuery {
    uint32_t struct_size;
    uint32_t api_version;
    const char *provider_id;
    const char *framework;
    uint32_t required_capabilities;
    UmiTestPlatformProviderKind kind;
    int require_trusted;
    int include_disabled;
    int include_unavailable;
} UmiTestPlatformProviderQuery;

/**
 * Represent the test platform provider selection data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformProviderSelection {
    uint32_t struct_size;
    uint32_t api_version;
    UmiTestPlatformProviderDescriptor provider;
    uint32_t matched_capabilities;
    int32_t score;
    uint64_t registry_revision;
    int found;
} UmiTestPlatformProviderSelection;

/**
 * Represent the test platform provider registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformProviderRegistry UmiTestPlatformProviderRegistry;

/**
 * Initialise test platform provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_provider_registry_create(
    UmiTestPlatformProviderRegistry **out_registry);
/**
 * Release or reset state held by test platform provider registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_provider_registry_destroy(
    UmiTestPlatformProviderRegistry *registry);
/**
 * Provide the test platform provider registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_provider_registry_upsert(
    UmiTestPlatformProviderRegistry *registry,
    const UmiTestPlatformProviderDescriptor *descriptor);
/**
 * Remove test platform provider registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_provider_registry_remove(
    UmiTestPlatformProviderRegistry *registry,
    const char *provider_id);
/**
 * Find test platform provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_provider_registry_find(
    const UmiTestPlatformProviderRegistry *registry,
    const char *provider_id,
    UmiTestPlatformProviderDescriptor *out_descriptor);
/**
 * Find test platform provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_provider_registry_at(
    const UmiTestPlatformProviderRegistry *registry,
    size_t position,
    UmiTestPlatformProviderDescriptor *out_descriptor);
/**
 * Provide the test platform provider registry select operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_provider_registry_select(
    const UmiTestPlatformProviderRegistry *registry,
    const UmiTestPlatformProviderQuery *query,
    UmiTestPlatformProviderSelection *out_selection);
/**
 * Initialise test platform provider query from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_platform_provider_query_init(
    UmiTestPlatformProviderQuery *query);
/**
 * Return the number of records represented by test platform provider registry without
 * changing their state.
 */
size_t umi_test_platform_provider_registry_count(
    const UmiTestPlatformProviderRegistry *registry);
/**
 * Provide the test platform provider registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_provider_registry_revision(
    const UmiTestPlatformProviderRegistry *registry);
/**
 * Provide the test platform provider kind text operation used by this module and its
 * client applications.
 */
const char *umi_test_platform_provider_kind_text(
    UmiTestPlatformProviderKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_PROVIDER_REGISTRY_H */
