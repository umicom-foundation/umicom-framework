/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/inspector.c
 *
 * PURPOSE:
 *   Expose selected component properties to any frontend without giving the view direct access to document internals.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/inspector.h"
#include <string.h>
UmiStatus umi_designer_inspect(const UmiDesignerDocument *d,const char *id,UmiDesignerInspectorSnapshot *out){UmiDeclNode n;UmiStatus s;if(d==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_document_find_node(umi_designer_document_declarative((UmiDesignerDocument *)d),id,&n);if(s!=UMI_STATUS_OK)return s;(void)memset(out,0,sizeof(*out));(void)umi_decl_copy_text(out->node_id,sizeof(out->node_id),n.node_id);(void)umi_decl_copy_text(out->component_type,sizeof(out->component_type),n.component_type);out->attribute_count=n.attribute_count;if(n.attribute_count>0U)(void)memcpy(out->attributes,n.attributes,n.attribute_count*sizeof(n.attributes[0]));return UMI_STATUS_OK;}
