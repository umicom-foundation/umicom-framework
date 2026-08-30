/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/redaction.c
 *
 * PURPOSE:
 *   Implement case-insensitive field classification and length-preserving
 *   masking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/redaction.h"

#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
struct UmiRedactor { char patterns[UMI_REDACTION_RULE_MAX][UMI_REDACTION_PATTERN_CAPACITY]; size_t count; UmiMutex *mutex; };
static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t i, j, pn;
    if (text == NULL || pattern == NULL) return 0;
    pn = strlen(pattern); if (pn == 0U) return 0;
    for (i = 0U; text[i] != '\0'; ++i) {
        for (j = 0U; j < pn && text[i + j] != '\0' && tolower((unsigned char)text[i + j]) == tolower((unsigned char)pattern[j]); ++j) { }
        if (j == pn) return 1;
    }
    return 0;
}
UmiStatus umi_redactor_create(UmiRedactor **out_redactor)
{
    static const char *defaults[] = {"password", "secret", "token", "credential", "api_key", "private_key"};
    size_t i; UmiRedactor *redactor; UmiStatus status;
    if (out_redactor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_redactor = NULL; redactor = (UmiRedactor *)calloc(1U, sizeof(*redactor));
    if (redactor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&redactor->mutex); if (status != UMI_STATUS_OK) { free(redactor); return status; }
    *out_redactor = redactor;
    for (i = 0U; i < sizeof(defaults)/sizeof(defaults[0]); ++i) (void)umi_redactor_add_pattern(redactor, defaults[i]);
    return UMI_STATUS_OK;
}
void umi_redactor_destroy(UmiRedactor *redactor)
{ if (redactor != NULL) { umi_mutex_destroy(redactor->mutex); free(redactor); } }
UmiStatus umi_redactor_add_pattern(UmiRedactor *redactor, const char *pattern)
{
    size_t i; UmiStatus status;
    if (redactor == NULL || pattern == NULL || pattern[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(pattern) >= UMI_REDACTION_PATTERN_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(redactor->mutex); if (status != UMI_STATUS_OK) return status;
    for (i = 0U; i < redactor->count; ++i) if (strcmp(redactor->patterns[i], pattern) == 0) { (void)umi_mutex_unlock(redactor->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    if (redactor->count >= UMI_REDACTION_RULE_MAX) { (void)umi_mutex_unlock(redactor->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    (void)strcpy(redactor->patterns[redactor->count++], pattern); (void)umi_mutex_unlock(redactor->mutex); return UMI_STATUS_OK;
}
int umi_redactor_is_sensitive(const UmiRedactor *redactor, const char *field_name)
{
    size_t i; int sensitive = 0;
    if (redactor == NULL || field_name == NULL) return 0;
    if (umi_mutex_lock(redactor->mutex) != UMI_STATUS_OK) return 0;
    for (i = 0U; i < redactor->count; ++i) if (contains_case_insensitive(field_name, redactor->patterns[i])) { sensitive = 1; break; }
    (void)umi_mutex_unlock(redactor->mutex); return sensitive;
}
UmiStatus umi_redactor_copy_value(const UmiRedactor *redactor, const char *field_name, const char *value, char *out_value, size_t capacity)
{
    size_t length, i;
    if (redactor == NULL || field_name == NULL || value == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value); if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_redactor_is_sensitive(redactor, field_name)) { (void)memcpy(out_value, value, length + 1U); return UMI_STATUS_OK; }
    for (i = 0U; i < length; ++i) {
        out_value[i] = '*';
    }
    out_value[length] = '\0';
    return UMI_STATUS_OK;
}
size_t umi_redactor_pattern_count(const UmiRedactor *redactor)
{ size_t n = 0U; if (redactor != NULL && umi_mutex_lock(redactor->mutex) == UMI_STATUS_OK) { n = redactor->count; (void)umi_mutex_unlock(redactor->mutex); } return n; }
