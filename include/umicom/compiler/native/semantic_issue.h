/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/semantic_issue.h
 *
 * PURPOSE:
 *   Capture compiler semantic diagnostics with stable codes, severity, spans and fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SEMANTIC_ISSUE_H
#define UMICOM_COMPILER_NATIVE_SEMANTIC_ISSUE_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiNativeSemanticSeverity { UMI_NC_SEM_NOTE=0, UMI_NC_SEM_WARNING=1, UMI_NC_SEM_ERROR=2, UMI_NC_SEM_FATAL=3 } UmiNativeSemanticSeverity;
typedef struct UmiNativeSemanticIssue { UmiNativeSemanticSeverity severity; uint32_t code; UmiNativeSourceSpan span; char message[UMI_NC_TEXT_CAPACITY]; uint64_t fingerprint; } UmiNativeSemanticIssue;
UmiStatus umi_nc_semantic_issue_init(UmiNativeSemanticIssue *issue,UmiNativeSemanticSeverity severity,uint32_t code,UmiNativeSourceSpan span,const char *message);
bool umi_nc_semantic_issue_blocks_codegen(const UmiNativeSemanticIssue *issue);
#ifdef __cplusplus
}
#endif
#endif
