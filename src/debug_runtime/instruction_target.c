/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/instruction_target.c
 * PURPOSE: Own the copied identity used to request further instruction pages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "instruction_target_internal.h"
#include <stdlib.h>
/* The selection owns copied evidence and remains safe to release after workspace closure. */
void UmiDebugInstructionTargetDestroy(UmiDebugInstructionTarget *target)
{
    if (target == NULL)
        return;
    UmiDebugSelectionDestroy(target->selection);
    free(target);
}
/* Return a descriptive copy without reopening or querying the native debugger. */
UmiStatus UmiDebugInstructionTargetRead(const UmiDebugInstructionTarget *target,
                                        UmiDebugInstructionContext *out)
{
    if (target == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = target->context;
    return UMI_STATUS_OK;
}
