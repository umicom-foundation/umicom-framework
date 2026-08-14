/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/archive.c
 * PURPOSE: Save and restore version-checked offline knowledge snapshots.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This compact archive is intended for local restart on the same Framework
 * ABI.  Its header records structure sizes so incompatible snapshots fail
 * safely instead of being interpreted as current data.
 */
#include "umicom/knowledge/archive.h"

#include <stdio.h>
#include <string.h>

typedef struct UmiKnowledgeArchiveHeader {
    char magic[8];
    uint32_t version;
    uint32_t collection_size;
    uint32_t source_size;
    uint32_t entry_size;
    uint64_t collection_count;
    uint64_t source_count;
    uint64_t entry_count;
    uint64_t dimension;
} UmiKnowledgeArchiveHeader;

static UmiStatus write_item(FILE *file, const void *value, size_t size)
{
    return fwrite(value, size, 1U, file) == 1U
        ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

static UmiStatus read_item(FILE *file, void *value, size_t size)
{
    return fread(value, size, 1U, file) == 1U
        ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_knowledge_archive_save(const UmiKnowledgeService *service,
                                     const char *path)
{
    UmiKnowledgeServiceSnapshot snapshot;
    UmiKnowledgeArchiveHeader header;
    UmiKnowledgeCatalogue *catalogue;
    UmiKnowledgeVectorIndex *index;
    char temporary[UMI_KNOWLEDGE_URI_CAPACITY];
    FILE *file;
    UmiStatus status;
    size_t position;
    if (service == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snprintf(temporary, sizeof(temporary), "%s.tmp", path) < 0) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_knowledge_service_snapshot(service, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&header, 0, sizeof(header));
    (void)memcpy(header.magic, "UMIKNOW", 7U);
    header.version = UMI_KNOWLEDGE_ARCHIVE_VERSION;
    header.collection_size = (uint32_t)sizeof(UmiKnowledgeCollection);
    header.source_size = (uint32_t)sizeof(UmiKnowledgeSource);
    header.entry_size = (uint32_t)sizeof(UmiKnowledgeVectorEntry);
    header.collection_count = (uint64_t)snapshot.collection_count;
    header.source_count = (uint64_t)snapshot.source_count;
    header.entry_count = (uint64_t)snapshot.chunk_count;
    header.dimension = (uint64_t)snapshot.embedding_dimension;
    file = fopen(temporary, "wb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    status = write_item(file, &header, sizeof(header));
    for (position = 0U; status == UMI_STATUS_OK &&
         position < snapshot.collection_count; ++position) {
        UmiKnowledgeCollection collection;
        status = umi_knowledge_service_collection_at(
            service, position, &collection);
        if (status == UMI_STATUS_OK) {
            status = write_item(file, &collection, sizeof(collection));
        }
    }
    catalogue = umi_knowledge_service_catalogue((UmiKnowledgeService *)service);
    for (position = 0U; status == UMI_STATUS_OK &&
         position < snapshot.source_count; ++position) {
        UmiKnowledgeSource source;
        status = umi_knowledge_catalogue_source_at(catalogue, position, &source);
        if (status == UMI_STATUS_OK) {
            status = write_item(file, &source, sizeof(source));
        }
    }
    index = umi_knowledge_service_vector_index((UmiKnowledgeService *)service);
    for (position = 0U; status == UMI_STATUS_OK &&
         position < snapshot.chunk_count; ++position) {
        UmiKnowledgeVectorEntry entry;
        status = umi_knowledge_vector_index_entry_at(index, position, &entry);
        if (status == UMI_STATUS_OK) {
            status = write_item(file, &entry, sizeof(entry));
        }
    }
    if (fclose(file) != 0 && status == UMI_STATUS_OK) {
        status = UMI_STATUS_IO_ERROR;
    }
    if (status == UMI_STATUS_OK && rename(temporary, path) != 0) {
        status = UMI_STATUS_IO_ERROR;
    }
    if (status != UMI_STATUS_OK) (void)remove(temporary);
    return status;
}

UmiStatus umi_knowledge_archive_load(UmiKnowledgeService *service,
                                     const char *path)
{
    UmiKnowledgeServiceSnapshot snapshot;
    UmiKnowledgeArchiveHeader header;
    FILE *file;
    UmiStatus status;
    size_t position;
    if (service == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_knowledge_service_snapshot(service, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    /* Product shells may pre-register empty default collections.  Collection
     * records are upserted below, but indexed source/vector state must be empty
     * so restore cannot silently combine two archives. */
    if (snapshot.source_count != 0U || snapshot.chunk_count != 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    status = read_item(file, &header, sizeof(header));
    if (status == UMI_STATUS_OK &&
        (memcmp(header.magic, "UMIKNOW", 7U) != 0 ||
         header.version != UMI_KNOWLEDGE_ARCHIVE_VERSION ||
         header.collection_size != sizeof(UmiKnowledgeCollection) ||
         header.source_size != sizeof(UmiKnowledgeSource) ||
         header.entry_size != sizeof(UmiKnowledgeVectorEntry) ||
         header.dimension != snapshot.embedding_dimension)) {
        status = UMI_STATUS_PARSE_ERROR;
    }
    for (position = 0U; status == UMI_STATUS_OK &&
         position < (size_t)header.collection_count; ++position) {
        UmiKnowledgeCollection collection;
        status = read_item(file, &collection, sizeof(collection));
        if (status == UMI_STATUS_OK) {
            status = umi_knowledge_service_add_collection(service, &collection);
        }
    }
    for (position = 0U; status == UMI_STATUS_OK &&
         position < (size_t)header.source_count; ++position) {
        UmiKnowledgeSource source;
        status = read_item(file, &source, sizeof(source));
        if (status == UMI_STATUS_OK) {
            status = umi_knowledge_catalogue_upsert(
                umi_knowledge_service_catalogue(service), &source);
        }
    }
    for (position = 0U; status == UMI_STATUS_OK &&
         position < (size_t)header.entry_count; ++position) {
        UmiKnowledgeVectorEntry entry;
        status = read_item(file, &entry, sizeof(entry));
        if (status == UMI_STATUS_OK) {
            status = umi_knowledge_vector_index_upsert(
                umi_knowledge_service_vector_index(service),
                &entry.chunk, &entry.embedding);
        }
    }
    if (fclose(file) != 0 && status == UMI_STATUS_OK) {
        status = UMI_STATUS_IO_ERROR;
    }
    return status;
}
