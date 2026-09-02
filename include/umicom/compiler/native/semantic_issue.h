/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/semantic_issue.h
 *
 * PURPOSE:
 *   Capture compiler semantic diagnostics with stable codes, severity, spans and fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SEMANTIC_ISSUE_H
#define UMICOM_COMPILER_NATIVE_SEMANTIC_ISSUE_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native semantic severity values accepted by this public contract.
 */
typedef enum UmiNativeSemanticSeverity { UMI_NC_SEM_NOTE=0, UMI_NC_SEM_WARNING=1, UMI_NC_SEM_ERROR=2, UMI_NC_SEM_FATAL=3 } UmiNativeSemanticSeverity;
/**
 * Represent the native semantic issue data shared with callers of this public contract.
 */
typedef struct UmiNativeSemanticIssue { UmiNativeSemanticSeverity severity; uint32_t code; UmiNativeSourceSpan span; char message[UMI_NC_TEXT_CAPACITY]; uint64_t fingerprint; } UmiNativeSemanticIssue;
/**
 * Initialise nc semantic issue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_semantic_issue_init(UmiNativeSemanticIssue *issue,UmiNativeSemanticSeverity severity,uint32_t code,UmiNativeSourceSpan span,const char *message);
/**
 * Provide the nc semantic issue blocks codegen operation used by this module and its
 * client applications.
 */
bool umi_nc_semantic_issue_blocks_codegen(const UmiNativeSemanticIssue *issue);
#ifdef __cplusplus
}
#endif
#endif
