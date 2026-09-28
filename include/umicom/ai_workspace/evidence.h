/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/evidence.h
 * PURPOSE: Freeze selected retrieval passages and inspect literal source references.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_WORKSPACE_EVIDENCE_H
#define UMICOM_AI_WORKSPACE_EVIDENCE_H
#include "umicom/ai_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_EVIDENCE_MAX_REFERENCES 128U
#define UMI_AI_EVIDENCE_REPORT_CAPACITY 32768U

/* Prepare a grounded job from an explicitly inspected ordered selection. This
 * is the bridge from canonical lexical/vector/reranker results to generation:
 * the implementation does NOT run a different lexical search afterwards.
 * Every supplied source must exactly match the current workspace, collection
 * and revision. Duplicate sources, invalid scores and stale corpus revisions
 * are rejected. Zero sources is an error, not an ungrounded fallback.
 *
 * The job uses the existing durable schema and review/run lifecycle. Ranking
 * scores remain transient (the existing schema does not persist them). They
 * are not part of request identity. The source order and exact source bytes
 * are part of identity. Neither the ranking model nor compiled provider bytes
 * are attested by this operation. All calls use the workspace owner thread. */
UmiStatus UmiAiWorkspacePrepareEvidence(UmiAiWorkspace *workspace,
    const char *jobId, const char *providerId, const char *modelId,
    const char *collectionId, const char *prompt, const char *requestedBy,
    uint32_t maxOutputTokens, uint64_t expectedCorpusRevision,
    const UmiAiWorkspaceEvidence *evidence, size_t evidenceCount);

typedef enum UmiAiEvidenceSourceState {
    UMI_AI_EVIDENCE_SOURCE_UNCHANGED = 1,
    UMI_AI_EVIDENCE_SOURCE_CHANGED,
    UMI_AI_EVIDENCE_SOURCE_REMOVED
} UmiAiEvidenceSourceState;

typedef struct UmiAiEvidenceSourceCheck {
    UmiAiWorkspaceSource frozen;
    UmiAiWorkspaceSource current; /* Zero when removed. Never a substituted citation. */
    UmiAiEvidenceSourceState state;
} UmiAiEvidenceSourceCheck;

typedef enum UmiAiEvidenceReferenceState {
    UMI_AI_EVIDENCE_REFERENCE_VALID = 1,
    UMI_AI_EVIDENCE_REFERENCE_MALFORMED,
    UMI_AI_EVIDENCE_REFERENCE_UNKNOWN
} UmiAiEvidenceReferenceState;

typedef struct UmiAiEvidenceReference {
    size_t byteOffset;
    size_t byteLength;
    size_t sourceNumber; /* 1-based; zero for malformed syntax. */
    UmiAiEvidenceReferenceState state;
    bool canonicalSpelling; /* Legacy [S01] resolves, but is not canonical [S1]. */
} UmiAiEvidenceReference;

typedef struct UmiAiEvidenceReferences {
    size_t count, validCount, malformedCount, unknownCount;
    size_t sourceUses[UMI_AI_WORKSPACE_MAX_EVIDENCE];
    UmiAiEvidenceReference items[UMI_AI_EVIDENCE_MAX_REFERENCES];
} UmiAiEvidenceReferences;

/* Inspect every literal "[S" prefix in bounded UTF-8 text. Markdown, code
 * fences and quotation are NOT interpreted. Missing or malformed references
 * are report data; exceeding capacity is an error with output unchanged.
 * A valid reference proves that a numbered source exists, NOT that the prose
 * is true, complete, authorised, or entailed by that source. */
UmiStatus UmiAiEvidenceScanReferences(const char *text, size_t evidenceCount,
    UmiAiEvidenceReferences *outReferences);

typedef struct UmiAiEvidenceSummary {
    uint64_t workspaceRevision, corpusRevision, preparedCorpusRevision;
    size_t sourceCount, changedSources, removedSources, unusedSources;
    size_t validReferences, malformedReferences, unknownReferences;
    bool corpusChanged, hasResponse, outputLengthLimited;
    bool referenceBindingsValid; /* Structural only; false until a response exists. */
    UmiAiWorkspaceJobKind kind;
    UmiAiWorkspaceJobState state;
} UmiAiEvidenceSummary;

typedef struct UmiAiEvidenceReview UmiAiEvidenceReview;

/* Capture only the currently loaded workspace on its owner thread. Does not
 * reload a database, run a provider/tool, approve a job or persist a decision.
 * The result owns all copied text and remains readable after its source closes.
 * Other threads may read an immutable review while its owner preserves its
 * lifetime; destruction must not race those reads. *outReview is NULL on error. */
UmiStatus UmiAiEvidenceCapture(const UmiAiWorkspace *workspace, const char *jobId,
    UmiAiEvidenceReview **outReview);
void UmiAiEvidenceDestroy(UmiAiEvidenceReview *review);
UmiStatus UmiAiEvidenceSummaryRead(const UmiAiEvidenceReview *review,
    UmiAiEvidenceSummary *outSummary);
UmiStatus UmiAiEvidenceJobRead(const UmiAiEvidenceReview *review,
    UmiAiWorkspaceJob *outJob);
UmiStatus UmiAiEvidenceSourceAt(const UmiAiEvidenceReview *review, size_t index,
    UmiAiEvidenceSourceCheck *outSource);
UmiStatus UmiAiEvidenceReferenceAt(const UmiAiEvidenceReview *review, size_t index,
    UmiAiEvidenceReference *outReference);

/* Copy an inclusive original-source line range from the FROZEN passage.
 * Newlines within the range, including its final newline when present, are
 * retained byte-for-byte. Never reads the source file or replaces old text
 * with a newer source. On failure, output and optional outBytes are unchanged. */
UmiStatus UmiAiEvidenceCopyLines(const UmiAiEvidenceReview *review, size_t sourceIndex,
    uint32_t firstLine, uint32_t lastLine, char *output, size_t capacity,
    size_t *outBytes);

/* Plain UTF-8, no HTML or terminal commands. Caller buffer is unchanged on any
 * error. source and response text may contain private or untrusted material:
 * review/redact it before sharing. There is no automatic telemetry. */
UmiStatus UmiAiEvidenceFormat(const UmiAiEvidenceReview *review,
    char *output, size_t capacity, size_t *outBytes);
const char *UmiAiEvidenceSourceStateText(UmiAiEvidenceSourceState state);
#ifdef __cplusplus
}
#endif
#endif
