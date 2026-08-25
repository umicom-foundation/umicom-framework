/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/submission_channel.h
 *
 * PURPOSE:
 *   Describe one approved transport channel for regulatory submissions.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_CHANNEL_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_CHANNEL_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSubmissionChannel {
    char channel_id[UMI_REG_ID_CAPACITY];
    char authority_id[UMI_REG_ID_CAPACITY];
    char protocol[UMI_REG_NAME_CAPACITY];
    int enabled;
} UmiSubmissionChannel;

/* Initialise one validated describe one approved transport channel for regulatory submissions. record. */
UmiStatus umi_reg_submission_channel_init(UmiSubmissionChannel *record, const char *channel_id, const char *authority_id, const char *protocol, int enabled);

#ifdef __cplusplus
}
#endif

#endif
