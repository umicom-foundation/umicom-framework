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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * NULL bytes with zero capacity measures the required size. Decode validates
 * all fields before publishing and leaves the destination unchanged on refusal.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_currency_archive_encode(const UmiCurrency *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_currency_archive_decode(const void *bytes, size_t byte_count,
    UmiCurrency *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCurrency object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_currency_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCurrency *workspace, UmiCurrency *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_currency_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCurrency *expected, UmiCurrency *value);


/** Transfer this value using the portable format described in value_archive.h.
 * NULL bytes with zero capacity measures the required size. Decode validates
 * all fields before publishing and leaves the destination unchanged on refusal.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_financial_id_archive_encode(const UmiFinancialId *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_financial_id_archive_decode(const void *bytes, size_t byte_count,
    UmiFinancialId *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFinancialId object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_financial_id_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFinancialId *workspace, UmiFinancialId *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_financial_id_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFinancialId *expected, UmiFinancialId *value);


#ifdef __cplusplus
}
#endif
#endif
