/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/collection.h
 *
 * PURPOSE:
 *   Organise reusable API requests into folders while retaining stable IDs,
 *   deterministic order and optimistic collection revision tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_COLLECTION_H
#define UMICOM_WEB_WORKBENCH_COLLECTION_H

#include "umicom/web/workbench/request.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebWorkbenchCollection {
    char collection_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char description[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    UmiWebWorkbenchRequest requests[UMI_WEB_WORKBENCH_MAX_REQUESTS];
    size_t request_count;
    uint64_t revision;
} UmiWebWorkbenchCollection;

void umi_web_workbench_collection_init(
    UmiWebWorkbenchCollection *collection,
    const char *collection_id,
    const char *name);
UmiStatus umi_web_workbench_collection_upsert(
    UmiWebWorkbenchCollection *collection,
    const UmiWebWorkbenchRequest *request,
    uint64_t expected_revision);
UmiStatus umi_web_workbench_collection_remove(
    UmiWebWorkbenchCollection *collection,
    const char *request_id,
    uint64_t expected_revision);
const UmiWebWorkbenchRequest *umi_web_workbench_collection_find(
    const UmiWebWorkbenchCollection *collection,
    const char *request_id);
size_t umi_web_workbench_collection_query(
    const UmiWebWorkbenchCollection *collection,
    const char *text,
    const char *folder,
    const UmiWebWorkbenchRequest **out_requests,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_COLLECTION_H */
