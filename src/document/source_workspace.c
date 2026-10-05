/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/source_workspace.c
 * PURPOSE: Keep immutable source dependencies under the existing document capture and stale-state rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/source_workspace.h"
#include <stdlib.h>
#include <string.h>
struct UmiDocumentSourceWorkspace
{
    UmiDocumentCoordinator *owner;
    UmiDocumentSourceRequest *requests[UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM];
    size_t count, source_bytes;
};
void UmiDocumentSourceWorkspaceDestroy(UmiDocumentSourceWorkspace *workspace)
{
    if (workspace == NULL)
        return;
    for (size_t i = 0U; i < workspace->count; ++i)
        UmiDocumentSourceRequestDestroy(workspace->requests[i]);
    free(workspace);
}
size_t UmiDocumentSourceWorkspaceCount(const UmiDocumentSourceWorkspace *workspace)
{
    return workspace != NULL ? workspace->count : 0U;
}
UmiStatus UmiDocumentSourceWorkspaceAt(const UmiDocumentSourceWorkspace *workspace, size_t index,
                                       UmiDocumentSourceRequestSummary *out_document)
{
    if (workspace == NULL || out_document == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->count)
        return UMI_STATUS_NOT_FOUND;
    return UmiDocumentSourceRequestInspect(workspace->requests[index], out_document);
}
UmiStatus UmiDocumentSourceWorkspaceRead(const UmiDocumentSourceWorkspace *workspace, size_t index,
                                         const char **out_text, size_t *out_bytes)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (workspace == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->count)
        return UMI_STATUS_NOT_FOUND;
    return UmiDocumentSourceRequestRead(workspace->requests[index], out_text, out_bytes);
}
UmiStatus UmiDocumentSourceWorkspaceFind(const UmiDocumentSourceWorkspace *workspace,
                                         const char *document_uri, size_t *out_index)
{
    if (out_index == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = SIZE_MAX;
    if (workspace == NULL || document_uri == NULL || document_uri[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < workspace->count; ++i)
    {
        UmiDocumentSourceRequestSummary document;
        UmiStatus status = UmiDocumentSourceRequestInspect(workspace->requests[i], &document);
        if (status != UMI_STATUS_OK)
            return status;
        if (strcmp(document.uri, document_uri) == 0)
        {
            *out_index = i;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiDocumentSourceWorkspaceCheck(UmiDocumentCoordinator *coordinator,
                                          const UmiDocumentSourceWorkspace *workspace)
{
    if (coordinator == NULL || workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator != workspace->owner)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < workspace->count; ++i)
    {
        UmiStatus status = UmiDocumentSourceRequestCheck(coordinator, workspace->requests[i]);
        if (status != UMI_STATUS_OK)
            return status;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceWorkspaceCreate(UmiDocumentCoordinator *coordinator,
                                           const UmiDocumentId *document_ids, size_t count,
                                           UmiDocumentSourceWorkspace **out_workspace)
{
    if (out_workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_workspace = NULL;
    if (coordinator == NULL || document_ids == NULL || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i)
    {
        if (document_ids[i] == 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t prior = 0U; prior < i; ++prior)
            if (document_ids[i] == document_ids[prior])
                return UMI_STATUS_ALREADY_EXISTS;
    }
    UmiDocumentSourceWorkspace *workspace = calloc(1U, sizeof(*workspace));
    if (workspace == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    workspace->owner = coordinator;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDocumentSourceRequest *request = NULL;
        UmiDocumentSourceRequestSummary document;
        status = UmiDocumentSourceRequestCreateInspection(coordinator, document_ids[i], &request);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceRequestInspect(request, &document);
        if (status == UMI_STATUS_OK && document.uri[0] == '\0')
            status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK &&
            document.source_bytes > UMI_DOCUMENT_SOURCE_WORKSPACE_BYTE_BUDGET - workspace->source_bytes)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK)
        {
            size_t duplicate = SIZE_MAX;
            UmiStatus lookup = UmiDocumentSourceWorkspaceFind(workspace, document.uri, &duplicate);
            if (lookup == UMI_STATUS_OK)
                status = UMI_STATUS_ALREADY_EXISTS;
            else if (lookup != UMI_STATUS_NOT_FOUND)
                status = lookup;
        }
        if (status != UMI_STATUS_OK)
        {
            UmiDocumentSourceRequestDestroy(request);
            break;
        }
        workspace->requests[workspace->count++] = request;
        workspace->source_bytes += document.source_bytes;
    }
    /* One final check closes the capture boundary: a dependency that changed
     * while gathering another document cannot become an accepted baseline. */
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceWorkspaceCheck(coordinator, workspace);
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentSourceWorkspaceDestroy(workspace);
        return status;
    }
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}
