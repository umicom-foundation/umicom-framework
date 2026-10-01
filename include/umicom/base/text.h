/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/text.h
 *
 * PURPOSE:
 *   Provide one warning-clean bounded text contract for Framework modules that
 *   copy, append or format text into fixed-capacity ABI structures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_TEXT_H
#define UMICOM_BASE_TEXT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Copy the complete source or leave an empty destination when it does not fit. */
UmiStatus umi_text_copy(char *destination, size_t capacity, const char *source);

/* Copy the largest valid prefix and always terminate a non-empty destination. */
size_t umi_text_copy_truncated(char *destination, size_t capacity,
                               const char *source);

/* Append complete text without modifying the destination when it does not fit. */
UmiStatus umi_text_append(char *destination, size_t capacity,
                          const char *suffix);

/* Format bounded diagnostic or presentation text and report whether it fit. */
UmiStatus umi_text_format(char *destination, size_t capacity,
                          const char *format, ...);

/** Replace a complete text field and advance its owning record's revision.
 * Unlike a presentation copy, a refused edit leaves both the previous bytes
 * and revision unchanged. The destination has capacity writable bytes. Source
 * must contain a terminator within readable storage; at most capacity bytes
 * are inspected. A full buffer or exhausted revision returns CAPACITY_EXCEEDED.
 * Source may overlap destination, including a suffix of its current text.
 * revision must point to separate writable storage and must not overlap either
 * text range. Serialize access on the owner; this operation supplies no lock.
 * Every successful edit advances once, including an identical replacement. */
UmiStatus umi_text_update(char *destination, size_t capacity,
                          const char *source, uint64_t *revision);

#ifdef __cplusplus
}
#endif

#endif
