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

typedef struct UmiDesignerTemplate {
    char template_id[UMI_DECL_ID_CAPACITY];
    char name[UMI_DECL_NAME_CAPACITY];
    char category[UMI_DECL_NAME_CAPACITY];
    UmiDeclNode nodes[UMI_DESIGNER_TEMPLATE_NODE_CAPACITY];
    size_t node_count;
} UmiDesignerTemplate;

typedef struct UmiDesignerTemplateRegistry UmiDesignerTemplateRegistry;

UmiStatus umi_designer_template_init(UmiDesignerTemplate *item,
                                         const char *template_id,
                                         const char *name,
                                         const char *category);
UmiStatus umi_designer_template_add_node(UmiDesignerTemplate *item, const UmiDeclNode *node);
UmiStatus umi_designer_template_registry_create(UmiDesignerTemplateRegistry **out_registry);
void umi_designer_template_registry_destroy(UmiDesignerTemplateRegistry *registry);
UmiStatus umi_designer_template_registry_register(UmiDesignerTemplateRegistry *registry,
                                                      const UmiDesignerTemplate *item);
UmiStatus umi_designer_template_registry_find(const UmiDesignerTemplateRegistry *registry,
                                                  const char *template_id,
                                                  UmiDesignerTemplate *out_item);
size_t umi_designer_template_registry_count(const UmiDesignerTemplateRegistry *registry);
UmiStatus umi_designer_template_instantiate(const UmiDesignerTemplate *item,
                                                UmiDesignerTransactionHistory *history,
                                                const char *target_parent_id,
                                                const char *id_prefix);

#ifdef __cplusplus
}
#endif
#endif
