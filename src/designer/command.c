/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/command.c
 *
 * PURPOSE:
 *   Expose undo and redo through the canonical command registry so menus, shortcuts and AI tools use the same designer actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/command.h"
#include <stdio.h>
static UmiStatus undo_handler(void *u,const char *arg,char *out,size_t cap){UmiStatus s;(void)arg;s=umi_designer_history_undo((UmiDesignerHistory *)u);if(out!=NULL&&cap>0U)(void)snprintf(out,cap,"designer undo: %s",umi_status_text(s));return s;}
static UmiStatus redo_handler(void *u,const char *arg,char *out,size_t cap){UmiStatus s;(void)arg;s=umi_designer_history_redo((UmiDesignerHistory *)u);if(out!=NULL&&cap>0U)(void)snprintf(out,cap,"designer redo: %s",umi_status_text(s));return s;}
UmiStatus umi_designer_register_commands(UmiCommandRegistry *commands,UmiDesignerHistory *history){UmiCommandDescriptor d={0};UmiStatus s;if(commands==NULL||history==NULL)return UMI_STATUS_INVALID_ARGUMENT;d.structure_size=(uint32_t)sizeof(d);d.command_id="designer.undo";d.title="Undo";d.category="Designer";d.description="Undo the last semantic designer operation";d.required_permission="";d.handler=undo_handler;d.user_data=history;s=umi_command_registry_register(commands,&d);if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_ALREADY_EXISTS)return s;d.command_id="designer.redo";d.title="Redo";d.description="Redo the last undone semantic designer operation";d.handler=redo_handler;s=umi_command_registry_register(commands,&d);return s==UMI_STATUS_ALREADY_EXISTS?UMI_STATUS_OK:s;}
