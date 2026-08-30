/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/declaration.c
 *
 * PURPOSE:
 *   Describe top-level and local C declarations independently from parser implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/declaration.h"
#include <string.h>
UmiStatus umi_nc_declaration_init(UmiNativeDeclaration *d,uint32_t node,UmiNativeDeclarationKind kind,const char *name,uint32_t type){ if(d==NULL||node==0U||name==NULL||name[0]=='\0'||type==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));d->node_id=node;d->kind=kind;d->type_id=type;if(umi_nc_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK; }
UmiStatus umi_nc_declaration_validate(const UmiNativeDeclaration *d){ if(d==NULL||d->node_id==0U||d->name[0]=='\0'||d->type_id==0U)return UMI_STATUS_INVALID_ARGUMENT;if(d->external_linkage&&d->internal_linkage)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK; }
