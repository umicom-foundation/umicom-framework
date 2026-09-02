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

/*
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
/**
 * Represent the code guard service data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardService UmiCodeGuardService;
/**
 * Initialise codeguard service from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_codeguard_service_create(const char *root, UmiCodeGuardService **out_service);
/**
 * Release or reset state held by codeguard service so the same storage can be reused
 * safely.
 */
void umi_codeguard_service_destroy(UmiCodeGuardService *service);
/**
 * Provide the codeguard service scan operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_service_scan(UmiCodeGuardService *service, const UmiCodeGuardProfile *profile);
/**
 * Provide the codeguard service result operation used by this module and its client
 * applications.
 */
const UmiCodeGuardResult *umi_codeguard_service_result(const UmiCodeGuardService *service);
/**
 * Provide the codeguard service summary operation used by this module and its client
 * applications.
 */
UmiCodeGuardSummary umi_codeguard_service_summary(const UmiCodeGuardService *service);
#ifdef __cplusplus
}
#endif
#endif
