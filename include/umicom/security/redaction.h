/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/redaction.h
 *
 * PURPOSE:
 *   Define reusable redaction rules for diagnostic fields, configuration values,
 *   audit records and command output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_REDACTION_H
#define UMICOM_SECURITY_REDACTION_H

#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_REDACTION_PATTERN_CAPACITY 96U
#define UMI_REDACTION_RULE_MAX 64U
/**
 * Represent the redactor data shared with callers of this public contract.
 */
typedef struct UmiRedactor UmiRedactor;
/**
 * Initialise redactor from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_redactor_create(UmiRedactor **out_redactor);
/**
 * Release or reset state held by redactor so the same storage can be reused safely.
 */
void umi_redactor_destroy(UmiRedactor *redactor);
/**
 * Provide the redactor add pattern operation used by this module and its client
 * applications.
 */
UmiStatus umi_redactor_add_pattern(UmiRedactor *redactor, const char *pattern);
/**
 * Provide the redactor is sensitive operation used by this module and its client
 * applications.
 */
int umi_redactor_is_sensitive(const UmiRedactor *redactor, const char *field_name);
/**
 * Provide the redactor copy value operation used by this module and its client
 * applications.
 */
UmiStatus umi_redactor_copy_value(const UmiRedactor *redactor,
                                  const char *field_name,
                                  const char *value,
                                  char *out_value,
                                  size_t capacity);
/**
 * Return the number of records represented by redactor pattern without changing their
 * state.
 */
size_t umi_redactor_pattern_count(const UmiRedactor *redactor);
#ifdef __cplusplus
}
#endif

#endif
