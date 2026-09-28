/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Shared exact-source comparison. No native-structure padding is compared. */
#ifndef UMICOM_AI_EVIDENCE_INTERNAL_H
#define UMICOM_AI_EVIDENCE_INTERNAL_H
#include "workspace_internal.h"
#include "umicom/ai_workspace/evidence.h"
struct UmiAiEvidenceReview {
    UmiAiEvidenceSummary summary;
    UmiAiWorkspaceJob job;
    UmiAiEvidenceSourceCheck sources[UMI_AI_WORKSPACE_MAX_EVIDENCE];
    UmiAiEvidenceReferences references;
};
bool AwEvidenceSourceEqual(const UmiAiWorkspaceSource *a, const UmiAiWorkspaceSource *b);
UmiStatus AwEvidenceSelectionCheck(const UmiAiWorkspace *workspace,
    const char *collection, uint64_t corpusRevision,
    const UmiAiWorkspaceEvidence *evidence, size_t count);
#endif
