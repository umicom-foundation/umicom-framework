/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/instruction_fields_internal.h
 * PURPOSE: Share complete instruction decoding between captured pages and the compatibility disassembly API.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_INSTRUCTION_FIELDS_INTERNAL_H
#define UMICOM_DEBUG_INSTRUCTION_FIELDS_INTERNAL_H
#include "response_fields_internal.h"
#include "umicom/debug_runtime/instruction_inspection.h"
/* Callers supply temporary storage. Public APIs publish it only after the whole
 * response has passed validation, so a late malformed row cannot leak a prefix. */
UmiStatus DebugInstructionArrayDecode(const char *json, UmiDebugInstructionRecord *rows,
                                      size_t capacity, size_t *count);
UmiStatus DebugInstructionReferenceDecode(const char *json, uint32_t frame_id, char *out,
                                          size_t capacity);
/* Convert a validated address only when the established 64-bit model can hold it.
 * Display-only pages preserve wider address spelling without this conversion. */
UmiStatus DebugInstructionAddressRead(const char *text, uint64_t *out);
#endif
