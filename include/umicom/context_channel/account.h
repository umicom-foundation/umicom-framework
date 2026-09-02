/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/account.h
 *
 * PURPOSE:
 *   Define the canonical account context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ACCOUNT_H
#define UMICOM_CONTEXT_CHANNEL_ACCOUNT_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the account context data shared with callers of this public contract.
 */
typedef struct UmiAccountContext {
    uint32_t structure_size;
    char account_id[UMI_CONTEXT_TEXT_CAPACITY];
    char organisation_id[UMI_CONTEXT_TEXT_CAPACITY];
    char book_id[UMI_CONTEXT_TEXT_CAPACITY];
    char currency[32U];
    char account_type[64U];
    char environment[64U];
    uint64_t revision;
} UmiAccountContext;
/**
 * Initialise account context from caller-provided values so later operations receive a
 * known state.
 */
void umi_account_context_init(UmiAccountContext *context);
/**
 * Check that account context satisfies its contract before another service relies on it.
 */
UmiStatus umi_account_context_validate(const UmiAccountContext *context);
/**
 * Copy account context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_account_context_copy(UmiAccountContext *destination, const UmiAccountContext *source);
/**
 * Provide the account context set account id operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_account_id(UmiAccountContext *context, const char *value);
/**
 * Provide the account context set organisation id operation used by this module and its
 * client applications.
 */
UmiStatus umi_account_context_set_organisation_id(UmiAccountContext *context, const char *value);
/**
 * Provide the account context set book id operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_book_id(UmiAccountContext *context, const char *value);
/**
 * Provide the account context set currency operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_currency(UmiAccountContext *context, const char *value);
/**
 * Provide the account context set account type operation used by this module and its
 * client applications.
 */
UmiStatus umi_account_context_set_account_type(UmiAccountContext *context, const char *value);
/**
 * Provide the account context set environment operation used by this module and its client
 * applications.
 */
UmiStatus umi_account_context_set_environment(UmiAccountContext *context, const char *value);
#ifdef __cplusplus
}
#endif
#endif
