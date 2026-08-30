/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/document.h
 *
 * PURPOSE:
 *   Own the authoritative semantic application tree used by parsers, validators, designers and renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_DOCUMENT_H
#define UMICOM_DECLARATIVE_DOCUMENT_H

#include "umicom/declarative/node.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeclDocument UmiDeclDocument;

typedef struct UmiDeclDocumentSnapshot {
    char application_id[UMI_DECL_ID_CAPACITY];
    UmiDeclVersion version;
    size_t node_count;
    uint64_t revision;
} UmiDeclDocumentSnapshot;

UmiStatus umi_decl_document_create(const char *application_id, UmiDeclDocument **out_document);
void umi_decl_document_destroy(UmiDeclDocument *document);
UmiStatus umi_decl_document_clone(const UmiDeclDocument *source, UmiDeclDocument **out_document);
UmiStatus umi_decl_document_set_version(UmiDeclDocument *document, UmiDeclVersion version);
UmiStatus umi_decl_document_add_node(UmiDeclDocument *document, const UmiDeclNode *node);
UmiStatus umi_decl_document_update_node(UmiDeclDocument *document, const UmiDeclNode *node);
UmiStatus umi_decl_document_remove_node(UmiDeclDocument *document, const char *node_id);
UmiStatus umi_decl_document_find_node(const UmiDeclDocument *document, const char *node_id, UmiDeclNode *out_node);
UmiStatus umi_decl_document_node_at(const UmiDeclDocument *document, size_t index, UmiDeclNode *out_node);
UmiStatus umi_decl_document_snapshot(const UmiDeclDocument *document, UmiDeclDocumentSnapshot *out_snapshot);
size_t umi_decl_document_node_count(const UmiDeclDocument *document);

#ifdef __cplusplus
}
#endif

#endif
