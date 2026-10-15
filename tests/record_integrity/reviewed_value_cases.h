/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/record_integrity/reviewed_value_cases.h
 * PURPOSE: Share reviewed-publication regression cases using each real workbench fixture and equality function.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_REVIEWED_VALUE_CASES_H
#define UMICOM_TEST_REVIEWED_VALUE_CASES_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A per-domain fixture supplies real accepted values and a complete field-wise
 * equality function. Failure comparisons use copied bytes because refusal must
 * not write anything, including padding. Success compares semantic fields. */
#define UMI_TEST_REVIEWED_VALUE(Function, Record, Identity, Text, Replace, Equal) \
static int Function(const Record *sample) \
{ \
    Record *live = malloc(sizeof *live), *draft = malloc(sizeof *draft), *expected = malloc(sizeof *expected); \
    unsigned char *before = malloc(sizeof *live); \
    int failed = 0; \
    if (live == NULL || draft == NULL || expected == NULL || before == NULL) { failed = 1; goto done; } \
    *live = *sample; live->revision = 7U; \
    *draft = *live; \
    draft->Text[0] = live->Text[0] == 'r' ? 'q' : 'r'; draft->Text[1] = '\0'; \
    draft->revision = UINT64_MAX; \
    *expected = *draft; expected->revision = 8U; \
    if (Replace(live, 7U, draft) != UMI_STATUS_OK || !Equal(live, expected)) { failed = 1; goto done; } \
    /* A stale result must not erase the accepted payload or consume a token. */ \
    memcpy(before, live, sizeof *live); \
    if (Replace(live, 7U, sample) != UMI_STATUS_INVALID_STATE || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    if (Replace(NULL, 8U, draft) != UMI_STATUS_INVALID_ARGUMENT || \
        Replace(live, 8U, NULL) != UMI_STATUS_INVALID_ARGUMENT || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    *draft = *live; draft->Identity[0] = live->Identity[0] == 'z' ? 'y' : 'z'; \
    if (Replace(live, 8U, draft) != UMI_STATUS_INVALID_ARGUMENT || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    *draft = *live; draft->Identity[0] = '\0'; \
    if (Replace(live, 8U, draft) != UMI_STATUS_INVALID_ARGUMENT || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    *draft = *live; memset(draft->Identity, 'x', sizeof draft->Identity); \
    if (Replace(live, 8U, draft) != UMI_STATUS_INVALID_ARGUMENT || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    *draft = *live; memset(draft->Text, 'x', sizeof draft->Text); \
    if (Replace(live, 8U, draft) == UMI_STATUS_OK || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    /* A malformed live owner is refused rather than silently repaired by a draft. */ \
    *draft = *live; memset(live->Identity, 'x', sizeof live->Identity); \
    memcpy(before, live, sizeof *live); \
    if (Replace(live, 8U, draft) != UMI_STATUS_INVALID_ARGUMENT || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
    *live = *expected; \
    if (Replace(live, 8U, live) != UMI_STATUS_OK) { failed = 1; goto done; } \
    expected->revision = 9U; \
    if (!Equal(live, expected)) { failed = 1; goto done; } \
    live->revision = UINT64_MAX - 1U; *draft = *live; \
    if (Replace(live, UINT64_MAX - 1U, draft) != UMI_STATUS_OK || live->revision != UINT64_MAX) { failed = 1; goto done; } \
    memcpy(before, live, sizeof *live); \
    if (Replace(live, UINT64_MAX, draft) != UMI_STATUS_CAPACITY_EXCEEDED || memcmp(before, live, sizeof *live) != 0) { failed = 1; goto done; } \
done: \
    if (failed) (void)fprintf(stderr, "Reviewed value contract failed: %s\n", #Record); \
    free(before); free(expected); free(draft); free(live); \
    return failed; \
}

/* Existing setters remain useful for preparing drafts. Their failure must not
 * erase accepted text, and no legacy edit may wrap the token used by reviews. */
#define UMI_TEST_REVIEWED_TEXT_FIELD(Record, Field, Set, Sample) do { \
    Record field_value = *(Sample); \
    field_value.revision = 12U; \
    unsigned char saved[sizeof field_value]; \
    char oversized[sizeof field_value.Field + 1U]; \
    memset(oversized, 'x', sizeof oversized); oversized[sizeof oversized - 1U] = '\0'; \
    if (Set(NULL, "text") != UMI_STATUS_INVALID_ARGUMENT || \
        Set(&field_value, "retained") != UMI_STATUS_OK || field_value.revision != 13U) return 1; \
    if (Set(&field_value, field_value.Field + 1U) != UMI_STATUS_OK || \
        strcmp(field_value.Field, "etained") != 0 || field_value.revision != 14U) return 1; \
    memcpy(saved, &field_value, sizeof field_value); \
    if (Set(&field_value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED || \
        memcmp(saved, &field_value, sizeof field_value) != 0) return 1; \
    if (Set(&field_value, NULL) != UMI_STATUS_INVALID_ARGUMENT || \
        memcmp(saved, &field_value, sizeof field_value) != 0) return 1; \
    oversized[sizeof field_value.Field - 1U] = '\0'; \
    if (Set(&field_value, oversized) != UMI_STATUS_OK || \
        strlen(field_value.Field) != sizeof field_value.Field - 1U) return 1; \
    field_value.revision = UINT64_MAX - 1U; \
    if (Set(&field_value, "last") != UMI_STATUS_OK || field_value.revision != UINT64_MAX) return 1; \
    memcpy(saved, &field_value, sizeof field_value); \
    if (Set(&field_value, "refused") != UMI_STATUS_CAPACITY_EXCEEDED || \
        memcmp(saved, &field_value, sizeof field_value) != 0) return 1; \
} while (0)

#endif
