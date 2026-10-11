/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/instruction_target_internal.h
 * PURPOSE: Retain a paused-frame selection independently of adapter and widget lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_INSTRUCTION_TARGET_INTERNAL_H
#define UMICOM_DEBUG_INSTRUCTION_TARGET_INTERNAL_H
#include "umicom/debug_runtime/instruction_inspection.h"
struct UmiDebugInstructionTarget
{
    UmiDebugInstructionContext context;
    UmiDebugSelection *selection;
};
#endif
