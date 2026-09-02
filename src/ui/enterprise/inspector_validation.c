/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_validation.c
 *
 * PURPOSE:
 *   Implement property-level inspector validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_validation.h"
#include <string.h>
#include <stdlib.h>
/*
 * Perform ui ent inspector validation through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_ui_ent_inspector_validation_run(const UmiUiEntInspectorProperty *p,UmiUiEntInspectorValidation *out){char *end=NULL;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof *out);umi_ui_ent_copy_text(out->property_id,sizeof out->property_id,p->property_id);out->valid=1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(p->required&&p->value[0]=='\0'){out->valid=0;umi_ui_ent_copy_text(out->message,sizeof out->message,"property is required");return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(p->value_type,"integer")==0&&p->value[0]){(void)strtol(p->value,&end,10);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!end||*end!='\0'){out->valid=0;umi_ui_ent_copy_text(out->message,sizeof out->message,"expected integer");}}return UMI_STATUS_OK;}
