/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diagnostic_parser_clang.c
 *
 * PURPOSE:
 *   Verify the built-in Clang diagnostic parser recognizes representative
 *   tool output and emits a normalized problem.
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
#include <assert.h>

#include "umicom/developer_productivity/diagnostic_parsers/clang.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/developer_productivity/problem.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDeveloperProblemTransferEqual(const UmiDeveloperProblem *a, const UmiDeveloperProblem *b)
{
    return a->problem_id == b->problem_id &&
        strcmp(a->source, b->source) == 0 &&
        strcmp(a->code, b->code) == 0 &&
        strcmp(a->message, b->message) == 0 &&
        a->severity == b->severity &&
        strcmp(a->location.uri, b->location.uri) == 0 &&
        a->location.line == b->location.line &&
        a->location.column == b->location.column &&
        a->location.end_line == b->location.end_line &&
        a->location.end_column == b->location.end_column &&
        a->suppressible == b->suppressible &&
        a->transient == b->transient &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDeveloperProblemTransferTails(UmiDeveloperProblem *value)
{
    (void)value;
    {
        size_t used = strlen(value->source) + 1U;
        memset(value->source + used, 0xa5, sizeof(value->source) - used);
    }
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
    {
        size_t used = strlen(value->location.uri) + 1U;
        memset(value->location.uri + used, 0xa5, sizeof(value->location.uri) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDeveloperProblemTransferMalformed(const UmiDeveloperProblem *sample)
{
    (void)sample;
    {
        UmiDeveloperProblem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source, 'x', sizeof(invalid.source));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_problem_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_problem_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperProblem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_problem_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_problem_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperProblem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_problem_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_problem_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperProblem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.location.uri, 'x', sizeof(invalid.location.uri));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_problem_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_problem_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated location.uri was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDeveloperProblemTransferCases, UmiDeveloperProblem,
    umi_developer_problem_archive_encode, umi_developer_problem_archive_decode,
    UmiDeveloperProblemTransferEqual, UmiDeveloperProblemTransferTails, UmiDeveloperProblemTransferMalformed)

int main(void)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    const UmiDeveloperDiagnosticParser *parser =
        umi_developer_diagnostic_parser_clang();

    assert(umi_developer_diagnostic_parser_validate(parser) == UMI_STATUS_OK);
    assert(parser->parse(
        "src/main.c:12:3: warning: unused variable",
        &problem,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(umi_developer_problem_validate(&problem) == UMI_STATUS_OK);
    if (UmiDeveloperProblemTransferCases(&problem) != 0) return 1;

    return 0;
}
