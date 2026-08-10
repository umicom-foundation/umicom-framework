/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/privacy_filter.h
 *
 * PURPOSE:
 *   Apply security redaction rules to observability fields before data reaches
 *   logs, exports or remote providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_PRIVACY_FILTER_H
#define UMICOM_DIAGNOSTICS_PRIVACY_FILTER_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/security/redaction.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPrivacyFilter { UmiRedactor *redactor; } UmiPrivacyFilter;
UmiStatus umi_privacy_filter_init(UmiPrivacyFilter *filter, UmiRedactor *redactor);
UmiStatus umi_privacy_filter_value(const UmiPrivacyFilter *filter,
                                   const char *field_name,
                                   const char *value,
                                   char *out_value,
                                   size_t capacity);
#ifdef __cplusplus
}
#endif

#endif
