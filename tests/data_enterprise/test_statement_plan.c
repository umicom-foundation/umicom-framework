/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_statement_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the statement plan enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/data/enterprise/statement_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/statement_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataStatementPlanTransferEqual(const UmiDataStatementPlan *a, const UmiDataStatementPlan *b)
{
    return strcmp(a->statement_id, b->statement_id) == 0 &&
        a->query_fingerprint == b->query_fingerprint &&
        a->schema_fingerprint == b->schema_fingerprint &&
        a->parameter_count == b->parameter_count &&
        a->read_only == b->read_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataStatementPlanTransferTails(UmiDataStatementPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->statement_id) + 1U;
        memset(value->statement_id + used, 0xa5, sizeof(value->statement_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataStatementPlanTransferMalformed(const UmiDataStatementPlan *sample)
{
    (void)sample;
    {
        UmiDataStatementPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.statement_id, 'x', sizeof(invalid.statement_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_statement_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_statement_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated statement_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataStatementPlanTransferCases, UmiDataStatementPlan,
    umi_data_statement_plan_archive_encode, umi_data_statement_plan_archive_decode,
    UmiDataStatementPlanTransferEqual, UmiDataStatementPlanTransferTails, UmiDataStatementPlanTransferMalformed)

int main(void) {
    UmiDataStatementPlan item;
    CHECK(umi_data_statement_plan_init(&item,"stmt1",11U,22U,2U,true) == UMI_STATUS_OK);
    if (UmiDataStatementPlanTransferCases(&item) != 0) return 1;

    CHECK(item.read_only);
    return 0;
}
