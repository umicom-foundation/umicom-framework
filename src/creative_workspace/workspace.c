/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/workspace.c
 * PURPOSE: Own committed state, optimistic concurrency and bounded undo/redo history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus Allocate(UmiDataServer * server, UmiCreativeWorkspace * * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * out = NULL;
    if (server == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeWorkspace * w = calloc(1U, sizeof( * w));
    if (w == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    w -> history = calloc(UMI_CREATIVE_HISTORY_CAPACITY + 1U, sizeof( * w -> history));
    if (w -> history == NULL){
        free(w);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    w -> server = server;
    * out = w;
    return UMI_STATUS_OK;
}

static void ResetHistory(UmiCreativeWorkspace * w)
{
    memset(w -> history, 0, (UMI_CREATIVE_HISTORY_CAPACITY + 1U) * sizeof( * w -> history));
    w -> history[0] = w -> current;
    w -> historyCount = 1U;
    w -> historyCursor = 0U;
}

UmiStatus UmiCreativeWorkspaceCreate(UmiDataServer * server, const UmiCreativeProject * project, UmiCreativeWorkspace * * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * out = NULL;
    if (UmiCreativeProjectValidate(project)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeWorkspace * w = NULL;
    UmiStatus status = Allocate(server, & w);
    if (status!= UMI_STATUS_OK)return status;
    w -> current = * project;
    w -> current.revision = 1U;
    status = UmiCreativeStoreSave(server, NULL, & w -> current, & w -> recoveryRequired);
    if (status!= UMI_STATUS_OK){
        UmiCreativeWorkspaceDestroy(w);
        return status;
    }
    ResetHistory(w);
    * out = w;
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeWorkspaceOpen(UmiDataServer * server, const char * id, UmiCreativeWorkspace * * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * out = NULL;
    if (!UmiCreativeIdValid(id))return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeWorkspace * w = NULL;
    UmiStatus status = Allocate(server, & w);
    if (status!= UMI_STATUS_OK)return status;
    status = UmiCreativeStoreLoad(server, id, & w -> current, & w -> recoveryRequired);
    if (status!= UMI_STATUS_OK){
        UmiCreativeWorkspaceDestroy(w);
        return status;
    }
    ResetHistory(w);
    * out = w;
    return UMI_STATUS_OK;
}

void UmiCreativeWorkspaceDestroy(UmiCreativeWorkspace * w)
{
    if (w == NULL)return;
    free(w -> history);
    free(w);
}

UmiStatus UmiCreativeWorkspaceCopy(const UmiCreativeWorkspace * w, UmiCreativeProject * out)
{
    if (w == NULL || out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if (w -> recoveryRequired)return UMI_STATUS_INVALID_STATE;
    * out = w -> current;
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeWorkspaceReload(UmiCreativeWorkspace * w)
{
    if (w == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if (w -> busy)return UMI_STATUS_BUSY;
    if (w -> recoveryRequired)return UMI_STATUS_INVALID_STATE;
    w -> busy = true;
    UmiStatus status = UmiCreativeStoreLoad(w -> server, w -> current.id, & w -> current, & w -> recoveryRequired);
    w -> busy = false;
    if (status == UMI_STATUS_OK)ResetHistory(w);
    return status;
}

static UmiStatus Ready(UmiCreativeWorkspace * w, uint64_t revision)
{
    if (w == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if (w -> recoveryRequired)return UMI_STATUS_INVALID_STATE;
    if (w -> busy || revision!= w -> current.revision)return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}

static UmiStatus Commit(UmiCreativeWorkspace * w, UmiCreativeProject * next)
{
    if (w -> current.revision == UINT64_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;
    next -> revision = w -> current.revision + 1U;
    w -> busy = true;
    UmiStatus status = UmiCreativeStoreSave(w -> server, & w -> current, next, & w -> recoveryRequired);
    w -> busy = false;
    if (status == UMI_STATUS_OK)w -> current = * next;
    return status;
}

UmiStatus UmiCreativeWorkspaceReplace(UmiCreativeWorkspace * w, const UmiCreativeProject * candidate, uint64_t revision)
{
    UmiStatus status = Ready(w, revision);
    if (status!= UMI_STATUS_OK)return status;
    if (UmiCreativeProjectValidate(candidate)!= UMI_STATUS_OK || strcmp(candidate -> id, w -> current.id)!= 0)return UMI_STATUS_INVALID_ARGUMENT;
    if (candidate -> revision!= revision)return UMI_STATUS_BUSY;
    if (w -> current.elementCount!= 0U && candidate -> elementCount!= 0U && w -> current.settings.unit!= candidate -> settings.unit)return UMI_STATUS_INVALID_STATE;
    if (UmiCreativeProjectEqual(candidate, & w -> current)){
        w -> busy = true;
        status = UmiCreativeStoreSave(w -> server, & w -> current, & w -> current, & w -> recoveryRequired);
        w -> busy = false;
        return status;
    }
    UmiCreativeProject * next = malloc(sizeof( * next));
    if (next == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    * next = * candidate;
    status = Commit(w, next);
    free(next);
    if (status!= UMI_STATUS_OK)return status;
    /* Allocate history up front. Nothing fallible follows a successful commit. */
    w -> historyCount = w -> historyCursor + 1U;
    if (w -> historyCount == UMI_CREATIVE_HISTORY_CAPACITY + 1U){
        memmove( & w -> history[0], & w -> history[1], UMI_CREATIVE_HISTORY_CAPACITY * sizeof( * w -> history));
        -- w -> historyCount;
    }
    w -> history[w -> historyCount ++ ] = w -> current;
    w -> historyCursor = w -> historyCount-1U;
    return UMI_STATUS_OK;
}

static UmiStatus Travel(UmiCreativeWorkspace * w, uint64_t revision, bool redo)
{
    UmiStatus status = Ready(w, revision);
    if (status!= UMI_STATUS_OK)return status;
    if ((redo && w -> historyCursor + 1U >= w -> historyCount) || (!redo && w -> historyCursor == 0U))return UMI_STATUS_NOT_FOUND;
    size_t destination = redo ? w -> historyCursor + 1U : w -> historyCursor-1U;
    UmiCreativeProject * next = malloc(sizeof( * next));
    if (next == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    * next = w -> history[destination];
    status = Commit(w, next);
    free(next);
    if (status == UMI_STATUS_OK)w -> historyCursor = destination;
    return status;
}

UmiStatus UmiCreativeWorkspaceUndo(UmiCreativeWorkspace * w, uint64_t revision){
    return Travel(w, revision, false);
}

UmiStatus UmiCreativeWorkspaceRedo(UmiCreativeWorkspace * w, uint64_t revision){
    return Travel(w, revision, true);
}

UmiStatus UmiCreativeWorkspaceHistory(const UmiCreativeWorkspace * w, UmiCreativeHistory * out)
{
    if (w == NULL || out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    out -> undoCount = w -> historyCursor;
    out -> redoCount = w -> historyCount - w -> historyCursor-1U;
    out -> revision = w -> current.revision;
    out -> recoveryRequired = w -> recoveryRequired;
    return UMI_STATUS_OK;
}

