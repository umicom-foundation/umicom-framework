/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Shared record discovery keeps key encoding and consistent reads out of hosts. */
#ifndef UMICOM_DATA_RECORD_CATALOGUE_H
#define UMICOM_DATA_RECORD_CATALOGUE_H
#include "umicom/data/record_history.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_CATALOGUE_PAGE_CAPACITY 16U
#define UMI_RECORD_CATALOGUE_ENTITY_LIMIT 256U
#define UMI_RECORD_CATALOGUE_SCAN_LIMIT 32768U

typedef struct UmiRecordCatalogueQuery {
    char entity_prefix[64];
    char text[160];
    char after_entity[64];
    char expected_snapshot[65];
    size_t limit;
} UmiRecordCatalogueQuery;

typedef struct UmiRecordCataloguePage {
    char entity_prefix[64];
    char text[160];
    char snapshot[65];
    char next_after_entity[64];
    size_t count, matching_count, namespace_count;
    UmiRecordRevision rows[UMI_RECORD_CATALOGUE_PAGE_CAPACITY];
} UmiRecordCataloguePage;

/* Scan current amendment heads in one owned transaction. Results are sorted by
 * identifier bytes and search is a case-sensitive substring of identifier,
 * source, actor or amendment reason. Empty prefix/text means no such filter.
 * Content is not searched or verified; metadata-only history also appears.
 * These filters are not access control. Authorize the selected store in the host.
 *
 * For the first page set after_entity and expected_snapshot empty. Continue with
 * the page's next_after_entity and snapshot using the same prefix/text. An empty
 * next_after_entity means there is no next page. Changed namespace heads or
 * filters invalidate the snapshot and return INVALID_STATE; restart explicitly.
 * The cursor must identify a matching row. The digest is not authentication.
 *
 * A visit examines at most SCAN_LIMIT total storage rows and retains at most
 * ENTITY_LIMIT heads in the prefix. Exceeding either returns CAPACITY_EXCEEDED,
 * never a silently truncated success. Malformed reserved head keys or selected
 * metadata fail the read. No old history or payload integrity audit is implied.
 * Output is unchanged on failure, including a BUSY nested transaction. */
UmiStatus umi_record_catalogue_page(UmiDataServer *server,
    const UmiRecordCatalogueQuery *query, UmiRecordCataloguePage *output);
#ifdef __cplusplus
}
#endif
#endif
