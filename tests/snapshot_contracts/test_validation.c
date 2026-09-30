/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_validation.c
 * PURPOSE: Verify range overflow, bounded reads and useful field diagnostics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/snapshot_validation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Record { char id[4]; char label[8]; } Record;
static const UmiSnapshotTextField valid_fields[] = {
    {"id", offsetof(Record, id), 4U, 1}, {"label", offsetof(Record, label), 8U, 0}
};

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Record item = {"abc", "sample"};
    UmiSnapshotValidation detail;
    if (strcmp(argv[1], "valid") == 0) {
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), valid_fields, 2U, &detail) == UMI_STATUS_OK);
        CHECK(detail.issue == UMI_SNAPSHOT_VALID && detail.field == NULL && detail.field_index == SIZE_MAX && detail.capacity == 0U);
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), NULL, 0U, NULL) == UMI_STATUS_OK);
        item.label[0] = '\0'; item.label[7] = 'z';
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), valid_fields, 2U, NULL) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiSnapshotValidateTextFields(NULL, sizeof(item), valid_fields, 2U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(detail.issue == UMI_SNAPSHOT_NULL_RECORD && detail.field_index == SIZE_MAX);
        CHECK(UmiSnapshotValidateTextFields(&item, 0U, valid_fields, 2U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(detail.issue == UMI_SNAPSHOT_INVALID_SCHEMA);
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), NULL, 1U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), valid_fields, SIZE_MAX, &detail) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "schema") == 0) {
        const UmiSnapshotTextField invalid[] = {
            {NULL, 0U, 4U, 1}, {"zero", 0U, 0U, 0}, {"offset", SIZE_MAX, 1U, 0},
            {"overflow", 1U, SIZE_MAX, 0}, {"past", sizeof(item), 1U, 0},
            {"required", 0U, 4U, 2}, {"negative", 0U, 4U, -1}
        };
        for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); ++index) {
            CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), &invalid[index], 1U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(detail.issue == UMI_SNAPSHOT_INVALID_SCHEMA && detail.field_index == 0U);
        }
        /* An invalid later descriptor takes precedence over malformed text:
         * the implementation must finish checking the schema before scanning. */
        UmiSnapshotTextField fields[2] = {valid_fields[0], {"bad", SIZE_MAX, 8U, 0}};
        memset(item.id, 'x', sizeof(item.id));
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), fields, 2U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(detail.issue == UMI_SNAPSHOT_INVALID_SCHEMA && detail.field_index == 1U);
    } else if (strcmp(argv[1], "bounds") == 0) {
        memset(item.id, 'x', sizeof(item.id));
        item.label[0] = '\0';
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), valid_fields, 2U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(detail.issue == UMI_SNAPSHOT_UNTERMINATED_TEXT && detail.capacity == sizeof(item.id));
        item.id[3] = '\0';
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), valid_fields, 2U, NULL) == UMI_STATUS_OK);
        item.id[0] = '\0';
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), valid_fields, 2U, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(detail.issue == UMI_SNAPSHOT_EMPTY_REQUIRED_TEXT && strcmp(detail.field, "id") == 0);
    } else if (strcmp(argv[1], "immutable") == 0) {
        unsigned char before[sizeof(item)]; memcpy(before, &item, sizeof(item));
        UmiSnapshotTextField fields[2]; memcpy(fields, valid_fields, sizeof(fields));
        unsigned char schema_before[sizeof(fields)]; memcpy(schema_before, fields, sizeof(fields));
        CHECK(UmiSnapshotValidateTextFields(&item, sizeof(item), fields, 2U, &detail) == UMI_STATUS_OK);
        CHECK(memcmp(before, &item, sizeof(item)) == 0 && memcmp(schema_before, fields, sizeof(fields)) == 0);
    } else if (strcmp(argv[1], "messages") == 0) {
        CHECK(strcmp(UmiSnapshotIssueText(UMI_SNAPSHOT_VALID), "valid snapshot") == 0);
        CHECK(strstr(UmiSnapshotIssueText(UMI_SNAPSHOT_UNTERMINATED_TEXT), "terminator") != NULL);
        CHECK(strstr(UmiSnapshotIssueText(UMI_SNAPSHOT_DUPLICATE_ID), "duplicate") != NULL);
        CHECK(strstr(UmiSnapshotIssueText(UMI_SNAPSHOT_SCOPE_MISMATCH), "document scope") != NULL);
        CHECK(strstr(UmiSnapshotIssueText((UmiSnapshotIssue)999), "unknown") != NULL);
    } else return 2;
    return 0;
}
