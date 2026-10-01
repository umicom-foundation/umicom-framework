/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/activity_private.h
 * PURPOSE: Own copied activity rows separately from mutable banking state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_ACTIVITY_PRIVATE_H
#define UMICOM_BANK_ACTIVITY_PRIVATE_H
#include "umicom/bank_operations/activity.h"
struct UmiBankActivity {
    UmiBankActivitySummary summary;
    UmiBankActivityRow rows[UMI_BANK_STATEMENT_CAPACITY];
};
void BankActivityDateText(UmiFinancialDate date, char out[11]);
#endif
