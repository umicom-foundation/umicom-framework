/* Umicom Framework | Project/workspace private helpers | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_INTERNAL_H
#define UMICOM_PROJECT_WORKSPACE_INTERNAL_H
#include <stddef.h>
#include <string.h>
#include "umicom/base/status.h"
static inline UmiStatus umi_pw_copy(char *dst, size_t cap, const char *src)
{
    size_t len;
    if (dst == NULL || cap == 0U || src == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    len = strlen(src);
    if (len >= cap) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (len > 0U) (void)memcpy(dst, src, len);
    dst[len] = '\0';
    return UMI_STATUS_OK;
}
#endif
