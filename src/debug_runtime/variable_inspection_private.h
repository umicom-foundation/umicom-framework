/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/variable_inspection_private.h
 * PURPOSE: Retain copied debugger evidence independently from service and native widget lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VARIABLE_INSPECTION_PRIVATE_H
#define UMICOM_VARIABLE_INSPECTION_PRIVATE_H
#include "umicom/debug_runtime/variable_inspection.h"
struct UmiDebugVariableTarget {
    UmiDebugViewStamp stamp;
    UmiDebugRuntimeVariable value;
    UmiDebugThreadSnapshot thread;
    uint64_t ancestors[UMI_DEBUG_VARIABLE_INSPECTION_DEPTH];
    size_t ancestorCount;
    /* Assignment addresses a name in its parent container, not the child
     * reference used to expand this row. Keep both identities distinct. */
    uint64_t containerReference;
    int ambiguousName;
};
struct UmiDebugVariablePage {
    UmiDebugVariableTarget parent;
    size_t count;
    UmiDebugRuntimeVariable items[];
};
UmiDebugService *UmiDebugWorkspaceVariableService(UmiDebugWorkspace *workspace);
#endif
