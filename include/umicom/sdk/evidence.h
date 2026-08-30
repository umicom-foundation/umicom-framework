/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/evidence.h
 *
 * PURPOSE:
 *   Collect named pass/fail evidence produced by SDK validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_EVIDENCE_H
#define UMICOM_SDK_EVIDENCE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkEvidence { const char *check_id; int passed; const char *detail; } UmiSdkEvidence;
UmiStatus umi_sdk_evidence_validate(const UmiSdkEvidence *evidence);
#ifdef __cplusplus
}
#endif
#endif
