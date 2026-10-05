/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/memory_inspection_private.h
 * PURPOSE: Keep memory results independent from adapter buffers and widget lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEMORY_INSPECTION_PRIVATE_H
#define UMICOM_MEMORY_INSPECTION_PRIVATE_H
#include "umicom/debug_runtime/memory_inspection.h"
#include "variable_inspection_private.h"
struct UmiDebugMemoryCapture
{
    UmiDebugVariableTarget target;
    UmiDebugMemoryBytes result;
    int64_t offset;
    uint32_t requested;
};
#endif
