/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payments_service.h
 *
 * PURPOSE:
 *   Represent Framework payment-service readiness, volume and exception evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENTS_SERVICE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENTS_SERVICE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentsService {
    UmiFinancialId id;
    bool running;
    size_t instruction_count;
    size_t settled_count;
    size_t exception_count;
    size_t unmatched_count;
} UmiPaymentsPaymentsService;
UmiStatus umi_payments_payments_service_init(UmiPaymentsPaymentsService *value,
    const char *id,
    bool running,
    size_t instruction_count,
    size_t settled_count,
    size_t exception_count,
    size_t unmatched_count);
bool umi_payments_payments_service_valid(const UmiPaymentsPaymentsService *value);
bool umi_payments_payments_service_ready(const UmiPaymentsPaymentsService *value);
#ifdef __cplusplus
}
#endif
#endif
