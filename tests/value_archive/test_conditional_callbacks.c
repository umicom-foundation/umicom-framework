/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_conditional_callbacks.c
 *
 * PURPOSE:
 *   Verify mutating and checking archive fixtures against the same real codec.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/value_archive.h"
#include "umicom/test_runtime/environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned malformed_reviews = 0U;
static unsigned checker_calls = 0U;

static UmiTestRuntimeEnvironment ArchiveSample(void)
{
    UmiTestRuntimeEnvironment value;
    umi_test_runtime_environment_init(&value, "conditional-fixture");
    value.entry_count = 7U;
    value.generation = 3U;
    value.revision = 11U;
    return value;
}

/* Compare semantic fields, leaving padding and unused text tails unobserved. */
static int SamplesEqual(const UmiTestRuntimeEnvironment *left,
    const UmiTestRuntimeEnvironment *right)
{
    return left->structure_size == right->structure_size &&
        strcmp(left->id, right->id) == 0 &&
        strcmp(left->name, right->name) == 0 &&
        strcmp(left->detail, right->detail) == 0 &&
        left->entry_count == right->entry_count &&
        left->generation == right->generation && left->revision == right->revision &&
        left->updated_at_ms == right->updated_at_ms && left->enabled == right->enabled;
}

static void FillTail(char *text, size_t capacity)
{
    size_t used = strlen(text) + 1U;
    memset(text + used, 0xa5, capacity - used);
}

static void ArchiveFillUnusedText(UmiTestRuntimeEnvironment *value)
{
    FillTail(value->id, sizeof(value->id));
    FillTail(value->name, sizeof(value->name));
    FillTail(value->detail, sizeof(value->detail));
}

/* Observe real invalid-review calls so a fixture that silently skips the
 * malformed callback cannot pass merely because valid round trips succeed. */
static UmiStatus RestoreObserved(const void *bytes, size_t length,
    const UmiTestRuntimeEnvironment *expected, UmiTestRuntimeEnvironment *value)
{
    if (expected && memchr(expected->id, '\0', sizeof(expected->id)) == NULL)
        ++malformed_reviews;
    return umi_test_runtime_environment_archive_decode_if_unchanged(
        bytes, length, expected, value);
}

#define ARCHIVE_TYPE UmiTestRuntimeEnvironment
#define ARCHIVE_ENCODE umi_test_runtime_environment_archive_encode
#define ARCHIVE_DECODE umi_test_runtime_environment_archive_decode
#define ARCHIVE_CONDITIONAL_DECODE RestoreObserved
#define ARCHIVE_EQUAL SamplesEqual
#include "record_cases.h"

/* Other typed fixture families already return an assertion result. Keep that
 * contract exercised alongside the record family's retained void mutator. */
static int CheckingMalformed(const UmiTestRuntimeEnvironment *sample)
{
    ++checker_calls;
    return ArchiveConditionalMalformedChecks(sample);
}

UMI_TEST_CONDITIONAL_ARCHIVE(ExplicitCheckerCases, UmiTestRuntimeEnvironment,
    umi_test_runtime_environment_archive_encode, RestoreObserved,
    SamplesEqual, ArchiveFillUnusedText, CheckingMalformed)

int main(void)
{
    UmiTestRuntimeEnvironment sample = ArchiveSample();
    if (ArchiveRecordCases() != 0 || ExplicitCheckerCases(&sample) != 0)
        return EXIT_FAILURE;
    if (malformed_reviews != 2U || checker_calls != 1U) {
        (void)fprintf(stderr, "Expected both malformed-review contracts; saw %u reviews and %u checker calls.\n",
            malformed_reviews, checker_calls);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
