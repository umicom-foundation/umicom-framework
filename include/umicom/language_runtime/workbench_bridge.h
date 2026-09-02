/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/workbench_bridge.h
 *
 * PURPOSE:
 *   Bind real Definition, References and Workspace Symbol requests to Developer Workbench commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_WORKBENCH_BRIDGE_H
#define UMICOM_LANGUAGE_RUNTIME_WORKBENCH_BRIDGE_H
#include "umicom/developer_workbench/workbench.h"
#include "umicom/language_runtime/platform.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime workbench bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeWorkbenchBridge UmiLanguageRuntimeWorkbenchBridge;
/**
 * Represent the language runtime workbench context data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeWorkbenchContext{char document_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY],symbol_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY],language_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY],root_uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];uint32_t line,column;}UmiLanguageRuntimeWorkbenchContext;
/**
 * Initialise language runtime workbench bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_workbench_bridge_create(UmiDeveloperWorkbench*w,UmiLanguageRuntimePlatform*p,UmiLanguageRuntimeWorkbenchBridge**out);
/**
 * Release or reset state held by language runtime workbench bridge so the same storage can
 * be reused safely.
 */
void umi_language_runtime_workbench_bridge_destroy(UmiLanguageRuntimeWorkbenchBridge*b);
/**
 * Provide the language runtime workbench bridge set context operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_workbench_bridge_set_context(UmiLanguageRuntimeWorkbenchBridge*b,const UmiLanguageRuntimeWorkbenchContext*c);
/**
 * Provide the language runtime workbench bridge bind operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_workbench_bridge_bind(UmiLanguageRuntimeWorkbenchBridge*b);
#ifdef __cplusplus
}
#endif
#endif
