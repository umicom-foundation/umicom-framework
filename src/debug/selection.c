/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/selection.c
 * PURPOSE: Own immutable row evidence and reject callbacks from changed debugger state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "selection_private.h"
#include <stdlib.h>
#include <string.h>
bool UmiDebugViewStampEqual(const UmiDebugViewStamp *a,const UmiDebugViewStamp *b)
{
    return a!=NULL&&b!=NULL&&a->owner==b->owner&&a->selection==b->selection&&a->controller==b->controller&&
        a->configurations==b->configurations&&a->sessions==b->sessions&&a->threads==b->threads&&a->frames==b->frames&&
        a->scopes==b->scopes&&a->variables==b->variables&&a->watches==b->watches&&a->breakpoints==b->breakpoints&&
        memcmp(a->selectedThread,b->selectedThread,sizeof(a->selectedThread))==0&&
        memcmp(a->selectedFrame,b->selectedFrame,sizeof(a->selectedFrame))==0&&
        memcmp(a->selectedScope,b->selectedScope,sizeof(a->selectedScope))==0;
}
UmiStatus UmiDebugSelectionCapture(UmiDebugWorkspace *workspace,UmiDebugSelectionKind kind,size_t index,UmiDebugSelection **out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    if(workspace==NULL||(kind!=UMI_DEBUG_SELECT_THREAD&&kind!=UMI_DEBUG_SELECT_FRAME))return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugSelection candidate;
    UmiStatus status=UmiDebugWorkspaceCopySelection(workspace,kind,index,&candidate);
    if(status!=UMI_STATUS_OK)return status;
    UmiDebugSelection *selection=malloc(sizeof(*selection));
    if(selection==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    *selection=candidate;*out=selection;return UMI_STATUS_OK;
}
void UmiDebugSelectionDestroy(UmiDebugSelection *selection){free(selection);}
UmiStatus UmiDebugSelectionRead(const UmiDebugSelection *selection,UmiDebugSelectionSnapshot *out)
{
    if(selection==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=selection->snapshot;return UMI_STATUS_OK;
}
UmiStatus UmiDebugSelectionValidate(UmiDebugWorkspace *workspace,const UmiDebugSelection *selection,UmiDebugSelectionSnapshot *out)
{
    if(workspace==NULL||selection==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugViewStamp current;
    UmiStatus status=UmiDebugWorkspaceViewStamp(workspace,&current);
    if(status!=UMI_STATUS_OK)return status;
    const UmiDebugViewStamp *old=&selection->stamp;
    /* Unrelated variable/watch/breakpoint changes refresh presentation without
     * retargeting this captured thread or frame. Compare navigation generations. */
    if(current.owner!=old->owner||current.selection!=old->selection||current.controller!=old->controller||
        current.configurations!=old->configurations||current.sessions!=old->sessions||current.threads!=old->threads||current.frames!=old->frames||
        strcmp(current.selectedThread,old->selectedThread)!=0||strcmp(current.selectedFrame,old->selectedFrame)!=0)
        return UMI_STATUS_BUSY;
    if(out!=NULL)*out=selection->snapshot;
    return UMI_STATUS_OK;
}
