/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/reviewed_examples.c
 * PURPOSE:
 *   Encode source-verified, reviewed prompt/completion spans as one portable
 *   UTF-8 JSONL record, with private provenance returned out of band.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/reviewed_examples.h"
#include "umicom/base/sha256.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int ReviewedContinuation(unsigned char value)
{
    return value >= 0x80U && value <= 0xbfU;
}

/* Validate complete RFC 3629 UTF-8 without normalising, interpreting markup,
 * or silently replacing a malformed byte. Validation covers ALL supplied
 * reviewed text, not only selected spans. NUL is excluded for text training. */
static int ReviewedUtf8Valid(const unsigned char *text, size_t length)
{
    size_t i = 0U;
    while (i < length) {
        unsigned char c = text[i];
        if (c == 0U) return 0;
        if (c < 0x80U) { ++i; continue; }
        if (c >= 0xc2U && c <= 0xdfU) {
            if (length - i < 2U || !ReviewedContinuation(text[i + 1U])) return 0;
            i += 2U;
        } else if (c >= 0xe0U && c <= 0xefU) {
            if (length - i < 3U ||
                !ReviewedContinuation(text[i + 1U]) ||
                !ReviewedContinuation(text[i + 2U]) ||
                (c == 0xe0U && text[i + 1U] < 0xa0U) ||
                (c == 0xedU && text[i + 1U] > 0x9fU)) return 0;
            i += 3U;
        } else if (c >= 0xf0U && c <= 0xf4U) {
            if (length - i < 4U ||
                !ReviewedContinuation(text[i + 1U]) ||
                !ReviewedContinuation(text[i + 2U]) ||
                !ReviewedContinuation(text[i + 3U]) ||
                (c == 0xf0U && text[i + 1U] < 0x90U) ||
                (c == 0xf4U && text[i + 1U] > 0x8fU)) return 0;
            i += 4U;
        } else {
            return 0;
        }
    }
    return 1;
}

static int ReviewedSpanValid(UmiAiTrainingTextSpan span, size_t total)
{
    return span.byteCount > 0U &&
           span.byteCount <= UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES &&
           span.offset <= total && span.byteCount <= total - span.offset;
}

/* Count the exact JSON-escaped bytes, excluding the surrounding quotes.
 * Each control byte has a canonical escape; valid non-ASCII UTF-8 is retained. */
static size_t ReviewedEscapedLength(const unsigned char *text, size_t length)
{
    size_t total = 0U;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = text[i];
        if (c == '"' || c == '\\' || c == '\n' || c == '\r' || c == '\t')
            total += 2U;
        else if (c < 0x20U)
            total += 6U;
        else
            ++total;
    }
    return total;
}

static size_t ReviewedEscapeWrite(char *output,
                                  const unsigned char *text, size_t length)
{
    static const char hex[] = "0123456789abcdef";
    size_t used = 0U;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = text[i];
        if (c == '"' || c == '\\') {
            output[used++] = '\\';
            output[used++] = (char)c;
        } else if (c == '\n' || c == '\r' || c == '\t') {
            output[used++] = '\\';
            output[used++] = c == '\n' ? 'n' : c == '\r' ? 'r' : 't';
        } else if (c < 0x20U) {
            output[used++] = '\\';
            output[used++] = 'u';
            output[used++] = '0';
            output[used++] = '0';
            output[used++] = hex[c >> 4U];
            output[used++] = hex[c & 15U];
        } else {
            output[used++] = (char)c;
        }
    }
    return used;
}

UmiStatus UmiAiTrainingReviewedExampleJsonl(
    const UmiAiTrainingSourceReview *review,
    const void *originalAsset, size_t originalLength,
    const void *preparedUtf8, size_t preparedLength,
    UmiAiTrainingTextSpan prompt,
    UmiAiTrainingTextSpan completion,
    unsigned evaluationPercentage,
    char *output, size_t outputCapacity,
    size_t *outLength,
    UmiAiTrainingReviewedExampleEvidence *outEvidence)
{
    static const char prefix[] = "{\"prompt\":\"";
    static const char middle[] = "\",\"completion\":\"";
    static const char suffix[] = "\"}\n";
    const unsigned char *text = preparedUtf8;
    UmiAiTrainingReviewedExampleEvidence evidence = {0};
    char *candidate = NULL;
    size_t promptBytes, completionBytes, required, used = 0U;
    UmiStatus status;

    if (review == NULL || originalAsset == NULL || originalLength == 0U ||
        text == NULL || preparedLength == 0U || output == NULL ||
        outLength == NULL || outEvidence == NULL ||
        !ReviewedSpanValid(prompt, preparedLength) ||
        !ReviewedSpanValid(completion, preparedLength) ||
        prompt.offset + prompt.byteCount > completion.offset)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The source owner checks actual SHA-256 bytes, rights, visibility,
     * extraction review and common-asset train/evaluation partitioning. */
    status = UmiAiTrainingSourceVerifyBytes(
        review, originalAsset, originalLength, preparedUtf8, preparedLength,
        evaluationPercentage, &evidence.admittedSource);
    if (status != UMI_STATUS_OK) return status;
    if (!ReviewedUtf8Valid(text, preparedLength)) return UMI_STATUS_PARSE_ERROR;
    /* Even when the whole derivative is UTF-8, slicing inside a multibyte
     * character would produce invalid JSON text. Require codepoint edges. */
    if ((prompt.offset != 0U && ReviewedContinuation(text[prompt.offset])) ||
        (completion.offset != 0U && ReviewedContinuation(text[completion.offset])) ||
        (prompt.offset + prompt.byteCount < preparedLength &&
         ReviewedContinuation(text[prompt.offset + prompt.byteCount])) ||
        (completion.offset + completion.byteCount < preparedLength &&
         ReviewedContinuation(text[completion.offset + completion.byteCount])))
        return UMI_STATUS_INVALID_ARGUMENT;

    promptBytes = ReviewedEscapedLength(text + prompt.offset, prompt.byteCount);
    completionBytes = ReviewedEscapedLength(text + completion.offset,
                                           completion.byteCount);
    required = sizeof(prefix) - 1U + sizeof(middle) - 1U +
               sizeof(suffix) - 1U + promptBytes + completionBytes;
    if (required == SIZE_MAX || outputCapacity <= required)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = malloc(required + 1U);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    memcpy(candidate + used, prefix, sizeof(prefix) - 1U);
    used += sizeof(prefix) - 1U;
    used += ReviewedEscapeWrite(candidate + used,
                               text + prompt.offset, prompt.byteCount);
    memcpy(candidate + used, middle, sizeof(middle) - 1U);
    used += sizeof(middle) - 1U;
    used += ReviewedEscapeWrite(candidate + used,
                               text + completion.offset, completion.byteCount);
    memcpy(candidate + used, suffix, sizeof(suffix) - 1U);
    used += sizeof(suffix) - 1U;
    candidate[used] = '\0';
    status = UmiSha256Buffer(candidate, used, evidence.jsonSha256);
    if (status == UMI_STATUS_OK) {
        evidence.prompt = prompt;
        evidence.completion = completion;
        evidence.jsonBytes = used;
        memcpy(output, candidate, used + 1U);
        *outLength = used;
        *outEvidence = evidence;
    }
    free(candidate);
    return status;
}
