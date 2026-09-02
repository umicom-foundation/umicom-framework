/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/document_store.h
 *
 * PURPOSE:
 *   Publish the public document store contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_DOCUMENT_STORE_H
#define UMICOM_FRONTEND_DOCUMENT_STORE_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend document data shared with callers of this public contract.
 */
typedef struct UmiFrontendDocument { char path[UMI_FRONTEND_DEV_PATH_CAPACITY]; char media_type[UMI_FRONTEND_DEV_NAME_CAPACITY]; char language[64]; char content[UMI_FRONTEND_DEV_CONTENT_CAPACITY]; uint64_t revision; int dirty; } UmiFrontendDocument;
/**
 * Represent the frontend document store data shared with callers of this public contract.
 */
typedef struct UmiFrontendDocumentStore { UmiFrontendDocument items[UMI_FRONTEND_DEV_MAX_DOCUMENTS]; size_t count; uint64_t revision; } UmiFrontendDocumentStore;
/**
 * Initialise frontend document store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_document_store_init(UmiFrontendDocumentStore *store);
/**
 * Provide the frontend document store upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_document_store_upsert(UmiFrontendDocumentStore *store,const UmiFrontendDocument *document);
/**
 * Find frontend document store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_document_store_find(const UmiFrontendDocumentStore *store,const char *path,UmiFrontendDocument *out_document);
/**
 * Provide the frontend document store mark saved operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_document_store_mark_saved(UmiFrontendDocumentStore *store,const char *path);
#endif
