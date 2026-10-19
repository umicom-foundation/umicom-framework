/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/impact_metadata.c
 * Purpose: Read explicit versioned impact ownership without interpreting raw build expressions.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "impact_internal.h"
#include "inventory_codec.h"
#include <stdlib.h>
#include <string.h>

/* Metadata precedes retained raw inventory text. Hex values preserve names and
 * paths verbatim; unknown keys are refused so a future producer cannot silently
 * promise more complete dependency knowledge than this reader understands. */
UmiStatus UmiImpactMetadataRead(const char *detail, UmiImpactFieldFn field,
    void *context, bool *present)
{
    static const char marker[] = "impact-version=1\n";
    if (detail == NULL || field == NULL || present == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *present = false;
    /* Version two adds related CTest registrations. Version-one readers do not
     * recognise this marker and conservatively widen coverage. Keep their
     * original check for format review; existing version-one input still works. */
#if 0
    if (strncmp(detail, marker, sizeof(marker) - 1U) != 0) return UMI_STATUS_OK;
#endif
    static const char relatedMarker[] = "impact-version=2\n";
    bool related = strncmp(detail, relatedMarker, sizeof(relatedMarker) - 1U) == 0;
    if (!related && strncmp(detail, marker, sizeof(marker) - 1U) != 0) return UMI_STATUS_OK;
    const char *cursor = detail + sizeof(marker) - 1U;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        if (end == NULL) return UMI_STATUS_PARSE_ERROR;
        if ((size_t)(end - cursor) == 10U && memcmp(cursor, "impact-end", 10U) == 0) {
            *present = true;
            return UMI_STATUS_OK;
        }
        const char *equals = memchr(cursor, '=', (size_t)(end - cursor));
        if (equals == NULL || equals == cursor) return UMI_STATUS_PARSE_ERROR;
        size_t keySize = (size_t)(equals - cursor);
        if (keySize > 20U) return UMI_STATUS_PARSE_ERROR;
        char key[21];
        memcpy(key, cursor, keySize); key[keySize] = '\0';
        if (strcmp(key, "complete") != 0 && strcmp(key, "dependency") != 0 &&
            strcmp(key, "target") != 0 && strcmp(key, "always") != 0 && strcmp(key, "path") != 0 &&
            !(related && strcmp(key, "test") == 0))
            return UMI_STATUS_PARSE_ERROR;
        size_t valueSize = (size_t)(end - equals - 1);
        if (valueSize > UMI_RELEASE_INVENTORY_FIELD_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
        char *value = malloc(valueSize + 1U);
        if (value == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        memcpy(value, equals + 1, valueSize); value[valueSize] = '\0';
        bool flag = strcmp(key, "complete") == 0 || strcmp(key, "always") == 0;
        UmiStatus status = UMI_STATUS_OK;
        if (flag) {
            if (strcmp(value, "0") != 0 && strcmp(value, "1") != 0) status = UMI_STATUS_PARSE_ERROR;
        } else {
            status = UmiReleaseInventoryDecodeField(value);
            if (status == UMI_STATUS_OK && value[0] == '\0') status = UMI_STATUS_PARSE_ERROR;
        }
        if (status == UMI_STATUS_OK) status = field(context, key, value);
        free(value);
        if (status != UMI_STATUS_OK) return status;
        cursor = end + 1;
    }
    return UMI_STATUS_PARSE_ERROR;
}
