/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_audit_admission.c
 *
 * PURPOSE:
 *   Exercise retained audit bounds, exact sequence gaps, fixed text and publication refusal without mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/audit_journal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static UmiBrokerAuditRecord Record(uint64_t sequence)
{
    UmiBrokerAuditRecord r = {0}; r.sequence = sequence;
    strcpy(r.eventId, "event"); strcpy(r.source, "paper"); strcpy(r.kind, "submit"); strcpy(r.entityId, "order");
    return r;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    UmiBrokerAuditJournal *j = malloc(sizeof(*j)), *before = malloc(sizeof(*before));
    CHECK(j != NULL && before != NULL);
    umi_broker_audit_journal_init(j);
    UmiBrokerAuditRecord first = Record(1U), next = Record(2U);
    CHECK(umi_broker_audit_journal_append(j, &first) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "gap") == 0) next.sequence = 5U;
    else if (strcmp(mode, "last-sequence") == 0) next.sequence = UINT64_MAX;
    else if (strcmp(mode, "sequence-exhausted") == 0) {
        j->records[0].sequence = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "sequence-zero") == 0) { next.sequence = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "sequence-repeat") == 0) { next.sequence = 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strncmp(mode, "text-", 5U) == 0) {
        char *field = NULL; size_t capacity = 0U;
#define FIELD(n) if (strcmp(mode + 5, #n) == 0) { field = next.n; capacity = sizeof(next.n); }
        FIELD(eventId) FIELD(causationId) FIELD(source) FIELD(kind) FIELD(entityId) FIELD(detail)
#undef FIELD
        CHECK(field != NULL); memset(field, 'x', capacity); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "empty-kind") == 0) { next.kind[0] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "negative-time") == 0) { next.eventTimeMilliseconds = -1; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "bad-status") == 0) { next.status = (UmiStatus)999; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "bad-persisted") == 0) { next.persisted = 2; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "count-over") == 0 || strcmp(mode, "count-read") == 0 || strcmp(mode, "count-mark") == 0) {
        j->count = UMI_BROKER_AUDIT_CAPACITY + 1U; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "revision-zero") == 0) { j->revision = 0U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(mode, "revision-max") == 0) { j->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "gap-corrupt") == 0) { j->gapCount = 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(mode, "retained-text") == 0) {
        memset(j->records[0].source, 'x', sizeof(j->records[0].source)); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "retained-sequence") == 0) {
        CHECK(umi_broker_audit_journal_append(j, &next) == UMI_STATUS_OK);
        j->records[1].sequence = 1U; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "full") == 0) {
        for (uint64_t i = 2U; i <= UMI_BROKER_AUDIT_CAPACITY; ++i) {
            UmiBrokerAuditRecord r = Record(i);
            CHECK(umi_broker_audit_journal_append(j, &r) == UMI_STATUS_OK);
        }
        next.sequence = UMI_BROKER_AUDIT_CAPACITY + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "append") != 0 && strcmp(mode, "mark") != 0 &&
               strcmp(mode, "null-owner") != 0 && strcmp(mode, "null-record") != 0) return 2;
    memcpy(before, j, sizeof(*j));
    UmiStatus status;
    if (strcmp(mode, "count-read") == 0) {
        CHECK(umi_broker_audit_journal_persisted_count(j) == 0U);
        status = UmiBrokerAuditJournalValidate(j);
    } else if (strcmp(mode, "count-mark") == 0 || strcmp(mode, "mark") == 0)
        status = umi_broker_audit_journal_mark_persisted(j, 1U);
    else if (strcmp(mode, "null-owner") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT; status = umi_broker_audit_journal_append(NULL, &next);
    } else if (strcmp(mode, "null-record") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT; status = umi_broker_audit_journal_append(j, NULL);
    } else status = umi_broker_audit_journal_append(j, &next);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) CHECK(memcmp(before, j, sizeof(*j)) == 0);
    else {
        CHECK(UmiBrokerAuditJournalValidate(j) == UMI_STATUS_OK);
        CHECK(j->revision == before->revision + 1U);
        if (strcmp(mode, "mark") == 0) CHECK(umi_broker_audit_journal_persisted_count(j) == 1U);
        else CHECK(j->count == before->count + 1U);
        if (strcmp(mode, "gap") == 0) CHECK(j->gapCount == 3U);
        if (strcmp(mode, "last-sequence") == 0) CHECK(j->gapCount == UINT64_MAX - 2U);
    }
    free(before); free(j); return 0;
}
