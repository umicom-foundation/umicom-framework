/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/binary_manifest.h
 *
 * PURPOSE:
 *   Describe one executable or shared-library binary and architecture evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_BINARY_MANIFEST
#define UMICOM_SDK_RUNTIME_BINARY_MANIFEST
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime binary manifest data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeBinaryManifest {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t file_size;
    uint64_t checksum;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeBinaryManifest;
/**
 * Initialise sdk runtime binary manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_binary_manifest_init(UmiSdkRuntimeBinaryManifest *value, const char *id);
/**
 * Check that sdk runtime binary manifest satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_binary_manifest_validate(const UmiSdkRuntimeBinaryManifest *value);
/**
 * Provide the sdk runtime binary manifest set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_binary_manifest_set_path(UmiSdkRuntimeBinaryManifest *value, const char *path);
/**
 * Provide the sdk runtime binary manifest set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_binary_manifest_set_detail(UmiSdkRuntimeBinaryManifest *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime binary manifest set file without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_binary_manifest_set_file_size(UmiSdkRuntimeBinaryManifest *value, uint64_t number);
/**
 * Provide the sdk runtime binary manifest set checksum operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_binary_manifest_set_checksum(UmiSdkRuntimeBinaryManifest *value, uint64_t number);
/**
 * Provide the sdk runtime binary manifest set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_binary_manifest_set_state(UmiSdkRuntimeBinaryManifest *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime binary manifest same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_binary_manifest_same_identity(const UmiSdkRuntimeBinaryManifest *left, const UmiSdkRuntimeBinaryManifest *right);
#ifdef __cplusplus
}
#endif
#endif
