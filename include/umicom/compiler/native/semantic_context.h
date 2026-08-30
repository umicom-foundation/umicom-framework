/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/semantic_context.h
 *
 * PURPOSE:
 *   Aggregate semantic diagnostics and expose a single code-generation readiness decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SEMANTIC_CONTEXT_H
#define UMICOM_COMPILER_NATIVE_SEMANTIC_CONTEXT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/semantic_issue.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_SEMANTIC_ISSUES 128U
typedef struct UmiNativeSemanticContext { UmiNativeSemanticIssue issues[UMI_NC_MAX_SEMANTIC_ISSUES]; size_t count; size_t warnings; size_t errors; } UmiNativeSemanticContext;
void umi_nc_semantic_context_init(UmiNativeSemanticContext *context);
UmiStatus umi_nc_semantic_context_report(UmiNativeSemanticContext *context,const UmiNativeSemanticIssue *issue);
bool umi_nc_semantic_context_can_codegen(const UmiNativeSemanticContext *context);
#ifdef __cplusplus
}
#endif
#endif
