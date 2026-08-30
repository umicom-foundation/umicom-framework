/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_rail_catalogue.h
 *
 * PURPOSE:
 *   Catalogue payment rails with deterministic identifier lookup and duplicate prevention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_RAIL_CATALOGUE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_RAIL_CATALOGUE_H
#include "umicom/finance/payments/payment_rail.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentRailCatalogue { UmiPaymentsPaymentRail items[UMI_PAYMENTS_MAX_ITEMS]; size_t count; } UmiPaymentsPaymentRailCatalogue;
void umi_payments_payment_rail_catalogue_init(UmiPaymentsPaymentRailCatalogue *registry);
UmiStatus umi_payments_payment_rail_catalogue_add(UmiPaymentsPaymentRailCatalogue *registry,const UmiPaymentsPaymentRail *item);
const UmiPaymentsPaymentRail *umi_payments_payment_rail_catalogue_find(const UmiPaymentsPaymentRailCatalogue *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
