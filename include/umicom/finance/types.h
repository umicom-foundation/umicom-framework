/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/types.h
 *
 * PURPOSE:
 *   Define fixed-size financial identifiers, currencies, money and account records used by reusable financial modules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Financial values are small plain C structures so every Umicom product can share them without exposing private implementation details.
 */

#ifndef UMICOM_FINANCE_TYPES_H
#define UMICOM_FINANCE_TYPES_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCE_ID_CAPACITY 48U
#define UMI_FINANCE_NAME_CAPACITY 96U

/**
 * Represent the financial id data shared with callers of this public contract.
 */
typedef struct UmiFinancialId {
    char value[UMI_FINANCE_ID_CAPACITY];
} UmiFinancialId;

/**
 * Represent the currency data shared with callers of this public contract.
 */
typedef struct UmiCurrency {
    char code[4];
} UmiCurrency;

/**
 * Represent the money data shared with callers of this public contract.
 */
typedef struct UmiMoney {
    int64_t minor_units;
    uint8_t scale;
    UmiCurrency currency;
} UmiMoney;

/**
 * Represent the quantity data shared with callers of this public contract.
 */
typedef struct UmiQuantity {
    double value;
} UmiQuantity;

/**
 * Represent the financial account data shared with callers of this public contract.
 */
typedef struct UmiFinancialAccount {
    UmiFinancialId account_id;
    UmiFinancialId party_id;
    UmiCurrency currency;
} UmiFinancialAccount;

/**
 * Represent the financial party data shared with callers of this public contract.
 */
typedef struct UmiFinancialParty {
    UmiFinancialId party_id;
    char display_name[UMI_FINANCE_NAME_CAPACITY];
} UmiFinancialParty;

/**
 * Represent the business time data shared with callers of this public contract.
 */
typedef struct UmiBusinessTime {
    int64_t epoch_millis;
} UmiBusinessTime;

/**
 * Represent the decimal data shared with callers of this public contract.
 */
typedef struct UmiDecimal {
    int64_t coefficient;
    uint8_t scale;
} UmiDecimal;

#ifdef __cplusplus
}
#endif
#endif
