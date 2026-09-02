/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/privacy_filter.h
 *
 * PURPOSE:
 *   Apply security redaction rules to observability fields before data reaches
 *   logs, exports or remote providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_PRIVACY_FILTER_H
#define UMICOM_DIAGNOSTICS_PRIVACY_FILTER_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/security/redaction.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the privacy filter data shared with callers of this public contract.
 */
typedef struct UmiPrivacyFilter { UmiRedactor *redactor; } UmiPrivacyFilter;
/**
 * Initialise privacy filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_privacy_filter_init(UmiPrivacyFilter *filter, UmiRedactor *redactor);
/**
 * Provide the privacy filter value operation used by this module and its client
 * applications.
 */
UmiStatus umi_privacy_filter_value(const UmiPrivacyFilter *filter,
                                   const char *field_name,
                                   const char *value,
                                   char *out_value,
                                   size_t capacity);
#ifdef __cplusplus
}
#endif

#endif
