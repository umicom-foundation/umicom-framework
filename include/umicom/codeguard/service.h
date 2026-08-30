/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/service.h
 *
 * PURPOSE:
 *   Expose a reusable CodeGuard service for Studio, Helix and other Umicom
 *   products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SERVICE_H
#define UMICOM_CODEGUARD_SERVICE_H
#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/summary.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCodeGuardService UmiCodeGuardService;
UmiStatus umi_codeguard_service_create(const char *root, UmiCodeGuardService **out_service);
void umi_codeguard_service_destroy(UmiCodeGuardService *service);
UmiStatus umi_codeguard_service_scan(UmiCodeGuardService *service, const UmiCodeGuardProfile *profile);
const UmiCodeGuardResult *umi_codeguard_service_result(const UmiCodeGuardService *service);
UmiCodeGuardSummary umi_codeguard_service_summary(const UmiCodeGuardService *service);
#ifdef __cplusplus
}
#endif
#endif
