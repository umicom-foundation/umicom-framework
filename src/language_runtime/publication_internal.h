/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/publication_internal.h
 * PURPOSE: Share bounded copies for private provider publication candidates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_PUBLICATION_INTERNAL_H
#define UMICOM_LANGUAGE_PUBLICATION_INTERNAL_H
#include "umicom/base/status.h"
#include "umicom/base/text.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Array inputs are examined within their own capacity. A larger destination
 * must never allow a scan beyond a smaller provider field. Nothing is copied
 * until both bounds pass, and outputs here are private candidates only. */
static inline UmiStatus umi_language_publication_copy(char *out, size_t capacity,
    const char *input, size_t input_capacity)
{
    size_t length = 0U;
    if (out == NULL || input == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    while (length < input_capacity && input[length] != '\0') ++length;
    if (length == input_capacity || length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, input, length + 1U);
    return UMI_STATUS_OK;
}
static inline UmiStatus umi_language_publication_document(const char *document_id, size_t capacity)
{
    size_t length = 0U;
    if (document_id == NULL || document_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    while (length < capacity && document_id[length] != '\0') ++length;
    return length < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
#endif
