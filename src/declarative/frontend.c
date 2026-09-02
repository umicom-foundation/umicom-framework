/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/frontend.c
 *
 * PURPOSE:
 *   Implement validation and construction for declarative frontend records.
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

#include "umicom/declarative/frontend.h"
#include <string.h>
/*
 * Initialise decl frontend from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_frontend_init(UmiDeclFrontend *item,const char *left,const char *right){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_decl_id_is_valid(left)||!umi_decl_id_is_valid(right))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(item,0,sizeof(*item));s=umi_decl_copy_text(item->frontend_id,sizeof(item->frontend_id),left);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_copy_text(item->adapter_id,sizeof(item->adapter_id),right);return s;}
/* Check that decl frontend satisfies its contract before another service relies on it. */
int umi_decl_frontend_is_valid(const UmiDeclFrontend *item){return item!=NULL&&umi_decl_id_is_valid(item->frontend_id)&&umi_decl_id_is_valid(item->adapter_id);}
