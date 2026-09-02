/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/privacy_filter.c
 *
 * PURPOSE:
 *   Adapt the shared security redactor to diagnostics and exporter boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/privacy_filter.h"

#include <stddef.h>
/*
 * Initialise privacy filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_privacy_filter_init(UmiPrivacyFilter *filter, UmiRedactor *redactor)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (filter == NULL || redactor == NULL) return UMI_STATUS_INVALID_ARGUMENT; filter->redactor = redactor; return UMI_STATUS_OK; }
/*
 * Provide the privacy filter value operation used by this module and its client
 * applications.
 */
UmiStatus umi_privacy_filter_value(const UmiPrivacyFilter *filter, const char *field_name, const char *value, char *out_value, size_t capacity)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (filter == NULL || filter->redactor == NULL) return UMI_STATUS_INVALID_STATE; return umi_redactor_copy_value(filter->redactor, field_name, value, out_value, capacity); }
