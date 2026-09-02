/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/template.h
 *
 * PURPOSE:
 *   Store and instantiate reusable multi-component visual-builder templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A template is a small semantic component tree copied by value. Instantiation
 * produces normal visible nodes and one undoable transaction.
 */
#ifndef UMICOM_DESIGNER_TEMPLATE_H
#define UMICOM_DESIGNER_TEMPLATE_H

#include "umicom/designer/transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_TEMPLATE_CAPACITY 64U
#define UMI_DESIGNER_TEMPLATE_NODE_CAPACITY 32U

/**
 * Represent the designer template data shared with callers of this public contract.
 */
typedef struct UmiDesignerTemplate {
    char template_id[UMI_DECL_ID_CAPACITY];
    char name[UMI_DECL_NAME_CAPACITY];
    char category[UMI_DECL_NAME_CAPACITY];
    UmiDeclNode nodes[UMI_DESIGNER_TEMPLATE_NODE_CAPACITY];
    size_t node_count;
} UmiDesignerTemplate;

/**
 * Represent the designer template registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTemplateRegistry UmiDesignerTemplateRegistry;

/**
 * Initialise designer template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_template_init(UmiDesignerTemplate *item,
                                         const char *template_id,
                                         const char *name,
                                         const char *category);
/**
 * Provide the designer template add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_template_add_node(UmiDesignerTemplate *item, const UmiDeclNode *node);
/**
 * Initialise designer template registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_template_registry_create(UmiDesignerTemplateRegistry **out_registry);
/**
 * Release or reset state held by designer template registry so the same storage can be
 * reused safely.
 */
void umi_designer_template_registry_destroy(UmiDesignerTemplateRegistry *registry);
/**
 * Add designer template registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_designer_template_registry_register(UmiDesignerTemplateRegistry *registry,
                                                      const UmiDesignerTemplate *item);
/**
 * Find designer template registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_designer_template_registry_find(const UmiDesignerTemplateRegistry *registry,
                                                  const char *template_id,
                                                  UmiDesignerTemplate *out_item);
/**
 * Return the number of records represented by designer template registry without changing
 * their state.
 */
size_t umi_designer_template_registry_count(const UmiDesignerTemplateRegistry *registry);
/**
 * Provide the designer template instantiate operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_template_instantiate(const UmiDesignerTemplate *item,
                                                UmiDesignerTransactionHistory *history,
                                                const char *target_parent_id,
                                                const char *id_prefix);

#ifdef __cplusplus
}
#endif
#endif
