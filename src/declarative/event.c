/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/event.c
 *
 * PURPOSE:
 *   Implement validation and construction for declarative event records.
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

#include "umicom/declarative/event.h"
#include <string.h>
/*
 * Initialise decl event from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_event_init(UmiDeclEvent *item,const char *left,const char *right){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_decl_id_is_valid(left)||!umi_decl_id_is_valid(right))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(item,0,sizeof(*item));s=umi_decl_copy_text(item->event_name,sizeof(item->event_name),left);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_decl_copy_text(item->command_id,sizeof(item->command_id),right);return s;}
/* Check that decl event satisfies its contract before another service relies on it. */
int umi_decl_event_is_valid(const UmiDeclEvent *item){return item!=NULL&&umi_decl_id_is_valid(item->event_name)&&umi_decl_id_is_valid(item->command_id);}
