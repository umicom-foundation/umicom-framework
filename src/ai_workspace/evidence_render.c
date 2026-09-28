/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/evidence_render.c
 * PURPOSE: Explain frozen AI evidence without presenting citation syntax as truth.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "evidence_internal.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct EvidenceText { char *text; size_t used; UmiStatus status; } EvidenceText;
static void Append(EvidenceText *buffer, const char *format, ...)
{
    if (buffer->status != UMI_STATUS_OK) return;
    va_list arguments; va_start(arguments, format);
    int written = vsnprintf(buffer->text + buffer->used,
        UMI_AI_EVIDENCE_REPORT_CAPACITY - buffer->used, format, arguments);
    va_end(arguments);
    if (written < 0 || (size_t)written >= UMI_AI_EVIDENCE_REPORT_CAPACITY - buffer->used) {
        buffer->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    buffer->used += (size_t)written;
}
UmiStatus UmiAiEvidenceFormat(const UmiAiEvidenceReview *review,
    char *output, size_t capacity, size_t *outBytes)
{
    if (review == NULL || output == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    EvidenceText buffer = {calloc(UMI_AI_EVIDENCE_REPORT_CAPACITY, 1U), 0U, UMI_STATUS_OK};
    if (buffer.text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    const UmiAiEvidenceSummary *summary = &review->summary;
    const UmiAiWorkspaceJob *job = &review->job;
    Append(&buffer, "AI source and citation inspection\nJob: %s\nState: %s\n"
        "Provider: %s\nRequested model: %s\nLoaded workspace revision: %" PRIu64
        "\nPrepared corpus: %" PRIu64 " | loaded corpus: %" PRIu64 "\n",
        job->id, UmiAiWorkspaceJobStateText(job->state), job->providerId, job->modelId,
        summary->workspaceRevision, summary->preparedCorpusRevision, summary->corpusRevision);
    Append(&buffer, "\nThis is a snapshot of already loaded records, not a database refresh.\n"
        "Reference existence is not proof of truth, entailment or approval.\n"
        "The original file, model binary and writer identity are not authenticated here.\n"
        "Frozen prompt:\n%s\n", job->prompt);
    Append(&buffer, "\nResponse present: %s | output length limit reached: %s\n"
        "Known references: %zu | malformed: %zu | unknown: %zu\n"
        "Supplied sources: %zu | changed: %zu | removed: %zu | not cited: %zu\n",
        summary->hasResponse ? "yes" : "no", summary->outputLengthLimited ? "yes" : "no",
        summary->validReferences, summary->malformedReferences, summary->unknownReferences,
        summary->sourceCount, summary->changedSources, summary->removedSources, summary->unusedSources);
    if (summary->corpusChanged)
        Append(&buffer, "The loaded corpus changed after preparation. Frozen passages below are retained, not refreshed in place.\n");
    if (job->kind == UMI_AI_WORKSPACE_TOOL)
        Append(&buffer, "Tool output is not a grounded draft; reference inspection grants no tool permission.\n");
    for (size_t i = 0U; i < summary->sourceCount; ++i) {
        const UmiAiEvidenceSourceCheck *source = &review->sources[i];
        Append(&buffer, "\n[S%zu] %s | %s\nStatus: %s\n"
            "Frozen lines %u-%u | source revision %" PRIu64 "\n%s\n",
            i + 1U, source->frozen.id, source->frozen.title,
            UmiAiEvidenceSourceStateText(source->state), source->frozen.firstLine,
            source->frozen.lastLine, source->frozen.revision, source->frozen.text);
        if (source->state == UMI_AI_EVIDENCE_SOURCE_CHANGED)
            Append(&buffer, "Current comparison only (NOT the cited passage): %s | %s\n"
                "Lines %u-%u | revision %" PRIu64 "\n%s\n",
                source->current.collectionId, source->current.title, source->current.firstLine,
                source->current.lastLine, source->current.revision, source->current.text);
    }
    if (summary->hasResponse) Append(&buffer, "\nReturned draft (unverified prose):\n%s\n", job->response.text);
    Append(&buffer, "\nLiteral reference locations (zero-based UTF-8 byte offsets):\n");
    for (size_t i = 0U; i < review->references.count; ++i) {
        const UmiAiEvidenceReference *reference = &review->references.items[i];
        Append(&buffer, "offset %zu, length %zu: source %zu, %s%s\n",
            reference->byteOffset, reference->byteLength, reference->sourceNumber,
            reference->state == UMI_AI_EVIDENCE_REFERENCE_VALID ? "exists" :
            reference->state == UMI_AI_EVIDENCE_REFERENCE_UNKNOWN ? "unknown source" : "malformed",
            reference->state == UMI_AI_EVIDENCE_REFERENCE_VALID && !reference->canonicalSpelling ?
                " (legacy leading-zero spelling)" : "");
    }
    UmiStatus status = buffer.status;
    if (status == UMI_STATUS_OK && buffer.used >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        memcpy(output, buffer.text, buffer.used + 1U);
        if (outBytes != NULL) *outBytes = buffer.used;
    }
    free(buffer.text); return status;
}
