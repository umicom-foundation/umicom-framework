/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/evidence_review.c
 * PURPOSE: Own immutable citation/source inspections without granting execution authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "evidence_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiEvidenceScanReferences(const char *text, size_t evidenceCount,
    UmiAiEvidenceReferences *outReferences)
{
    if (outReferences == NULL || evidenceCount > UMI_AI_WORKSPACE_MAX_EVIDENCE ||
        !AwTextValid(text, UMI_AI_TEXT_CAPACITY, true)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiEvidenceReferences result = {0};
    size_t length = strlen(text);
    for (size_t i = 0U; i + 1U < length; ++i) {
        if (text[i] != '[' || text[i + 1U] != 'S') continue;
        if (result.count == UMI_AI_EVIDENCE_MAX_REFERENCES) return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiAiEvidenceReference *item = &result.items[result.count++];
        size_t cursor = i + 2U, number = 0U, digits = 0U;
        while (cursor < length && text[cursor] >= '0' && text[cursor] <= '9') {
            if (digits < 2U) number = number * 10U + (size_t)(text[cursor] - '0');
            ++digits; ++cursor;
        }
        bool closed = cursor < length && text[cursor] == ']';
        item->byteOffset = i; item->byteLength = cursor - i + (closed ? 1U : 0U);
        if (!closed || digits == 0U || digits > 2U || number == 0U) {
            item->state = UMI_AI_EVIDENCE_REFERENCE_MALFORMED; ++result.malformedCount;
        } else {
            item->sourceNumber = number;
            item->canonicalSpelling = !(digits > 1U && text[i + 2U] == '0');
            if (number > evidenceCount) {
                item->state = UMI_AI_EVIDENCE_REFERENCE_UNKNOWN; ++result.unknownCount;
            } else {
                item->state = UMI_AI_EVIDENCE_REFERENCE_VALID; ++result.validCount;
                ++result.sourceUses[number - 1U];
            }
        }
        /* Continue at the first unconsumed byte. In particular, [S[S1] must
         * report the malformed outer prefix AND the nested literal reference. */
        i = cursor - 1U + (closed ? 1U : 0U);
    }
    *outReferences = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiEvidenceCapture(const UmiAiWorkspace *workspace, const char *jobId,
    UmiAiEvidenceReview **outReview)
{
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(jobId, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = AwJobIndex(workspace->state, jobId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiAiWorkspaceJob *job = &workspace->state->jobs[index];
    if (job->evidenceCount > UMI_AI_WORKSPACE_MAX_EVIDENCE) return UMI_STATUS_INVALID_STATE;
    UmiAiEvidenceReview *review = calloc(1U, sizeof *review);
    if (review == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    review->job = *job;
    UmiAiEvidenceSummary *summary = &review->summary;
    summary->workspaceRevision = workspace->state->revision;
    summary->corpusRevision = workspace->state->corpusRevision;
    summary->preparedCorpusRevision = job->corpusRevision;
    summary->sourceCount = job->evidenceCount;
    summary->kind = job->kind; summary->state = job->state;
    summary->corpusChanged = summary->corpusRevision != job->corpusRevision;
    summary->hasResponse = job->state == UMI_AI_WORKSPACE_SUCCEEDED && job->response.text[0] != '\0';
    summary->outputLengthLimited = summary->hasResponse && job->response.finish_reason == UMI_AI_FINISH_LENGTH;
    status = UmiAiEvidenceScanReferences(summary->hasResponse ? job->response.text : "",
        job->evidenceCount, &review->references);
    if (status != UMI_STATUS_OK) { free(review); return status; }
    summary->validReferences = review->references.validCount;
    summary->malformedReferences = review->references.malformedCount;
    summary->unknownReferences = review->references.unknownCount;
    summary->referenceBindingsValid = summary->hasResponse && job->kind != UMI_AI_WORKSPACE_TOOL &&
        summary->malformedReferences == 0U && summary->unknownReferences == 0U &&
        (job->evidenceCount == 0U || summary->validReferences != 0U);
    for (size_t i = 0U; i < job->evidenceCount; ++i) {
        UmiAiEvidenceSourceCheck *check = &review->sources[i];
        check->frozen = job->evidence[i].source;
        size_t current = AwSourceIndex(workspace->state, check->frozen.id);
        if (current == SIZE_MAX) {
            check->state = UMI_AI_EVIDENCE_SOURCE_REMOVED; ++summary->removedSources;
        } else {
            check->current = workspace->state->sources[current];
            check->state = AwEvidenceSourceEqual(&check->frozen, &check->current) ?
                UMI_AI_EVIDENCE_SOURCE_UNCHANGED : UMI_AI_EVIDENCE_SOURCE_CHANGED;
            if (check->state == UMI_AI_EVIDENCE_SOURCE_CHANGED) ++summary->changedSources;
        }
        if (review->references.sourceUses[i] == 0U) ++summary->unusedSources;
    }
    *outReview = review;
    return UMI_STATUS_OK;
}
void UmiAiEvidenceDestroy(UmiAiEvidenceReview *review) { free(review); }
UmiStatus UmiAiEvidenceSummaryRead(const UmiAiEvidenceReview *review, UmiAiEvidenceSummary *outSummary)
{
    if (review == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = review->summary; return UMI_STATUS_OK;
}
UmiStatus UmiAiEvidenceJobRead(const UmiAiEvidenceReview *review, UmiAiWorkspaceJob *outJob)
{
    if (review == NULL || outJob == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outJob = review->job; return UMI_STATUS_OK;
}
UmiStatus UmiAiEvidenceSourceAt(const UmiAiEvidenceReview *review, size_t index, UmiAiEvidenceSourceCheck *outSource)
{
    if (review == NULL || outSource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->summary.sourceCount) return UMI_STATUS_NOT_FOUND;
    *outSource = review->sources[index]; return UMI_STATUS_OK;
}
UmiStatus UmiAiEvidenceReferenceAt(const UmiAiEvidenceReview *review, size_t index, UmiAiEvidenceReference *outReference)
{
    if (review == NULL || outReference == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->references.count) return UMI_STATUS_NOT_FOUND;
    *outReference = review->references.items[index]; return UMI_STATUS_OK;
}
UmiStatus UmiAiEvidenceCopyLines(const UmiAiEvidenceReview *review, size_t sourceIndex,
    uint32_t firstLine, uint32_t lastLine, char *output, size_t capacity, size_t *outBytes)
{
    if (review == NULL || output == NULL || capacity == 0U || firstLine > lastLine)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (sourceIndex >= review->summary.sourceCount) return UMI_STATUS_NOT_FOUND;
    const UmiAiWorkspaceSource *source = &review->sources[sourceIndex].frozen;
    if (firstLine < source->firstLine || lastLine > source->lastLine) return UMI_STATUS_NOT_FOUND;
    uint64_t line = source->firstLine;
    size_t start = 0U, end = strlen(source->text), cursor = 0U;
    while (source->text[cursor] != '\0') {
        if (line == firstLine) { start = cursor; break; }
        if (source->text[cursor++] == '\n') ++line;
    }
    for (; source->text[cursor] != '\0'; ++cursor) {
        if (source->text[cursor] == '\n') {
            if (line == lastLine) { end = cursor + 1U; break; }
            ++line;
        }
    }
    size_t bytes = end - start;
    if (bytes >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, source->text + start, bytes); output[bytes] = '\0';
    if (outBytes != NULL) *outBytes = bytes;
    return UMI_STATUS_OK;
}
const char *UmiAiEvidenceSourceStateText(UmiAiEvidenceSourceState state)
{
    switch (state) {
    case UMI_AI_EVIDENCE_SOURCE_UNCHANGED: return "Unchanged in the loaded workspace";
    case UMI_AI_EVIDENCE_SOURCE_CHANGED: return "Changed since preparation";
    case UMI_AI_EVIDENCE_SOURCE_REMOVED: return "Removed from the loaded workspace";
    default: return "Unknown source state";
    }
}
