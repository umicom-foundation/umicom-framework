/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/binding.c
 *
 * PURPOSE:
 *   Implement validation and construction for declarative binding records.
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

#include "umicom/declarative/binding.h"
#include <string.h>
/*
 * Initialise decl binding from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_binding_init(UmiDeclBinding *item,const char *left,const char *right){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_decl_id_is_valid(left)||!umi_decl_id_is_valid(right))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(item,0,sizeof(*item));s=umi_decl_copy_text(item->source_expression,sizeof(item->source_expression),left);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_copy_text(item->target_property,sizeof(item->target_property),right);return s;}
/* Check that decl binding satisfies its contract before another service relies on it. */
int umi_decl_binding_is_valid(const UmiDeclBinding *item){return item!=NULL&&umi_decl_id_is_valid(item->source_expression)&&umi_decl_id_is_valid(item->target_property);}
