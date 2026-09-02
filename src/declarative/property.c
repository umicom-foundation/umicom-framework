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

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/property.h"
#include <string.h>
/*
 * Initialise decl property descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_decl_property_descriptor_init(UmiDeclPropertyDescriptor *d,const char *name,UmiDeclValueKind kind,const char *def,int required){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||name==NULL||name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;(void)memset(d,0,sizeof(*d));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_decl_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->kind=kind;d->required=required!=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(def!=NULL)return umi_decl_copy_text(d->default_value,sizeof(d->default_value),def);return UMI_STATUS_OK;}
/*
 * Provide the decl property validate text operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_property_validate_text(const UmiDeclPropertyDescriptor *d,const char *text){UmiDeclValue value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_decl_value_from_text(d->kind,text,&value);}
