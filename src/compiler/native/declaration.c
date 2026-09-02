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
/*
 * Initialise nc declaration from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_declaration_init(UmiNativeDeclaration *d,uint32_t node,UmiNativeDeclarationKind kind,const char *name,uint32_t type){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||node==0U||name==NULL||name[0]=='\0'||type==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));d->node_id=node;d->kind=kind;d->type_id=type;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK; }
/* Check that nc declaration satisfies its contract before another service relies on it. */
UmiStatus umi_nc_declaration_validate(const UmiNativeDeclaration *d){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->node_id==0U||d->name[0]=='\0'||d->type_id==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->external_linkage&&d->internal_linkage)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK; }
