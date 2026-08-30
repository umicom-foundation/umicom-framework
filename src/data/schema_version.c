/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/schema_version.c
 *
 * PURPOSE:
 *   Implement schema-version persistence in the canonical Framework metadata key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/schema_version.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define UMI_SCHEMA_VERSION_KEY "umicom.schema.version"

UmiStatus umi_schema_version_get(const UmiStore *store,
                                 uint32_t *out_version)
{
    char text[32];
    char *end = NULL;
    unsigned long value;
    UmiStatus status;
    if (store == NULL || out_version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_store_get(store, UMI_SCHEMA_VERSION_KEY, text, sizeof(text));
    if (status == UMI_STATUS_NOT_FOUND) {
        *out_version = 0U;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;
    errno = 0;
    value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > UINT32_MAX) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_version = (uint32_t)value;
    return UMI_STATUS_OK;
}

UmiStatus umi_schema_version_set(UmiStore *store, uint32_t version)
{
    char text[32];
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(text, sizeof(text), "%u", version);
    return umi_store_set(store, UMI_SCHEMA_VERSION_KEY, text);
}
