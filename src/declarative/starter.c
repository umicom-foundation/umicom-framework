/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/starter.c
 *
 * PURPOSE:
 *   Implement validation and construction for declarative starter records.
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

#include "umicom/declarative/starter.h"
#include <string.h>
UmiStatus umi_decl_starter_init(UmiDeclStarter *item,const char *left,const char *right){UmiStatus s;if(item==NULL||!umi_decl_id_is_valid(left)||!umi_decl_id_is_valid(right))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(item,0,sizeof(*item));s=umi_decl_copy_text(item->starter_id,sizeof(item->starter_id),left);if(s==UMI_STATUS_OK)s=umi_decl_copy_text(item->capability_id,sizeof(item->capability_id),right);return s;}
int umi_decl_starter_is_valid(const UmiDeclStarter *item){return item!=NULL&&umi_decl_id_is_valid(item->starter_id)&&umi_decl_id_is_valid(item->capability_id);}
