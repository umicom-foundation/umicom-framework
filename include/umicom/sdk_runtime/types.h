/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for installed Framework SDK and runtime resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_TYPES_H
#define UMICOM_SDK_RUNTIME_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SDK_RUNTIME_ID_CAPACITY 160U
#define UMI_SDK_RUNTIME_PATH_CAPACITY 1024U
#define UMI_SDK_RUNTIME_TEXT_CAPACITY 512U
#define UMI_SDK_RUNTIME_MAX_COMPONENTS 256U
#define UMI_SDK_RUNTIME_MAX_DEPENDENCIES 256U
#define UMI_SDK_RUNTIME_MAX_SEARCH_PATHS 64U

/**
 * List the named sdk runtime state values accepted by this public contract.
 */
typedef enum UmiSdkRuntimeState {
    UMI_SDK_RUNTIME_STATE_UNKNOWN = 0,
    UMI_SDK_RUNTIME_STATE_DISCOVERED = 1,
    UMI_SDK_RUNTIME_STATE_VALIDATED = 2,
    UMI_SDK_RUNTIME_STATE_READY = 3,
    UMI_SDK_RUNTIME_STATE_DEGRADED = 4,
    UMI_SDK_RUNTIME_STATE_INCOMPATIBLE = 5,
    UMI_SDK_RUNTIME_STATE_MISSING = 6
} UmiSdkRuntimeState;

/**
 * List the named sdk runtime component kind values accepted by this public contract.
 */
typedef enum UmiSdkRuntimeComponentKind {
    UMI_SDK_RUNTIME_COMPONENT_STATIC_LIBRARY = 1,
    UMI_SDK_RUNTIME_COMPONENT_SHARED_LIBRARY = 2,
    UMI_SDK_RUNTIME_COMPONENT_EXECUTABLE = 3,
    UMI_SDK_RUNTIME_COMPONENT_HEADER = 4,
    UMI_SDK_RUNTIME_COMPONENT_RESOURCE = 5,
    UMI_SDK_RUNTIME_COMPONENT_CMAKE_PACKAGE = 6,
    UMI_SDK_RUNTIME_COMPONENT_ADAPTER = 7,
    UMI_SDK_RUNTIME_COMPONENT_PLUGIN = 8
} UmiSdkRuntimeComponentKind;

/**
 * Provide the sdk runtime state text operation used by this module and its client
 * applications.
 */
const char *umi_sdk_runtime_state_text(UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime component kind text operation used by this module and its client
 * applications.
 */
const char *umi_sdk_runtime_component_kind_text(UmiSdkRuntimeComponentKind kind);
/**
 * Provide the sdk runtime copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_copy_text(char *destination, size_t capacity, const char *source);
/**
 * Check that sdk runtime text satisfies its contract before another service relies on it.
 */
bool umi_sdk_runtime_text_is_valid(const char *text, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
