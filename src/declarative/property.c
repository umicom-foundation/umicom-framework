/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/property.c
 *
 * PURPOSE:
 *   Implement typed property descriptors and boundary validation for component schemas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/property.h"
#include <string.h>
UmiStatus umi_decl_property_descriptor_init(UmiDeclPropertyDescriptor *d,const char *name,UmiDeclValueKind kind,const char *def,int required){if(d==NULL||name==NULL||name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;(void)memset(d,0,sizeof(*d));if(umi_decl_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->kind=kind;d->required=required!=0;if(def!=NULL)return umi_decl_copy_text(d->default_value,sizeof(d->default_value),def);return UMI_STATUS_OK;}
UmiStatus umi_decl_property_validate_text(const UmiDeclPropertyDescriptor *d,const char *text){UmiDeclValue value;if(d==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_decl_value_from_text(d->kind,text,&value);}
