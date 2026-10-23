/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_release_approval_parse.c
 * PURPOSE: Strict C23 approval grammar and byte-for-byte output preservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_release_approval.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

#define APPROVED_SHA \
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"

static int RejectUnchanged(const char *source)
{
    UmiAiTrainingReleaseApproval output, previous;
    memset(&output, 0xA7, sizeof(output));
    previous = output;
    CHECK(UmiAiTrainingReleaseApprovalParse(
        source, strlen(source), &output) != UMI_STATUS_OK);
    CHECK(memcmp(&output, &previous, sizeof(output)) == 0);
    return 0;
}

int main(void)
{
    const char approved[] =
        "# Independently reviewed release file\r\n"
        "UMICOM-AI-RELEASE-APPROVAL-1\r\n"
        "publication_count=2\r\n"
        "evaluated_at_unix_seconds=150\r\n"
        "batch_sha256=" APPROVED_SHA "\r\n";
    UmiAiTrainingReleaseApproval selected = {0};
    UmiAiTrainingReleaseApproval untouched;
    char nulSource[] = "UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=\0";

    CHECK(UmiAiTrainingReleaseApprovalParse(approved, strlen(approved),
                                           &selected) == UMI_STATUS_OK);
    CHECK(selected.publicationCount == 2U);
    CHECK(selected.evaluatedAtUnixSeconds == UINT64_C(150));
    CHECK(strcmp(selected.batchSha256, APPROVED_SHA) == 0);

    CHECK(RejectUnchanged("publication_count=2\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=2\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=0\n"
                           "evaluated_at_unix_seconds=150\nbatch_sha256=" APPROVED_SHA "\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=9\n"
                           "evaluated_at_unix_seconds=150\nbatch_sha256=" APPROVED_SHA "\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=2\n"
                           "publication_count=2\nevaluated_at_unix_seconds=150\n"
                           "batch_sha256=" APPROVED_SHA "\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=2\n"
                           "evaluated_at_unix_seconds=-150\nbatch_sha256=" APPROVED_SHA "\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=2\n"
                           "evaluated_at_unix_seconds=18446744073709551616\n"
                           "batch_sha256=" APPROVED_SHA "\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=2\n"
                           "evaluated_at_unix_seconds=150\n"
                           "batch_sha256=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                           "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\npublication_count=2\n"
                           "evaluated_at_unix_seconds=150\n"
                           "batch_sha256=" APPROVED_SHA "\nextra=1\n") == 0);
    CHECK(RejectUnchanged("UMICOM-AI-RELEASE-APPROVAL-1\n publication_count=2\n"
                           "evaluated_at_unix_seconds=150\n"
                           "batch_sha256=" APPROVED_SHA "\n") == 0);

    memset(&selected, 0x47, sizeof(selected));
    untouched = selected;
    CHECK(UmiAiTrainingReleaseApprovalParse(
        nulSource, sizeof(nulSource), &selected) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&selected, &untouched, sizeof(selected)) == 0);
    CHECK(UmiAiTrainingReleaseApprovalParse(approved, strlen(approved),
        &selected) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingReleaseApprovalParse(NULL, 0U,
        &selected) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingReleaseApprovalParse(approved, strlen(approved),
        NULL) == UMI_STATUS_INVALID_ARGUMENT);
    puts("Strict reviewed release approval parser: PASS");
    return 0;
}
