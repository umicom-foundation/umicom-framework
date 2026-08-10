/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/surface.c
 *
 * PURPOSE:
 *   Map design-surface geometry to semantic properties so layout state remains serialisable and frontend neutral.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/surface.h"
#include <stdio.h>
#include <stdlib.h>
static int attr_int(const UmiDeclNode *n,const char *name,int fallback){UmiDeclAttribute a;if(umi_decl_node_get_attribute(n,name,&a)!=UMI_STATUS_OK)return fallback;return (int)strtol(a.value.text,NULL,10);}
UmiStatus umi_designer_surface_get_rect(const UmiDesignerDocument *d,const char *id,UmiDesignerRect *out){UmiDeclNode n;UmiStatus s;if(d==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_document_find_node(umi_designer_document_declarative((UmiDesignerDocument *)d),id,&n);if(s!=UMI_STATUS_OK)return s;out->x=attr_int(&n,"x",0);out->y=attr_int(&n,"y",0);out->width=attr_int(&n,"width",120);out->height=attr_int(&n,"height",40);return UMI_STATUS_OK;}
UmiStatus umi_designer_surface_set_rect(UmiDesignerDocument *d,const char *id,UmiDesignerRect r){UmiDeclNode n;char value[32];UmiStatus s;if(d==NULL||id==NULL||r.width<0||r.height<0)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_document_find_node(umi_designer_document_declarative(d),id,&n);if(s!=UMI_STATUS_OK)return s;(void)snprintf(value,sizeof(value),"%d",r.x);s=umi_decl_node_set_attribute(&n,"x",UMI_DECL_VALUE_INTEGER,value);if(s==UMI_STATUS_OK){(void)snprintf(value,sizeof(value),"%d",r.y);s=umi_decl_node_set_attribute(&n,"y",UMI_DECL_VALUE_INTEGER,value);}if(s==UMI_STATUS_OK){(void)snprintf(value,sizeof(value),"%d",r.width);s=umi_decl_node_set_attribute(&n,"width",UMI_DECL_VALUE_INTEGER,value);}if(s==UMI_STATUS_OK){(void)snprintf(value,sizeof(value),"%d",r.height);s=umi_decl_node_set_attribute(&n,"height",UMI_DECL_VALUE_INTEGER,value);}if(s==UMI_STATUS_OK)s=umi_decl_document_update_node(umi_designer_document_declarative(d),&n);if(s==UMI_STATUS_OK)umi_designer_document_mark_changed(d);return s;}
