/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/validator.c
 *
 * PURPOSE:
 *   Validate component types, parent relationships and property types before a template can become an application plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/validator.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_decl_validate(const UmiDeclDocument *doc,const UmiDeclSchema *schema,UmiDeclDiagnosticList *diags){UmiDeclDocumentSnapshot snap;size_t i;size_t root_count=0U;if(doc==NULL||schema==NULL||diags==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_decl_diagnostics_clear(diags);if(umi_decl_document_snapshot(doc,&snap)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;if(!umi_decl_version_equal(snap.version,schema->version))(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_WARNING,0U,0U,"version","Template version differs from the active schema");for(i=0U;i<snap.node_count;++i){UmiDeclNode node;UmiDeclComponentDescriptor component;size_t p;(void)umi_decl_document_node_at(doc,i,&node);if(node.parent_id[0]=='\0')root_count+=1U;else{UmiDeclNode parent;if(umi_decl_document_find_node(doc,node.parent_id,&parent)!=UMI_STATUS_OK)(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_ERROR,i+1U,1U,"parent","Component parent does not exist");else if(strcmp(parent.node_id,node.node_id)==0)(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_ERROR,i+1U,1U,"cycle","Component cannot parent itself");}if(umi_decl_component_registry_find(schema->components,node.component_type,&component)!=UMI_STATUS_OK){(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_ERROR,i+1U,1U,"component-type","Unknown component type");continue;}for(p=0U;p<node.attribute_count;++p){UmiDeclPropertyDescriptor pd;if(umi_decl_component_find_property(&component,node.attributes[p].name,&pd)==UMI_STATUS_OK){if(umi_decl_property_validate_text(&pd,node.attributes[p].value.text)!=UMI_STATUS_OK)(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_ERROR,i+1U,1U,"property-type","Property value does not match the component schema");}else if(!schema->allow_unknown_properties){(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_WARNING,i+1U,1U,"property","Unknown property retained for forward compatibility");}}}if(root_count!=1U)(void)umi_decl_diagnostics_add(diags,UMI_DECL_DIAGNOSTIC_ERROR,0U,0U,"root","Template must contain exactly one root component");return umi_decl_diagnostics_error_count(diags)==0U?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR;}
