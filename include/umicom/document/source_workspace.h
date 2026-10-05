/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/source_workspace.h
 * PURPOSE: Capture all source dependencies before asynchronous work without acquiring edit permission.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SOURCE_WORKSPACE_H
#define UMICOM_DOCUMENT_SOURCE_WORKSPACE_H
#include "umicom/document/source_request.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM 64U
#define UMI_DOCUMENT_SOURCE_WORKSPACE_BYTE_BUDGET (64U * 1024U * 1024U)
    typedef struct UmiDocumentSourceWorkspace UmiDocumentSourceWorkspace;
    /* Capture an explicit, nonempty set of open document IDs on the coordinator's
 * owning thread. Read-only documents are useful dependencies and are accepted.
 * Every member must have a distinct, nonempty exact URI. This is not filesystem
 * alias resolution. Capture no more than 64 documents and 64 MiB of draft text;
 * the existing 8 MiB per-document limit still applies. No file is read or saved.
 *
 * Keep the same coordinator, workbench and store alive through Check. Copy
 * metadata and text for worker use; access this owner only on its original
 * thread. The capture is immutable, owns its source requests and grants no
 * editing permission. Failure clears the output. Destroy after use. */
    UmiStatus UmiDocumentSourceWorkspaceCreate(UmiDocumentCoordinator *coordinator,
                                               const UmiDocumentId *document_ids, size_t count,
                                               UmiDocumentSourceWorkspace **out_workspace);
    void UmiDocumentSourceWorkspaceDestroy(UmiDocumentSourceWorkspace *workspace);
    size_t UmiDocumentSourceWorkspaceCount(const UmiDocumentSourceWorkspace *workspace);
    UmiStatus UmiDocumentSourceWorkspaceAt(const UmiDocumentSourceWorkspace *workspace, size_t index,
                                           UmiDocumentSourceRequestSummary *out_document);
    /* Borrow immutable text until Destroy. Both outputs clear on failure. */
    UmiStatus UmiDocumentSourceWorkspaceRead(const UmiDocumentSourceWorkspace *workspace, size_t index,
                                             const char **out_text, size_t *out_bytes);
    /* Exact URI lookup. Missing output is SIZE_MAX, including on invalid input. */
    UmiStatus UmiDocumentSourceWorkspaceFind(const UmiDocumentSourceWorkspace *workspace,
                                             const char *document_uri, size_t *out_index);
    /* Check every captured dependency, including documents a proposed edit does
 * not change. Source/save revisions, identity, permissions, conflicts and
 * caret/selection must still match. A changed active tab alone is acceptable.
 * Newly opened documents are outside this explicit capture. */
    UmiStatus UmiDocumentSourceWorkspaceCheck(UmiDocumentCoordinator *coordinator,
                                              const UmiDocumentSourceWorkspace *workspace);
#ifdef __cplusplus
}
#endif
#endif
