/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/import.h
 * PURPOSE: Capture, inspect and atomically save complete text documents as retrieval passages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_IMPORT_H
#define UMICOM_AI_WORKSPACE_IMPORT_H
#include "umicom/ai_workspace/workspace.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_AI_WORKSPACE_IMPORT_MAX_BYTES                                                                    \
    (UMI_AI_WORKSPACE_MAX_SOURCES * (UMI_AI_WORKSPACE_PASSAGE_CAPACITY - 1U))
    typedef struct UmiAiWorkspaceImport UmiAiWorkspaceImport;
    typedef struct UmiAiWorkspaceImportSummary
    {
        char collectionId[UMI_AI_WORKSPACE_ID_CAPACITY];
        char collectionTitle[UMI_AI_WORKSPACE_TITLE_CAPACITY];
        char documentId[UMI_AI_WORKSPACE_ID_CAPACITY];
        char title[UMI_AI_WORKSPACE_TITLE_CAPACITY];
        size_t inputBytes, textBytes, passageCount;
        uint64_t workspaceRevision, corpusRevision;
        bool createsCollection, saved;
    } UmiAiWorkspaceImportSummary;
    typedef struct UmiAiWorkspaceImportPassage
    {
        UmiAiWorkspaceSource source;
        size_t textOffset, byteCount;
    } UmiAiWorkspaceImportPassage;
    /* Copies explicit UTF-8 bytes; no file, model or database write occurs here.
 * A leading UTF-8 BOM is removed and CRLF/lone CR become LF in the copied text.
 * All remaining bytes are retained in ordered passages. A long physical line
 * may span passages with the same line number; Unicode characters never split.
 * IDs are documentId.part.N; documentId has at most 55 bytes. Existing passage
 * IDs and different existing collection titles are rejected, never overwritten.
 * The borrowed workspace must outlive operations using this capture. */
    UmiStatus UmiAiWorkspaceImportCreate(UmiAiWorkspace *workspace, const char *collectionId,
                                         const char *collectionTitle, const char *documentId,
                                         const char *title, const char *text, size_t byteCount,
                                         UmiAiWorkspaceImport **outImport);
    void UmiAiWorkspaceImportDestroy(UmiAiWorkspaceImport *import);
    UmiStatus UmiAiWorkspaceImportInspect(const UmiAiWorkspaceImport *import,
                                          UmiAiWorkspaceImportSummary *outSummary);
    UmiStatus UmiAiWorkspaceImportPassageAt(const UmiAiWorkspaceImport *import, size_t index,
                                            UmiAiWorkspaceImportPassage *outPassage);
    /* Explicit approval applies exactly this capture through the existing Data
 * Server transaction. A changed workspace, even a new job, requires recapture.
 * Disk writers are checked again by that transaction. Success creates one
 * corpus revision, invalidates cached embeddings and consumes the capture.
 * Existing jobs retain their frozen evidence. No provider is contacted. */
    UmiStatus UmiAiWorkspaceImportApply(UmiAiWorkspace *workspace, UmiAiWorkspaceImport *import,
                                        bool approved);
#ifdef __cplusplus
}
#endif
#endif
