/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/command.c
 *
 * PURPOSE:
 *   Implement validation and construction for declarative command records.
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

#include "umicom/declarative/command.h"
#include <string.h>
/*
 * Initialise decl command from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_command_init(UmiDeclCommand *item,const char *left,const char *right){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_decl_id_is_valid(left)||!umi_decl_id_is_valid(right))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(item,0,sizeof(*item));s=umi_decl_copy_text(item->command_id,sizeof(item->command_id),left);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_copy_text(item->handler_id,sizeof(item->handler_id),right);return s;}
/* Check that decl command satisfies its contract before another service relies on it. */
int umi_decl_command_is_valid(const UmiDeclCommand *item){return item!=NULL&&umi_decl_id_is_valid(item->command_id)&&umi_decl_id_is_valid(item->handler_id);}
