/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/source_change.h
 * PURPOSE: Review and atomically replace or remove explicitly selected retrieval passages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_SOURCE_CHANGE_H
#define UMICOM_AI_WORKSPACE_SOURCE_CHANGE_H
#include "umicom/ai_workspace/import.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiAiWorkspaceSourceChange UmiAiWorkspaceSourceChange;
    typedef enum UmiAiWorkspaceSourceChangeKind
    {
        UMI_AI_WORKSPACE_SOURCE_REPLACE = 1,
        UMI_AI_WORKSPACE_SOURCE_REMOVE = 2
    } UmiAiWorkspaceSourceChangeKind;
    typedef struct UmiAiWorkspaceSourceChangeSummary
    {
        UmiAiWorkspaceSourceChangeKind kind;
        size_t beforeCount, afterCount;
        uint64_t workspaceRevision, corpusRevision;
        bool createsCollection, hasChanges, applied;
    } UmiAiWorkspaceSourceChangeSummary;
    /* Capture exactly the supplied unique IDs, never a prefix or inferred document
     * group. At least one existing passage must be selected. All IDs and texts are
     * copied; later caller edits cannot change the capture. The borrowed workspace
     * must outlive Check/Apply. These operations use the workspace's owner thread.
     * Replacement uses the complete UTF-8 import rules and documentId.part.N IDs.
     * IDs belonging to selected passages may be reused; all other collisions fail.
     * Collection creation, removals and replacements commit together. No original
     * file, existing job evidence or provider is changed by either operation. */
    UmiStatus UmiAiWorkspaceSourceReplacementCreate(UmiAiWorkspace *workspace, const char *const *sourceIds,
                                                    size_t sourceCount, const char *collectionId,
                                                    const char *collectionTitle, const char *documentId,
                                                    const char *title, const char *text, size_t byteCount,
                                                    UmiAiWorkspaceSourceChange **outChange);
    UmiStatus UmiAiWorkspaceSourceRemovalCreate(UmiAiWorkspace *workspace, const char *const *sourceIds,
                                                size_t sourceCount, UmiAiWorkspaceSourceChange **outChange);
    void UmiAiWorkspaceSourceChangeDestroy(UmiAiWorkspaceSourceChange *change);
    UmiStatus UmiAiWorkspaceSourceChangeInspect(const UmiAiWorkspaceSourceChange *change,
                                                UmiAiWorkspaceSourceChangeSummary *outSummary);
    UmiStatus UmiAiWorkspaceSourceChangeBeforeAt(const UmiAiWorkspaceSourceChange *change, size_t index,
                                                 UmiAiWorkspaceSource *outSource);
    UmiStatus UmiAiWorkspaceSourceChangeAfterAt(const UmiAiWorkspaceSourceChange *change, size_t index,
                                                UmiAiWorkspaceSource *outSource);
    /* Check detects changes in this workspace. Apply also compares the persisted
     * revision inside the Data Server transaction. Any intervening workspace edit,
     * including job preparation, requires a fresh capture. Explicit approval is
     * required even for removal. A successful capture cannot be applied twice.
     * Identical replacements consume the review without creating a new revision;
     * stored state is still checked. A changed corpus clears cached embeddings.
     * Search uses the new passages; saved jobs keep their original cited text.
     * Removing sources is not secure erasure of saved jobs or database backups. */
    UmiStatus UmiAiWorkspaceSourceChangeCheck(UmiAiWorkspace *workspace,
                                              const UmiAiWorkspaceSourceChange *change);
    UmiStatus UmiAiWorkspaceSourceChangeApply(UmiAiWorkspace *workspace, UmiAiWorkspaceSourceChange *change,
                                              bool approved);
#ifdef __cplusplus
}
#endif
#endif
