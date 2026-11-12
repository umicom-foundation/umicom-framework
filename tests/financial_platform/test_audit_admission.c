/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_audit_admission.c
 *
 * PURPOSE:
 *   Verify audit boundaries and failure-preserving observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/financial_platform/service.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

static UmiFinancialAuditRecord AuditRow(uint64_t sequence)
{
    UmiFinancialAuditRecord row = {0};
    row.sequence = sequence;
    strcpy(row.eventId, "event"); strcpy(row.lineageId, "lineage");
    strcpy(row.actorId, "operator"); strcpy(row.domain, "practice");
    strcpy(row.entityId, "record"); strcpy(row.action, "observe");
    row.eventTimeMilliseconds = -1; row.status = UMI_STATUS_OK;
    return row;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiFinancialAuditJournal journal, before;
    UmiFinancialAuditRecord row = AuditRow(10);
    size_t persisted = 77;
    umi_financial_audit_journal_init(&journal);
    if (strcmp(test, "empty") == 0) {
        REQUIRE(umi_financial_audit_journal_persisted_count_checked(&journal, &persisted) == UMI_STATUS_OK && persisted == 0);
        journal.gapCount = 1; REQUIRE(umi_financial_audit_journal_validate(&journal) == UMI_STATUS_INVALID_STATE); return 0;
    }
    REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        row.sequence = 12; row.persisted = 1;
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_OK && journal.gapCount == 1);
        REQUIRE(umi_financial_audit_journal_persisted_count_checked(&journal, &persisted) == UMI_STATUS_OK && persisted == 1);
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, 10) == UMI_STATUS_OK);
        REQUIRE(umi_financial_audit_journal_persisted_count_checked(&journal, &persisted) == UMI_STATUS_OK && persisted == 2);
    } else if (strcmp(test, "text") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialAuditRecord, eventId), offsetof(UmiFinancialAuditRecord, causationId), offsetof(UmiFinancialAuditRecord, lineageId), offsetof(UmiFinancialAuditRecord, actorId), offsetof(UmiFinancialAuditRecord, domain), offsetof(UmiFinancialAuditRecord, entityId), offsetof(UmiFinancialAuditRecord, action)};
        const size_t lengths[] = {64, 64, 64, 64, 32, 64, 48};
        for (size_t i = 0; i < 7; ++i) {
            row = AuditRow(11); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &journal, sizeof journal);
            REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
        }
    } else if (strcmp(test, "flags") == 0) {
        row.sequence = 11; row.persisted = 2; REQUIRE(umi_financial_audit_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        row.persisted = 0; row.status = (UmiStatus)99; REQUIRE(umi_financial_audit_record_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        row.status = UMI_STATUS_BUSY; REQUIRE(umi_financial_audit_record_validate(&row) == UMI_STATUS_OK);
    } else if (strcmp(test, "count") == 0 || strcmp(test, "gaps") == 0 || strcmp(test, "stored-text") == 0) {
        if (strcmp(test, "count") == 0) journal.count = SIZE_MAX;
        if (strcmp(test, "gaps") == 0) journal.gapCount = UINT64_MAX;
        if (strcmp(test, "stored-text") == 0) memset(journal.records[0].actorId, 'x', 64);
        row.sequence = 11; memcpy(&before, &journal, sizeof journal);
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) != UMI_STATUS_OK);
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, 10) != UMI_STATUS_OK);
        REQUIRE(umi_financial_audit_journal_persisted_count_checked(&journal, &persisted) != UMI_STATUS_OK && persisted == 77);
        REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
    } else if (strcmp(test, "revision") == 0) {
        journal.revision = 0; REQUIRE(umi_financial_audit_journal_validate(&journal) == UMI_STATUS_INVALID_STATE);
        journal.revision = UINT64_MAX; row.sequence = 11; memcpy(&before, &journal, sizeof journal);
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, 10) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
    } else if (strcmp(test, "sequence-zero") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "backward") == 0) {
        row.sequence = strcmp(test, "sequence-zero") == 0 ? 0U : (strcmp(test, "duplicate") == 0 ? 10U : 9U);
        memcpy(&before, &journal, sizeof journal);
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) != UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
    } else if (strcmp(test, "exhausted") == 0) {
        umi_financial_audit_journal_init(&journal); row.sequence = UINT64_MAX;
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_OK && journal.gapCount == 0);
        row.sequence = 1; memcpy(&before, &journal, sizeof journal);
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, UINT64_MAX) == UMI_STATUS_OK);
    } else if (strcmp(test, "near-limit") == 0) {
        umi_financial_audit_journal_init(&journal); row.sequence = 1;
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_OK);
        row.sequence = UINT64_MAX;
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_OK && journal.gapCount == UINT64_MAX - 2U);
        REQUIRE(umi_financial_audit_journal_validate(&journal) == UMI_STATUS_OK);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_AUDIT_CAPACITY; ++i) journal.records[i] = AuditRow((uint64_t)i + 1U);
        journal.count = UMI_FINANCIAL_AUDIT_CAPACITY; row.sequence = 1025;
        memcpy(&before, &journal, sizeof journal);
        REQUIRE(umi_financial_audit_journal_validate(&journal) == UMI_STATUS_OK);
        REQUIRE(umi_financial_audit_journal_append(&journal, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
    } else if (strcmp(test, "repeat-mark") == 0) {
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, 10) == UMI_STATUS_OK);
        uint64_t revision = journal.revision;
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, 10) == UMI_STATUS_OK && journal.revision == revision + 1U);
        REQUIRE(umi_financial_audit_journal_persisted_count(&journal) == 1);
    } else if (strcmp(test, "missing") == 0) {
        memcpy(&before, &journal, sizeof journal);
        REQUIRE(umi_financial_audit_journal_mark_persisted(&journal, 9) == UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&before, &journal, sizeof journal) == 0);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_audit_journal_persisted_count_checked(NULL, &persisted) == UMI_STATUS_INVALID_ARGUMENT && persisted == 77);
        REQUIRE(umi_financial_audit_journal_persisted_count_checked(&journal, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
