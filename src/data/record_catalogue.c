/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_catalogue.c
 *
 * PURPOSE:
 *   Discovery owns a read transaction. Visitor callbacks only copy identities;
 *   metadata reads happen afterwards, outside the Data Server visitor lock.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Discovery owns a read transaction. Visitor callbacks only copy identities;
 * metadata reads happen afterwards, outside the Data Server visitor lock. */
#include "umicom/data/record_catalogue.h"
#include "umicom/base/sha256.h"
#include "include/record_history_internal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct CatalogueScan {
    UmiRecordCatalogueQuery query;
    size_t visited, count;
    char entities[UMI_RECORD_CATALOGUE_ENTITY_LIMIT][64];
    UmiRecordRevision rows[UMI_RECORD_CATALOGUE_ENTITY_LIMIT];
} CatalogueScan;

static bool field_valid(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL) return false;
    for (const char *p = text; p != end; ++p)
        if ((unsigned char)*p < 32U || (unsigned char)*p == 127U) return false;
    return true;
}
static bool query_valid(const UmiRecordCatalogueQuery *query)
{
    if (!field_valid(query->entity_prefix, sizeof query->entity_prefix) ||
        !field_valid(query->text, sizeof query->text) ||
        !field_valid(query->after_entity, sizeof query->after_entity) ||
        !field_valid(query->expected_snapshot, sizeof query->expected_snapshot) ||
        query->limit == 0U || query->limit > UMI_RECORD_CATALOGUE_PAGE_CAPACITY)
        return false;
    size_t length = strlen(query->expected_snapshot);
    if (length != 0U && length != 64U) return false;
    for (size_t i = 0U; i < length; ++i) {
        char c = query->expected_snapshot[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    }
    return query->after_entity[0] == '\0' || length == 64U;
}
static int hex_value(char byte)
{
    if (byte >= '0' && byte <= '9') return byte - '0';
    if (byte >= 'a' && byte <= 'f') return byte - 'a' + 10;
    return -1;
}
static UmiStatus collect_head(const char *key, const char *value, void *context)
{
    (void)value;
    CatalogueScan *scan = context;
    if (++scan->visited > UMI_RECORD_CATALOGUE_SCAN_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    static const char prefix[] = "repo.record-history.h.";
    if (strncmp(key, prefix, sizeof prefix - 1U) != 0) return UMI_STATUS_OK;
    const char *encoded = key + sizeof prefix - 1U;
    size_t size = strlen(encoded);
    if (size == 0U || size > 126U || size % 2U != 0U) return UMI_STATUS_PARSE_ERROR;
    char entity[64] = {0};
    for (size_t i = 0U; i < size / 2U; ++i) {
        int high = hex_value(encoded[2U * i]), low = hex_value(encoded[2U * i + 1U]);
        if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
        unsigned char byte = (unsigned char)((unsigned)high * 16U + (unsigned)low);
        if (byte < 32U || byte == 127U) return UMI_STATUS_PARSE_ERROR;
        entity[i] = (char)byte;
    }
    /* Round-trip through the existing owner of the key format. Alternate
     * encodings must never create two apparent identities for one record. */
    char canonical[192];
    if (umi_record_history_key(entity, 0U, canonical, sizeof canonical) != UMI_STATUS_OK ||
        strcmp(key, canonical) != 0) return UMI_STATUS_PARSE_ERROR;
    if (strncmp(entity, scan->query.entity_prefix, strlen(scan->query.entity_prefix)) != 0)
        return UMI_STATUS_OK;
    if (scan->count == UMI_RECORD_CATALOGUE_ENTITY_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(scan->entities[scan->count++], entity, sizeof entity);
    return UMI_STATUS_OK;
}
static int entity_compare(const void *left, const void *right)
{
    return strcmp(left, right);
}
static bool matches(const UmiRecordRevision *row, const char *text)
{
    return strstr(row->amendment.entity_id, text) != NULL ||
        strstr(row->amendment.source, text) != NULL ||
        strstr(row->amendment.actor, text) != NULL ||
        strstr(row->amendment.reason, text) != NULL;
}
static UmiStatus hash_field(UmiSha256 *hash, const char *field)
{
    /* NUL is excluded by the codecs, making this delimiter unambiguous. */
    return UmiSha256Update(hash, field, strlen(field) + 1U);
}
UmiStatus umi_record_catalogue_page(UmiDataServer *server,
    const UmiRecordCatalogueQuery *query, UmiRecordCataloguePage *output)
{
    if (server == NULL || query == NULL || output == NULL || !query_valid(query))
        return UMI_STATUS_INVALID_ARGUMENT;
    CatalogueScan *scan = calloc(1U, sizeof *scan);
    UmiRecordCataloguePage *page = calloc(1U, sizeof *page);
    if (scan == NULL || page == NULL) { free(scan); free(page); return UMI_STATUS_OUT_OF_MEMORY; }
    scan->query = *query;
    memcpy(page->entity_prefix, query->entity_prefix, strlen(query->entity_prefix) + 1U);
    memcpy(page->text, query->text, strlen(query->text) + 1U);
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) { free(scan); free(page); return status; }
    status = umi_data_server_visit(server, collect_head, scan);
    if (status == UMI_STATUS_OK)
        qsort(scan->entities, scan->count, sizeof scan->entities[0], entity_compare);
    UmiSha256 hash;
    UmiSha256Init(&hash);
    if (status == UMI_STATUS_OK) status = hash_field(&hash, "Umicom record catalogue 1");
    if (status == UMI_STATUS_OK) status = hash_field(&hash, page->entity_prefix);
    if (status == UMI_STATUS_OK) status = hash_field(&hash, page->text);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < scan->count; ++i) {
        if (i != 0U && strcmp(scan->entities[i - 1U], scan->entities[i]) == 0) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        status = umi_record_history_latest(server, scan->entities[i], &scan->rows[i]);
        char encoded[UMI_RECORD_REVISION_TEXT_CAPACITY];
        if (status == UMI_STATUS_OK)
            status = umi_record_revision_format(&scan->rows[i], encoded, sizeof encoded);
        if (status == UMI_STATUS_OK) status = hash_field(&hash, encoded);
    }
    unsigned char digest[UMI_SHA256_BYTES];
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&hash, digest);
    if (status == UMI_STATUS_OK) {
        UmiSha256Hex(digest, page->snapshot);
        if (scan->query.expected_snapshot[0] != '\0' &&
            strcmp(scan->query.expected_snapshot, page->snapshot) != 0) status = UMI_STATUS_INVALID_STATE;
    }
    bool cursor_found = scan->query.after_entity[0] == '\0', more = false;
    page->namespace_count = scan->count;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < scan->count; ++i) {
        const UmiRecordRevision *row = &scan->rows[i];
        if (!matches(row, scan->query.text)) continue;
        ++page->matching_count;
        if (strcmp(row->amendment.entity_id, scan->query.after_entity) == 0) cursor_found = true;
        if (strcmp(row->amendment.entity_id, scan->query.after_entity) <= 0) continue;
        if (page->count < scan->query.limit) page->rows[page->count++] = *row;
        else more = true;
    }
    if (status == UMI_STATUS_OK && !cursor_found) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && more)
        memcpy(page->next_after_entity, page->rows[page->count - 1U].amendment.entity_id,
            sizeof page->next_after_entity);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK) {
        if (umi_data_server_rollback(server) != UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    } else *output = *page;
    free(scan); free(page);
    return status;
}
