/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/internal.h
 *
 * PURPOSE:
 *   Declare the internal contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Project/workspace private helpers | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_INTERNAL_H
#define UMICOM_PROJECT_WORKSPACE_INTERNAL_H
#include <stddef.h>
#include <string.h>
#include "umicom/base/status.h"
/**
 * Copy pw into module-owned storage so callers keep ownership of their input values.
 */
static inline UmiStatus umi_pw_copy(char *dst, size_t cap, const char *src)
{
    size_t len;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || cap == 0U || src == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    len = strlen(src);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (len >= cap) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (len > 0U) (void)memcpy(dst, src, len);
    dst[len] = '\0';
    return UMI_STATUS_OK;
}
#endif
