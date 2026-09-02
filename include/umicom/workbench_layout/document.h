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

/**
 * List the named workbench layout document flags values accepted by this public contract.
 */
typedef enum UmiWorkbenchLayoutDocumentFlags {
    UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN = 1U << 0,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED = 1U << 1,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY = 1U << 2,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED = 1U << 3,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_RECOVERED = 1U << 4,
    UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY = 1U << 5
} UmiWorkbenchLayoutDocumentFlags;

/**
 * Represent the workbench layout document data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench layout document from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_document_init(
    UmiWorkbenchLayoutDocument *document,
    const char *layout_id,
    const char *name);

/**
 * Copy workbench layout document into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_workbench_layout_document_copy(
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutDocument *source);

/**
 * Provide the workbench layout document validate structure operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_document_validate_structure(
    const UmiWorkbenchLayoutDocument *document);

/**
 * Provide the workbench layout document set identity operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_identity(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutIdentity *identity);

/**
 * Provide the workbench layout document set audit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_audit(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutAudit *audit);

/**
 * Provide the workbench layout document set metadata operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_metadata(
    UmiWorkbenchLayoutDocument *document,
    const char *name,
    const char *category,
    const char *description);

/**
 * Provide the workbench layout document add tag operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_add_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag);

/**
 * Provide the workbench layout document remove tag operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_remove_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag);

/**
 * Provide the workbench layout document has tag operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_document_has_tag(
    const UmiWorkbenchLayoutDocument *document,
    const char *tag);

/**
 * Provide the workbench layout document add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_add_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node,
    size_t *out_index);

/**
 * Provide the workbench layout document remove node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_remove_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    bool remove_descendants);

/**
 * Provide the workbench layout document set root operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_root(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id);

/**
 * Provide the workbench layout document attach child operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_attach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id,
    size_t position);

/**
 * Provide the workbench layout document detach child operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_detach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id);

/**
 * Provide the workbench layout document move node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_move_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *new_parent_node_id,
    size_t position);

/**
 * Provide the workbench layout document find node mutable operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node_mutable(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id);

/**
 * Provide the workbench layout document find node operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id);

/**
 * Provide the workbench layout document find node index operation used by this module and
 * its client applications.
 */
size_t umi_workbench_layout_document_find_node_index(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id);

/**
 * Provide the workbench layout document root operation used by this module and its client
 * applications.
 */
const UmiWorkbenchLayoutNode *umi_workbench_layout_document_root(
    const UmiWorkbenchLayoutDocument *document);

/**
 * Provide the workbench layout document root mutable operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutNode *umi_workbench_layout_document_root_mutable(
    UmiWorkbenchLayoutDocument *document);

/**
 * Provide the workbench layout document has flag operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_document_has_flag(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag);

/**
 * Provide the workbench layout document set flag operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_flag(
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag,
    bool enabled);

/**
 * Provide the workbench layout document calculate hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_layout_document_calculate_hash(
    const UmiWorkbenchLayoutDocument *document);

/**
 * Provide the workbench layout document refresh hash operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_document_refresh_hash(
    UmiWorkbenchLayoutDocument *document);

/**
 * Provide the workbench layout document increment revision operation used by this module
 * and its client applications.
 */
void umi_workbench_layout_document_increment_revision(
    UmiWorkbenchLayoutDocument *document);

#ifdef __cplusplus
}
#endif

#endif
