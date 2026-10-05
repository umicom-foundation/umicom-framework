/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_evidence_record.c
 *
 * PURPOSE:
 *   Implement the test evidence record behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | evidence_record | Sammy Hegab | Umicom Foundation | MIT */
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/application/production/evidence_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiApplicationProductionEvidenceRecordTransferEqual(const UmiApplicationProductionEvidenceRecord *a, const UmiApplicationProductionEvidenceRecord *b)
{
    return strcmp(a->evidence_id, b->evidence_id) == 0 &&
        strcmp(a->reference, b->reference) == 0 &&
        a->kind == b->kind &&
        a->state == b->state &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiApplicationProductionEvidenceRecordTransferTails(UmiApplicationProductionEvidenceRecord *value)
{
    (void)value;
    {
        size_t used = strlen(value->evidence_id) + 1U;
        memset(value->evidence_id + used, 0xa5, sizeof(value->evidence_id) - used);
    }
    {
        size_t used = strlen(value->reference) + 1U;
        memset(value->reference + used, 0xa5, sizeof(value->reference) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiApplicationProductionEvidenceRecordTransferMalformed(const UmiApplicationProductionEvidenceRecord *sample)
{
    (void)sample;
    {
        UmiApplicationProductionEvidenceRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.evidence_id, 'x', sizeof(invalid.evidence_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_application_production_evidence_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_application_production_evidence_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated evidence_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiApplicationProductionEvidenceRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.reference, 'x', sizeof(invalid.reference));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_application_production_evidence_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_application_production_evidence_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated reference was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiApplicationProductionEvidenceRecordTransferCases, UmiApplicationProductionEvidenceRecord,
    umi_application_production_evidence_record_archive_encode, umi_application_production_evidence_record_archive_decode,
    UmiApplicationProductionEvidenceRecordTransferEqual, UmiApplicationProductionEvidenceRecordTransferTails, UmiApplicationProductionEvidenceRecordTransferMalformed)

int main(void) {
    UmiApplicationProductionEvidenceRecord record;
    assert(umi_application_production_evidence_record_set(&record, "org.umicom.studio:tests", UMI_APPLICATION_PRODUCTION_EVIDENCE_TEST, UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED, "ctest:studio", 1U) == UMI_STATUS_OK);
    assert(umi_application_production_evidence_record_validate(&record) == UMI_STATUS_OK);
    if (UmiApplicationProductionEvidenceRecordTransferCases(&record) != 0) return 1;

    return 0;
}

