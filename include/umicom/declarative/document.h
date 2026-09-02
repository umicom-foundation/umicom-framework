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

/*
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

/**
 * Represent the decl document data shared with callers of this public contract.
 */
typedef struct UmiDeclDocument UmiDeclDocument;

/**
 * Represent the decl document snapshot data shared with callers of this public contract.
 */
typedef struct UmiDeclDocumentSnapshot {
    char application_id[UMI_DECL_ID_CAPACITY];
    UmiDeclVersion version;
    size_t node_count;
    uint64_t revision;
} UmiDeclDocumentSnapshot;

/**
 * Initialise decl document from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_document_create(const char *application_id, UmiDeclDocument **out_document);
/**
 * Release or reset state held by decl document so the same storage can be reused safely.
 */
void umi_decl_document_destroy(UmiDeclDocument *document);
/**
 * Provide the decl document clone operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_clone(const UmiDeclDocument *source, UmiDeclDocument **out_document);
/**
 * Provide the decl document set version operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_set_version(UmiDeclDocument *document, UmiDeclVersion version);
/**
 * Provide the decl document add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_add_node(UmiDeclDocument *document, const UmiDeclNode *node);
/**
 * Provide the decl document update node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_update_node(UmiDeclDocument *document, const UmiDeclNode *node);
/**
 * Provide the decl document remove node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_remove_node(UmiDeclDocument *document, const char *node_id);
/**
 * Provide the decl document find node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_find_node(const UmiDeclDocument *document, const char *node_id, UmiDeclNode *out_node);
/**
 * Find decl document node while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_decl_document_node_at(const UmiDeclDocument *document, size_t index, UmiDeclNode *out_node);
/**
 * Provide the decl document snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_snapshot(const UmiDeclDocument *document, UmiDeclDocumentSnapshot *out_snapshot);
/**
 * Return the number of records represented by decl document node without changing their
 * state.
 */
size_t umi_decl_document_node_count(const UmiDeclDocument *document);

#ifdef __cplusplus
}
#endif

#endif
