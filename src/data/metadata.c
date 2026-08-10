/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/metadata.c
 *
 * PURPOSE:
 *   Implement stable tab-separated metadata serialisation and parsing through a namespaced repository.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/metadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_metadata_save(UmiRepository *repository,
                            const UmiMetadata *metadata)
{
    char text[1024];
    int written;
    if (repository == NULL || metadata == NULL || metadata->record_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(text,
                       sizeof(text),
                       "%s\t%u\t%s\t%llu\t%llu",
                       metadata->record_type,
                       metadata->version,
                       metadata->source,
                       (unsigned long long)metadata->created_at_nanoseconds,
                       (unsigned long long)metadata->updated_at_nanoseconds);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_repository_save(repository, metadata->record_id, text);
}

UmiStatus umi_metadata_load(const UmiRepository *repository,
                            const char *record_id,
                            UmiMetadata *out_metadata)
{
    char text[1024];
    char *fields[5];
    char *cursor;
    size_t count = 0U;
    UmiStatus status;
    if (repository == NULL || record_id == NULL || out_metadata == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_repository_load(repository, record_id, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    cursor = strtok(text, "\t");
    while (cursor != NULL && count < 5U) {
        fields[count++] = cursor;
        cursor = strtok(NULL, "\t");
    }
    if (count != 5U) return UMI_STATUS_PARSE_ERROR;
    (void)memset(out_metadata, 0, sizeof(*out_metadata));
    (void)snprintf(out_metadata->record_id,
                   sizeof(out_metadata->record_id),
                   "%s",
                   record_id);
    (void)snprintf(out_metadata->record_type,
                   sizeof(out_metadata->record_type),
                   "%s",
                   fields[0]);
    out_metadata->version = (uint32_t)strtoul(fields[1], NULL, 10);
    (void)snprintf(out_metadata->source,
                   sizeof(out_metadata->source),
                   "%s",
                   fields[2]);
    out_metadata->created_at_nanoseconds =
        (uint64_t)strtoull(fields[3], NULL, 10);
    out_metadata->updated_at_nanoseconds =
        (uint64_t)strtoull(fields[4], NULL, 10);
    return UMI_STATUS_OK;
}
