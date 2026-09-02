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
/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t i, j, pn;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || pattern == NULL) return 0;
    pn = strlen(pattern); /* Apply this branch only when its contract condition is satisfied. */ if (pn == 0U) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; text[i] != '\0'; ++i) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = 0U; j < pn && text[i + j] != '\0' && tolower((unsigned char)text[i + j]) == tolower((unsigned char)pattern[j]); ++j) { }
        /* Apply this branch only when its contract condition is satisfied. */
        if (j == pn) return 1;
    }
    return 0;
}
/*
 * Initialise redactor from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_redactor_create(UmiRedactor **out_redactor)
{
    static const char *defaults[] = {"password", "secret", "token", "credential", "api_key", "private_key"};
    size_t i; UmiRedactor *redactor; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_redactor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_redactor = NULL; redactor = (UmiRedactor *)calloc(1U, sizeof(*redactor));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (redactor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&redactor->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(redactor); return status; }
    *out_redactor = redactor;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < sizeof(defaults)/sizeof(defaults[0]); ++i) (void)umi_redactor_add_pattern(redactor, defaults[i]);
    return UMI_STATUS_OK;
}
/* Release or reset state held by redactor so the same storage can be reused safely. */
void umi_redactor_destroy(UmiRedactor *redactor)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (redactor != NULL) { umi_mutex_destroy(redactor->mutex); free(redactor); } }
/*
 * Provide the redactor add pattern operation used by this module and its client
 * applications.
 */
UmiStatus umi_redactor_add_pattern(UmiRedactor *redactor, const char *pattern)
{
    size_t i; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (redactor == NULL || pattern == NULL || pattern[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(pattern) >= UMI_REDACTION_PATTERN_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(redactor->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < redactor->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(redactor->patterns[i], pattern) == 0) { (void)umi_mutex_unlock(redactor->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (redactor->count >= UMI_REDACTION_RULE_MAX) { (void)umi_mutex_unlock(redactor->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    (void)strcpy(redactor->patterns[redactor->count++], pattern); (void)umi_mutex_unlock(redactor->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the redactor is sensitive operation used by this module and its client
 * applications.
 */
int umi_redactor_is_sensitive(const UmiRedactor *redactor, const char *field_name)
{
    size_t i; int sensitive = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (redactor == NULL || field_name == NULL) return 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_mutex_lock(redactor->mutex) != UMI_STATUS_OK) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < redactor->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (contains_case_insensitive(field_name, redactor->patterns[i])) { sensitive = 1; break; }
    (void)umi_mutex_unlock(redactor->mutex); return sensitive;
}
/*
 * Provide the redactor copy value operation used by this module and its client
 * applications.
 */
UmiStatus umi_redactor_copy_value(const UmiRedactor *redactor, const char *field_name, const char *value, char *out_value, size_t capacity)
{
    size_t length, i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (redactor == NULL || field_name == NULL || value == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!umi_redactor_is_sensitive(redactor, field_name)) { (void)memcpy(out_value, value, length + 1U); return UMI_STATUS_OK; }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < length; ++i) {
        out_value[i] = '*';
    }
    out_value[length] = '\0';
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by redactor pattern without changing their
 * state.
 */
size_t umi_redactor_pattern_count(const UmiRedactor *redactor)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (redactor != NULL && umi_mutex_lock(redactor->mutex) == UMI_STATUS_OK) { n = redactor->count; (void)umi_mutex_unlock(redactor->mutex); } return n; }
