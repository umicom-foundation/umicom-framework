/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/service.h
 *
 * PURPOSE:
 *   Discover the shared financial platform control policies and provide a
 *   cross-domain readiness snapshot over the Framework financial control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_SERVICE_H
#define UMICOM_FINANCIAL_PLATFORM_SERVICE_H

#include <stddef.h>

#include "umicom/financial_platform/types.h"
#include "umicom/financial_platform/financial_control_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFinancialPlatformServiceSnapshot {
    uint64_t revision;
    size_t policyCount;
    size_t rateCount;
    size_t relationshipCount;
    size_t instrumentCount;
    size_t tradeCount;
    size_t positionCount;
    size_t paymentCount;
    size_t settlementCount;
    size_t auditCount;
    uint64_t auditGapCount;
    size_t persistedAuditCount;
    int referenceDataReady;
    int lifecycleReady;
    int auditReady;
} UmiFinancialPlatformServiceSnapshot;

size_t umi_financial_platform_service_count(void);

UmiStatus umi_financial_platform_service_id_at(
    size_t index,
    const char **outId);

UmiStatus umi_financial_platform_service_evaluate(
    const char *id,
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

UmiStatus umi_financial_platform_service_snapshot(
    const UmiFinancialControlService *service,
    UmiFinancialPlatformServiceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
