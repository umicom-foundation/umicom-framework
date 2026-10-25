/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_reviewed_examples.c
 * PURPOSE: Verify training JSONL derives only from cryptographically checked,
 *          explicitly approved text and never publishes partial output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/reviewed_examples.h"
#include "umicom/base/sha256.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "reviewed example line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    static const char original[] = "Fictional lesson source with explicit consent.";
    static const char prepared[] =
        "Question: What is a vector?\nAnswer: An ordered list of numbers.\n";
    static const char expected[] =
        "{\"prompt\":\"What is a vector?\",\"completion\":\"An ordered list of numbers.\"}\n";
    char originalHex[65] = {0};
    char preparedHex[65] = {0};
    char json[512];
    char previous[sizeof(json)];
    size_t produced = 999U;
    UmiAiTrainingReviewedExampleEvidence evidence = {0};
    UmiAiTrainingReviewedExampleEvidence previousEvidence;
    UmiAiTrainingSourceReview review = {0};
    UmiAiTrainingTextSpan question = {10U, 17U};
    UmiAiTrainingTextSpan answer = {36U, 27U};

    CHECK(UmiSha256Buffer(original, sizeof(original) - 1U,
                          originalHex) == UMI_STATUS_OK);
    CHECK(UmiSha256Buffer(prepared, sizeof(prepared) - 1U,
                          preparedHex) == UMI_STATUS_OK);
    review.relativeTextPath = "train/approved-lesson.txt";
    review.originalAssetSha256 = originalHex;
    review.preparedTextSha256 = preparedHex;
    review.licenceId = "CC0-1.0";
    review.rightsEvidenceId = "AUTHOR-APPROVED";
    review.medium = UMI_AI_TRAINING_SOURCE_PLAIN_TEXT;
    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC;
    review.rightsReviewed = 1;
    review.trainingUseApproved = 1;
    review.personalDataReviewed = 1;
    review.extractedTextReviewed = 1;

    /* Explicit byte offsets make source lineage inspectable and prevent
     * silently inventing a completion that was not in the vetted derivative. */
    CHECK(memcmp(prepared + question.offset, "What is a vector?",
                 question.byteCount) == 0);
    CHECK(memcmp(prepared + answer.offset, "An ordered list of numbers.",
                 answer.byteCount) == 0);

    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          prepared, sizeof(prepared) - 1U,
          question, answer, 20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_OK);
    CHECK(produced == sizeof(expected) - 1U);
    CHECK(strcmp(json, expected) == 0);
    CHECK(evidence.jsonBytes == produced);
    CHECK(evidence.jsonSha256[64] == '\0');
    CHECK(evidence.admittedSource.relativeTextPath[0] == 't');

    /* Unicode spans must align with complete UTF-8 codepoints. */
    static const unsigned char unicodeText[] = {
        'Q', ':', ' ', 0xc3U, 0xa9U, '\n',
        'A', ':', ' ', 'y', 'e', 's', '\n'
    };
    char unicodeDigest[65] = {0};
    CHECK(UmiSha256Buffer(unicodeText, sizeof(unicodeText),
                          unicodeDigest) == UMI_STATUS_OK);
    review.preparedTextSha256 = unicodeDigest;
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          unicodeText, sizeof(unicodeText),
          (UmiAiTrainingTextSpan){3U, 2U},
          (UmiAiTrainingTextSpan){9U, 3U},
          20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_OK);
    CHECK(strstr(json, "yes") != NULL);
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          unicodeText, sizeof(unicodeText),
          (UmiAiTrainingTextSpan){4U, 1U},
          (UmiAiTrainingTextSpan){9U, 3U},
          20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_INVALID_ARGUMENT);
    review.preparedTextSha256 = preparedHex;

    /* Quotes, backslashes and line breaks must remain valid JSON text. */
    static const char quotedText[] = "Q: a\"b\nA: line\\path\n";
    static const char escapedExpected[] =
        "{\"prompt\":\"a\\\"b\",\"completion\":\"line\\\\path\"}\n";
    char quotedDigest[65] = {0};
    CHECK(UmiSha256Buffer(quotedText, sizeof(quotedText) - 1U,
                          quotedDigest) == UMI_STATUS_OK);
    review.preparedTextSha256 = quotedDigest;
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          quotedText, sizeof(quotedText) - 1U,
          (UmiAiTrainingTextSpan){3U, 3U},
          (UmiAiTrainingTextSpan){10U, 9U},
          20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_OK);
    CHECK(strcmp(json, escapedExpected) == 0);
    review.preparedTextSha256 = preparedHex;

    memset(json, 0x6a, sizeof(json));
    memcpy(previous, json, sizeof(json));
    memset(&evidence, 0x5b, sizeof(evidence));
    memcpy(&previousEvidence, &evidence, sizeof(evidence));
    produced = 999U;
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          prepared, sizeof(prepared) - 1U,
          question, answer, 20U, json, 4U, &produced,
          &evidence) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(produced == 999U);
    CHECK(memcmp(json, previous, sizeof(json)) == 0);
    CHECK(memcmp(&evidence, &previousEvidence, sizeof(evidence)) == 0);

    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PRIVATE;
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          prepared, sizeof(prepared) - 1U,
          question, answer, 20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(json, previous, sizeof(json)) == 0);
    CHECK(memcmp(&evidence, &previousEvidence, sizeof(evidence)) == 0);
    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC;

    /* An altered derivative is not accepted even when the review flags
     * remain true: both digests are checked against supplied bytes. */
    static const char altered[] =
        "Question: What is a vector?\nAnswer: An unordered list of numbers.\n";
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          altered, sizeof(altered) - 1U,
          question, answer, 20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(json, previous, sizeof(json)) == 0);
    CHECK(memcmp(&evidence, &previousEvidence, sizeof(evidence)) == 0);

    static const unsigned char invalidUtf8[] = {0xc0U, 0xafU, 'A', 'B'};
    char invalidHex[65] = {0};
    CHECK(UmiSha256Buffer(invalidUtf8, sizeof(invalidUtf8),
                          invalidHex) == UMI_STATUS_OK);
    review.preparedTextSha256 = invalidHex;
    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          invalidUtf8, sizeof(invalidUtf8),
          (UmiAiTrainingTextSpan){0U, 2U},
          (UmiAiTrainingTextSpan){2U, 2U},
          20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(json, previous, sizeof(json)) == 0);
    review.preparedTextSha256 = preparedHex;

    CHECK(UmiAiTrainingReviewedExampleJsonl(&review,
          original, sizeof(original) - 1U,
          prepared, sizeof(prepared) - 1U,
          (UmiAiTrainingTextSpan){10U, 27U}, answer,
          20U, json, sizeof(json), &produced,
          &evidence) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(json, previous, sizeof(json)) == 0);

    puts("Verified public-source JSONL, immutable outputs, and refusal gates");
    return 0;
}
