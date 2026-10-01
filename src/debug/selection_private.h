/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/selection_private.h
 * PURPOSE: Keep debugger row ownership separate from public copied projections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_SELECTION_PRIVATE_H
#define UMICOM_DEBUG_SELECTION_PRIVATE_H
#include "umicom/debug/selection.h"
struct UmiDebugSelection { UmiDebugViewStamp stamp; UmiDebugSelectionSnapshot snapshot; };
UmiStatus UmiDebugWorkspaceCopySelection(UmiDebugWorkspace *workspace,
    UmiDebugSelectionKind kind, size_t index, UmiDebugSelection *out);
#endif
