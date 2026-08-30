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
typedef struct UmiFrontendDocument { char path[UMI_FRONTEND_DEV_PATH_CAPACITY]; char media_type[UMI_FRONTEND_DEV_NAME_CAPACITY]; char language[64]; char content[UMI_FRONTEND_DEV_CONTENT_CAPACITY]; uint64_t revision; int dirty; } UmiFrontendDocument;
typedef struct UmiFrontendDocumentStore { UmiFrontendDocument items[UMI_FRONTEND_DEV_MAX_DOCUMENTS]; size_t count; uint64_t revision; } UmiFrontendDocumentStore;
UmiStatus umi_frontend_document_store_init(UmiFrontendDocumentStore *store);
UmiStatus umi_frontend_document_store_upsert(UmiFrontendDocumentStore *store,const UmiFrontendDocument *document);
UmiStatus umi_frontend_document_store_find(const UmiFrontendDocumentStore *store,const char *path,UmiFrontendDocument *out_document);
UmiStatus umi_frontend_document_store_mark_saved(UmiFrontendDocumentStore *store,const char *path);
#endif
