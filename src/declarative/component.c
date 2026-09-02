/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/component.c
 *
 * PURPOSE:
 *   Implement semantic component metadata and property lookup used by validation and visual design.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/component.h"
#include <string.h>
/*
 * Initialise decl component descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_decl_component_descriptor_init(UmiDeclComponentDescriptor *d,const char *type,const char *display,const char *category,int container){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||!umi_decl_id_is_valid(type)||display==NULL||category==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(d,0,sizeof(*d));s=umi_decl_copy_text(d->component_type,sizeof(d->component_type),type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_copy_text(d->display_name,sizeof(d->display_name),display);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_copy_text(d->category,sizeof(d->category),category);d->container=container!=0;return s;}
/*
 * Provide the decl component add property operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_component_add_property(UmiDeclComponentDescriptor *d,const UmiDeclPropertyDescriptor *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->property_count>=UMI_DECL_MAX_PROPERTIES)return UMI_STATUS_CAPACITY_EXCEEDED;d->properties[d->property_count++]=*p;return UMI_STATUS_OK;}
/*
 * Provide the decl component find property operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_component_find_property(const UmiDeclComponentDescriptor *d,const char *name,UmiDeclPropertyDescriptor *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||name==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<d->property_count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(d->properties[i].name,name)==0){*out=d->properties[i];return UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}
