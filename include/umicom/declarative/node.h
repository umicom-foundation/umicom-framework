/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/node.h
 *
 * PURPOSE:
 *   Define semantic component nodes with stable identity, parent relationships and typed attributes.
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

#ifndef UMICOM_DECLARATIVE_NODE_H
#define UMICOM_DECLARATIVE_NODE_H

#include "umicom/declarative/attribute.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeclNode {
    UmiDeclNodeKind kind;
    char node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    char parent_id[UMI_DECL_ID_CAPACITY];
    UmiDeclAttribute attributes[UMI_DECL_MAX_ATTRIBUTES];
    size_t attribute_count;
} UmiDeclNode;

UmiStatus umi_decl_node_init(UmiDeclNode *node, const char *node_id, const char *component_type, const char *parent_id);
UmiStatus umi_decl_node_set_attribute(UmiDeclNode *node, const char *name, UmiDeclValueKind kind, const char *value_text);
UmiStatus umi_decl_node_get_attribute(const UmiDeclNode *node, const char *name, UmiDeclAttribute *out_attribute);
UmiStatus umi_decl_node_remove_attribute(UmiDeclNode *node, const char *name);

#ifdef __cplusplus
}
#endif

#endif
