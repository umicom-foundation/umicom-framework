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
typedef struct UmiRedactor UmiRedactor;
UmiStatus umi_redactor_create(UmiRedactor **out_redactor);
void umi_redactor_destroy(UmiRedactor *redactor);
UmiStatus umi_redactor_add_pattern(UmiRedactor *redactor, const char *pattern);
int umi_redactor_is_sensitive(const UmiRedactor *redactor, const char *field_name);
UmiStatus umi_redactor_copy_value(const UmiRedactor *redactor,
                                  const char *field_name,
                                  const char *value,
                                  char *out_value,
                                  size_t capacity);
size_t umi_redactor_pattern_count(const UmiRedactor *redactor);
#ifdef __cplusplus
}
#endif

#endif
