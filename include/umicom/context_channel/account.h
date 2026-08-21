/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/account.h
 *
 * PURPOSE:
 *   Define the canonical account context shared by applications and panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ACCOUNT_H
#define UMICOM_CONTEXT_CHANNEL_ACCOUNT_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_account_context_init(UmiAccountContext *context);
UmiStatus umi_account_context_validate(const UmiAccountContext *context);
UmiStatus umi_account_context_copy(UmiAccountContext *destination, const UmiAccountContext *source);
UmiStatus umi_account_context_set_account_id(UmiAccountContext *context, const char *value);
UmiStatus umi_account_context_set_organisation_id(UmiAccountContext *context, const char *value);
UmiStatus umi_account_context_set_book_id(UmiAccountContext *context, const char *value);
UmiStatus umi_account_context_set_currency(UmiAccountContext *context, const char *value);
UmiStatus umi_account_context_set_account_type(UmiAccountContext *context, const char *value);
UmiStatus umi_account_context_set_environment(UmiAccountContext *context, const char *value);
#ifdef __cplusplus
}
#endif
#endif
