/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/snapshot_validation.c
 * PURPOSE: Check complete field ranges before bounded inspection of records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/snapshot_validation.h"
#include <string.h>

static UmiStatus Report(UmiSnapshotValidation *out, UmiSnapshotIssue issue,
    const char *field, size_t index, size_t capacity)
{
    if (out != NULL) *out = (UmiSnapshotValidation){issue, field, index, capacity};
    return issue == UMI_SNAPSHOT_VALID ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Range subtraction prevents offset+capacity overflow. Inspect the entire
 * schema first so a later invalid descriptor cannot cause an earlier read
 * from a record whose layout was not yet accepted. The same primitive serves
 * each Framework registry instead of duplicating unbounded string checks. */
UmiStatus UmiSnapshotValidateTextFields(const void *record, size_t recordSize,
    const UmiSnapshotTextField *fields, size_t fieldCount,
    UmiSnapshotValidation *outValidation)
{
    if (record == NULL)
        return Report(outValidation, UMI_SNAPSHOT_NULL_RECORD, NULL, SIZE_MAX, 0U);
    if (recordSize == 0U || (fields == NULL && fieldCount != 0U) ||
        fieldCount > SIZE_MAX / sizeof(*fields))
        return Report(outValidation, UMI_SNAPSHOT_INVALID_SCHEMA, NULL, SIZE_MAX, 0U);
    for (size_t index = 0U; index < fieldCount; ++index) {
        const UmiSnapshotTextField *field = &fields[index];
        if (field->name == NULL || field->capacity == 0U ||
            (field->required != 0 && field->required != 1) ||
            field->offset > recordSize || field->capacity > recordSize - field->offset)
            return Report(outValidation, UMI_SNAPSHOT_INVALID_SCHEMA,
                field->name, index, field->capacity);
    }
    const unsigned char *bytes = record;
    for (size_t index = 0U; index < fieldCount; ++index) {
        const UmiSnapshotTextField *field = &fields[index];
        const unsigned char *text = bytes + field->offset;
        if (memchr(text, '\0', field->capacity) == NULL)
            return Report(outValidation, UMI_SNAPSHOT_UNTERMINATED_TEXT,
                field->name, index, field->capacity);
        if (field->required && text[0] == '\0')
            return Report(outValidation, UMI_SNAPSHOT_EMPTY_REQUIRED_TEXT,
                field->name, index, field->capacity);
    }
    return Report(outValidation, UMI_SNAPSHOT_VALID, NULL, SIZE_MAX, 0U);
}

const char *UmiSnapshotIssueText(UmiSnapshotIssue issue)
{
    switch (issue) {
    case UMI_SNAPSHOT_VALID: return "valid snapshot";
    case UMI_SNAPSHOT_NULL_RECORD: return "snapshot is null";
    case UMI_SNAPSHOT_INVALID_SCHEMA: return "invalid bounded-text schema";
    case UMI_SNAPSHOT_UNTERMINATED_TEXT: return "text field has no terminator within its capacity";
    case UMI_SNAPSHOT_EMPTY_REQUIRED_TEXT: return "required text field is empty";
    case UMI_SNAPSHOT_DUPLICATE_ID: return "batch contains a duplicate identifier";
    case UMI_SNAPSHOT_SCOPE_MISMATCH: return "record belongs to a different document scope";
    default: return "unknown snapshot validation issue";
    }
}
