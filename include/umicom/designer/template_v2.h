/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/template_v2.h
 *
 * PURPOSE:
 *   Store and instantiate reusable multi-component visual-builder templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A template is a small semantic component tree copied by value. Instantiation
 * produces normal visible nodes and one undoable transaction.
 */
#ifndef UMICOM_DESIGNER_TEMPLATE_V2_H
#define UMICOM_DESIGNER_TEMPLATE_V2_H

#include "umicom/designer/transaction_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_V2_TEMPLATE_CAPACITY 64U
#define UMI_DESIGNER_V2_TEMPLATE_NODE_CAPACITY 32U

typedef struct UmiDesignerTemplateV2 {
    char template_id[UMI_DECL_ID_CAPACITY];
    char name[UMI_DECL_NAME_CAPACITY];
    char category[UMI_DECL_NAME_CAPACITY];
    UmiDeclNode nodes[UMI_DESIGNER_V2_TEMPLATE_NODE_CAPACITY];
    size_t node_count;
} UmiDesignerTemplateV2;

typedef struct UmiDesignerTemplateRegistryV2 UmiDesignerTemplateRegistryV2;

UmiStatus umi_designer_template_v2_init(UmiDesignerTemplateV2 *item,
                                         const char *template_id,
                                         const char *name,
                                         const char *category);
UmiStatus umi_designer_template_v2_add_node(UmiDesignerTemplateV2 *item, const UmiDeclNode *node);
UmiStatus umi_designer_template_registry_v2_create(UmiDesignerTemplateRegistryV2 **out_registry);
void umi_designer_template_registry_v2_destroy(UmiDesignerTemplateRegistryV2 *registry);
UmiStatus umi_designer_template_registry_v2_register(UmiDesignerTemplateRegistryV2 *registry,
                                                      const UmiDesignerTemplateV2 *item);
UmiStatus umi_designer_template_registry_v2_find(const UmiDesignerTemplateRegistryV2 *registry,
                                                  const char *template_id,
                                                  UmiDesignerTemplateV2 *out_item);
size_t umi_designer_template_registry_v2_count(const UmiDesignerTemplateRegistryV2 *registry);
UmiStatus umi_designer_template_v2_instantiate(const UmiDesignerTemplateV2 *item,
                                                UmiDesignerTransactionHistoryV2 *history,
                                                const char *target_parent_id,
                                                const char *id_prefix);

#ifdef __cplusplus
}
#endif
#endif
