/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/ui_internal.h
 *
 * PURPOSE:
 *   Share private string-copy and revision helpers between toolkit-neutral UI
 *   model implementations without exposing implementation details publicly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_INTERNAL_H
#define UMICOM_UI_INTERNAL_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * Provide the ui copy text operation used by this module and its client applications.
 */
static inline int umi_ui_copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return 0;
    written = snprintf(destination, capacity, "%s", source != NULL ? source : "");
    return written >= 0 && (size_t)written < capacity;
}

/**
 * Provide the ui next revision operation used by this module and its client applications.
 */
static inline uint64_t umi_ui_next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

#endif
