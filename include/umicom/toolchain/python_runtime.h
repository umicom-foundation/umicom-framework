/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/python_runtime.h
 *
 * PURPOSE:
 *   Discover CPython as a reusable Framework development/runtime dependency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_PYTHON_RUNTIME_H
#define UMICOM_TOOLCHAIN_PYTHON_RUNTIME_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PYTHON_RUNTIME_API_VERSION 1U
#define UMI_PYTHON_RUNTIME_VERSION_CAPACITY 256U

/**
 * Represent the python runtime request data shared with callers of this public contract.
 */
typedef struct UmiPythonRuntimeRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *explicit_root;
    const char *preferred_executable;
    int validate_version;
} UmiPythonRuntimeRequest;

/**
 * Represent the python runtime snapshot data shared with callers of this public contract.
 */
typedef struct UmiPythonRuntimeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char executable[UMI_TOOL_PATH_CAPACITY];
    char home[UMI_TOOL_PATH_CAPACITY];
    char version[UMI_PYTHON_RUNTIME_VERSION_CAPACITY];
    int available;
    int validated;
} UmiPythonRuntimeSnapshot;

/**
 * Initialise python runtime request from caller-provided values so later operations
 * receive a known state.
 */
void umi_python_runtime_request_init(UmiPythonRuntimeRequest *request);
/**
 * Provide the python runtime discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_python_runtime_discover(const UmiPythonRuntimeRequest *request,
                                      UmiPythonRuntimeSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
