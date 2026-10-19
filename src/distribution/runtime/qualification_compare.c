/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_compare.c
 * Purpose: Retain exact evidence transitions and coverage loss across closed attempts.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/distribution/runtime/qualification_compare.h"
#include <stdlib.h>
#include <string.h>
struct UmiQualificationComparison {
    UmiQualificationComparisonSummary summary;
    UmiQualificationDifference *rows;
};
void UmiQualificationComparisonDestroy(UmiQualificationComparison *comparison)
{
    if (comparison == NULL) return;
    for (size_t i = 0U; i < comparison->summary.rows; ++i) free((void *)comparison->rows[i].name);
    free(comparison->rows); free(comparison);
}
static const UmiQualificationViewRow *Next(const UmiQualificationView *view, size_t *index)
{
    const UmiQualificationViewRow *row = UmiQualificationViewAt(view, *index);
    while (row != NULL && row->kind == UMI_QUALIFICATION_VIEW_TARGET)
        row = UmiQualificationViewAt(view, ++*index);
    return row;
}
static int Order(const UmiQualificationViewRow *a, const UmiQualificationViewRow *b)
{
    if (a == NULL) return b == NULL ? 0 : 1;
    if (b == NULL) return -1;
    if (a->kind != b->kind) return a->kind < b->kind ? -1 : 1;
    return strcmp(a->name, b->name);
}
static bool Passed(const UmiQualificationViewRow *row)
{
    return row != NULL && row->recorded && row->result.outcome == UMI_QUALIFICATION_PASSED;
}
static UmiQualificationChange Change(const UmiQualificationViewRow *before, const UmiQualificationViewRow *after)
{
    if (before == NULL) return UMI_QUALIFICATION_ADDED;
    if (after == NULL) return UMI_QUALIFICATION_REMOVED;
    if (Passed(before) && !Passed(after)) return UMI_QUALIFICATION_REGRESSED;
    if (!Passed(before) && Passed(after)) return UMI_QUALIFICATION_RECOVERED;
    if (before->recorded != after->recorded) return UMI_QUALIFICATION_RESULT_CHANGED;
    if (!before->recorded) return UMI_QUALIFICATION_UNCHANGED;
    return before->result.outcome == after->result.outcome && before->result.status == after->result.status &&
        before->result.exitCodeKnown == after->result.exitCodeKnown && before->result.exitCode == after->result.exitCode
        ? UMI_QUALIFICATION_UNCHANGED : UMI_QUALIFICATION_RESULT_CHANGED;
}
UmiStatus UmiQualificationCompare(const UmiQualificationView *before, const UmiQualificationView *after,
    UmiQualificationComparison **outComparison)
{
    if (before == NULL || after == NULL || outComparison == NULL || *outComparison != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiQualificationViewSummary a, b;
    UmiStatus status = UmiQualificationViewDescribe(before, &a);
    if (status == UMI_STATUS_OK) status = UmiQualificationViewDescribe(after, &b);
    if (status != UMI_STATUS_OK) return status;
    if (!a.results.closed || !b.results.closed) return UMI_STATUS_UNAVAILABLE;
    if (a.results.tests > UMI_RELEASE_INVENTORY_RECORD_LIMIT ||
        b.results.tests > UMI_RELEASE_INVENTORY_RECORD_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t maximum = a.results.tests + b.results.tests + 2U;
    if (maximum > SIZE_MAX / sizeof(UmiQualificationDifference)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiQualificationComparison *comparison = calloc(1U, sizeof(*comparison));
    if (comparison == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    comparison->rows = calloc(maximum, sizeof(*comparison->rows));
    if (comparison->rows == NULL) { free(comparison); return UMI_STATUS_OUT_OF_MEMORY; }
    comparison->summary.before = a; comparison->summary.after = b;
    comparison->summary.sameRecordedPlan = strcmp(a.identity.plan, b.identity.plan) == 0;
    comparison->summary.sameExecution = strcmp(a.identity.execution, b.identity.execution) == 0;
    size_t left = 0U, right = 0U, names = 0U;
    for (;;) {
        const UmiQualificationViewRow *old = Next(before, &left), *current = Next(after, &right);
        if (old == NULL && current == NULL) break;
        int order = Order(old, current);
        if (order < 0) current = NULL;
        if (order > 0) old = NULL;
        const UmiQualificationViewRow *identity = old != NULL ? old : current;
        size_t length = strlen(identity->name) + 1U;
        if (length > 2U * (size_t)UMI_RELEASE_INVENTORY_TEXT_LIMIT - names) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; break;
        }
        char *name = malloc(length);
        if (name == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
        memcpy(name, identity->name, length); names += length;
        UmiQualificationDifference *row = &comparison->rows[comparison->summary.rows];
        row->index = comparison->summary.rows++; row->name = name; row->kind = identity->kind;
        row->beforePresent = old != NULL; row->afterPresent = current != NULL;
        if (old != NULL) { row->beforeRecorded = old->recorded; row->before = old->result; ++left; }
        if (current != NULL) { row->afterRecorded = current->recorded; row->after = current->result; ++right; }
        row->change = Change(old, current);
        switch (row->change) {
        case UMI_QUALIFICATION_UNCHANGED: ++comparison->summary.unchanged; break;
        case UMI_QUALIFICATION_ADDED: ++comparison->summary.added; break;
        case UMI_QUALIFICATION_REMOVED: ++comparison->summary.removed; break;
        case UMI_QUALIFICATION_REGRESSED: ++comparison->summary.regressed; break;
        case UMI_QUALIFICATION_RECOVERED: ++comparison->summary.recovered; break;
        case UMI_QUALIFICATION_RESULT_CHANGED: ++comparison->summary.resultChanged; break;
        }
    }
    if (status == UMI_STATUS_OK) *outComparison = comparison;
    else UmiQualificationComparisonDestroy(comparison);
    return status;
}
UmiStatus UmiQualificationComparisonDescribe(const UmiQualificationComparison *comparison,
    UmiQualificationComparisonSummary *outSummary)
{
    if (comparison == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = comparison->summary; return UMI_STATUS_OK;
}
size_t UmiQualificationComparisonCount(const UmiQualificationComparison *comparison)
{
    return comparison != NULL ? comparison->summary.rows : 0U;
}
const UmiQualificationDifference *UmiQualificationComparisonAt(const UmiQualificationComparison *comparison, size_t index)
{
    return comparison != NULL && index < comparison->summary.rows ? &comparison->rows[index] : NULL;
}
const char *UmiQualificationChangeText(UmiQualificationChange change)
{
    switch (change) {
    case UMI_QUALIFICATION_UNCHANGED: return "Unchanged outcome";
    case UMI_QUALIFICATION_ADDED: return "Added coverage";
    case UMI_QUALIFICATION_REMOVED: return "Removed coverage";
    case UMI_QUALIFICATION_REGRESSED: return "Previously passed; now missing or non-passing";
    case UMI_QUALIFICATION_RECOVERED: return "Now passed";
    case UMI_QUALIFICATION_RESULT_CHANGED: return "Result changed";
    }
    return "Unknown";
}
