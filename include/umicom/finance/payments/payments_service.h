/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payments_service.h
 *
 * PURPOSE:
 *   Represent Framework payment-service readiness, volume and exception evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENTS_SERVICE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENTS_SERVICE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payments service data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentsService {
    UmiFinancialId id;
    bool running;
    size_t instruction_count;
    size_t settled_count;
    size_t exception_count;
    size_t unmatched_count;
} UmiPaymentsPaymentsService;
/**
 * Initialise payments payments service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payments_service_init(UmiPaymentsPaymentsService *value,
    const char *id,
    bool running,
    size_t instruction_count,
    size_t settled_count,
    size_t exception_count,
    size_t unmatched_count);
/**
 * Check that payments payments service satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payments_service_valid(const UmiPaymentsPaymentsService *value);
/**
 * Provide the payments payments service ready operation used by this module and its client
 * applications.
 */
bool umi_payments_payments_service_ready(const UmiPaymentsPaymentsService *value);
#ifdef __cplusplus
}
#endif
#endif
