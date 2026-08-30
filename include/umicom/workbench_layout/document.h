/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/document.h
 *
 * PURPOSE:
 *   Define the canonical persistent semantic layout document shared by Umicom Desk, Studio, Trader, TMS and future applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DOCUMENT_H
#define UMICOM_WORKBENCH_LAYOUT_DOCUMENT_H

#include "umicom/workbench_layout/node.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchLayoutDocumentFlags {
    UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN = 1U << 0,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED = 1U << 1,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY = 1U << 2,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED = 1U << 3,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_RECOVERED = 1U << 4,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY = 1U << 5
} UmiWorkbenchLayoutDocumentFlags;

typedef struct UmiWorkbenchLayoutDocument {
    uint32_t structure_size;
    UmiWorkbenchLayoutIdentity identity;
    UmiWorkbenchLayoutVersion version;
    UmiWorkbenchLayoutAudit audit;
    char name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char description[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
    UmiWorkbenchLayoutTag tags[UMI_WORKBENCH_LAYOUT_MAX_TAGS];
    size_t tag_count;
    UmiWorkbenchLayoutNode nodes[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t node_count;
    size_t root_index;
    uint32_t flags;
    uint64_t content_hash;
} UmiWorkbenchLayoutDocument;

void umi_workbench_layout_document_init(
    UmiWorkbenchLayoutDocument *document,
    const char *layout_id,
    const char *name);

UmiStatus umi_workbench_layout_document_copy(
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutDocument *source);

UmiStatus umi_workbench_layout_document_validate_structure(
    const UmiWorkbenchLayoutDocument *document);

UmiStatus umi_workbench_layout_document_set_identity(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutIdentity *identity);

UmiStatus umi_workbench_layout_document_set_audit(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutAudit *audit);

UmiStatus umi_workbench_layout_document_set_metadata(
    UmiWorkbenchLayoutDocument *document,
    const char *name,
    const char *category,
    const char *description);

UmiStatus umi_workbench_layout_document_add_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag);

UmiStatus umi_workbench_layout_document_remove_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag);

bool umi_workbench_layout_document_has_tag(
    const UmiWorkbenchLayoutDocument *document,
    const char *tag);

UmiStatus umi_workbench_layout_document_add_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node,
    size_t *out_index);

UmiStatus umi_workbench_layout_document_remove_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    bool remove_descendants);

UmiStatus umi_workbench_layout_document_set_root(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id);

UmiStatus umi_workbench_layout_document_attach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id,
    size_t position);

UmiStatus umi_workbench_layout_document_detach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id);

UmiStatus umi_workbench_layout_document_move_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *new_parent_node_id,
    size_t position);

UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node_mutable(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id);

const UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id);

size_t umi_workbench_layout_document_find_node_index(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id);

const UmiWorkbenchLayoutNode *umi_workbench_layout_document_root(
    const UmiWorkbenchLayoutDocument *document);

UmiWorkbenchLayoutNode *umi_workbench_layout_document_root_mutable(
    UmiWorkbenchLayoutDocument *document);

bool umi_workbench_layout_document_has_flag(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag);

UmiStatus umi_workbench_layout_document_set_flag(
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag,
    bool enabled);

uint64_t umi_workbench_layout_document_calculate_hash(
    const UmiWorkbenchLayoutDocument *document);

void umi_workbench_layout_document_refresh_hash(
    UmiWorkbenchLayoutDocument *document);

void umi_workbench_layout_document_increment_revision(
    UmiWorkbenchLayoutDocument *document);

#ifdef __cplusplus
}
#endif

#endif
