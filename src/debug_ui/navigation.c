/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_ui/navigation.c
 * PURPOSE: Compose debugger source navigation with existing document and diagnostic policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_ui/navigation.h"
#include "umicom/diagnostic_ui/navigation.h"
#include <string.h>
UmiStatus UmiDebugFrameOpenSource(UmiDocumentCoordinator *documents,
    const UmiDebugStackFrameSnapshot *frame,const char *baseDirectory,size_t *outOffset)
{
    if(documents==NULL||umi_debug_stack_frame_snapshot_validate(frame,NULL)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(frame->source_uri[0]=='\0'||frame->line==0U)return UMI_STATUS_NOT_FOUND;
    UmiDiagnosticSnapshot diagnostic;
    if(strlen(frame->source_uri)>=sizeof(diagnostic.uri))return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status=umi_diagnostic_snapshot_init(&diagnostic,"debug.frame",UMI_DIAGNOSTIC_INFO,
        UMI_DIAGNOSTIC_KIND_GENERAL,"debugger","Selected stack frame");
    if(status!=UMI_STATUS_OK)return status;
    strcpy(diagnostic.uri,frame->source_uri);diagnostic.line=frame->line;diagnostic.column=frame->column;
    /* Keep file opening, draft ownership and failed-open caret policy in the
     * shared coordinator path, rather than adding debugger-owned editor state. */
    return UmiDiagnosticOpenSource(documents,&diagnostic,baseDirectory,outOffset);
}
