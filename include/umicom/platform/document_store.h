/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/document_store.h
 *
 * PURPOSE:
 *   Define the authoritative toolkit-neutral document store used by editors,
 *   workspaces, sessions, recovery services and non-graphical automation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_DOCUMENT_STORE_H
#define UMICOM_PLATFORM_DOCUMENT_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_DISPLAY_NAME_CAPACITY 160U
#define UMI_DOCUMENT_STORE_MAX 256U

typedef uint64_t UmiDocumentId;

typedef struct UmiDocumentSnapshot {
    UmiDocumentId document_id;
    char display_name[UMI_DOCUMENT_DISPLAY_NAME_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    size_t length;
    uint64_t revision;
    uint64_t saved_revision;
    int dirty;
    int external_change;
    int has_path;
} UmiDocumentSnapshot;

typedef struct UmiDocumentStore UmiDocumentStore;

UmiStatus umi_document_store_create(UmiDocumentStore **out_store);
void umi_document_store_destroy(UmiDocumentStore *store);
UmiStatus umi_document_store_new(UmiDocumentStore *store,
                                 const char *display_name,
                                 UmiDocumentId *out_document_id);
/* Adopt already decoded UTF-8 content from a Framework document provider.
 * This keeps the store authoritative while allowing encoding-aware loaders. */
UmiStatus umi_document_store_create_loaded(UmiDocumentStore *store,
                                           const char *display_name,
                                           const char *path,
                                           const char *text,
                                           size_t length,
                                           UmiDocumentId *out_document_id);
UmiStatus umi_document_store_open(UmiDocumentStore *store,
                                  const char *path,
                                  UmiDocumentId *out_document_id);
UmiStatus umi_document_store_close(UmiDocumentStore *store,
                                   UmiDocumentId document_id,
                                   int force);
UmiStatus umi_document_store_snapshot(const UmiDocumentStore *store,
                                      UmiDocumentId document_id,
                                      UmiDocumentSnapshot *out_snapshot);
UmiStatus umi_document_store_at(const UmiDocumentStore *store,
                                size_t index,
                                UmiDocumentSnapshot *out_snapshot);
size_t umi_document_store_count(const UmiDocumentStore *store);
UmiStatus umi_document_store_copy_text(const UmiDocumentStore *store,
                                       UmiDocumentId document_id,
                                       char **out_text,
                                       size_t *out_length);
void umi_document_store_free_text(char *text);
UmiStatus umi_document_store_replace_text(UmiDocumentStore *store,
                                          UmiDocumentId document_id,
                                          const char *text,
                                          size_t length);
UmiStatus umi_document_store_insert(UmiDocumentStore *store,
                                    UmiDocumentId document_id,
                                    size_t offset,
                                    const char *text,
                                    size_t length);
UmiStatus umi_document_store_erase(UmiDocumentStore *store,
                                   UmiDocumentId document_id,
                                   size_t offset,
                                   size_t length);
UmiStatus umi_document_store_save(UmiDocumentStore *store,
                                  UmiDocumentId document_id);
UmiStatus umi_document_store_save_as(UmiDocumentStore *store,
                                     UmiDocumentId document_id,
                                     const char *path);
/* Record that an external provider successfully persisted the current revision. */
UmiStatus umi_document_store_mark_saved_as(UmiDocumentStore *store,
                                           UmiDocumentId document_id,
                                           const char *path);
UmiStatus umi_document_store_mark_external_change(
    UmiDocumentStore *store,
    UmiDocumentId document_id,
    int changed
);

#ifdef __cplusplus
}
#endif

#endif
