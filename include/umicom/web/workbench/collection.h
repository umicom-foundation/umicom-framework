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

/**
 * Represent the web workbench collection data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchCollection {
    char collection_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char description[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    UmiWebWorkbenchRequest requests[UMI_WEB_WORKBENCH_MAX_REQUESTS];
    size_t request_count;
    uint64_t revision;
} UmiWebWorkbenchCollection;

/**
 * Initialise web workbench collection from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_collection_init(
    UmiWebWorkbenchCollection *collection,
    const char *collection_id,
    const char *name);
/**
 * Provide the web workbench collection upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_collection_upsert(
    UmiWebWorkbenchCollection *collection,
    const UmiWebWorkbenchRequest *request,
    uint64_t expected_revision);
/**
 * Remove web workbench collection while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_collection_remove(
    UmiWebWorkbenchCollection *collection,
    const char *request_id,
    uint64_t expected_revision);
/**
 * Find web workbench collection while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWebWorkbenchRequest *umi_web_workbench_collection_find(
    const UmiWebWorkbenchCollection *collection,
    const char *request_id);
/**
 * Provide the web workbench collection query operation used by this module and its client
 * applications.
 */
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
