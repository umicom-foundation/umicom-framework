/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_commit.c
 *
 * PURPOSE:
 *   Implement inspector commit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_commit.h"
#include <string.h>
UmiStatus umi_ui_ent_inspector_commit_create(UmiUiEntInspectorCommit *c,const char *id,const char *before,const char *after,uint64_t base,uint64_t rev){if(!c||!umi_ui_ent_id_valid(id)||!before||!after||rev<=base)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof *c);if(umi_ui_ent_copy_text(c->property_id,sizeof c->property_id,id)!=UMI_STATUS_OK||umi_ui_ent_copy_text(c->before_value,sizeof c->before_value,before)!=UMI_STATUS_OK||umi_ui_ent_copy_text(c->after_value,sizeof c->after_value,after)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;c->base_revision=base;c->commit_revision=rev;return UMI_STATUS_OK;}
