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

#ifdef __cplusplus
}
#endif

#endif
