/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_validation.c
 *
 * PURPOSE:
 *   Exercise the inspector validation enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_validation.h"
#include <stdio.h>
int main(void){UmiUiEntInspectorProperty p;UmiUiEntInspectorValidation r;umi_ui_ent_inspector_property_init(&p);umi_ui_ent_copy_text(p.property_id,sizeof p.property_id,"width");umi_ui_ent_copy_text(p.section_id,sizeof p.section_id,"layout");umi_ui_ent_copy_text(p.label,sizeof p.label,"Width");umi_ui_ent_copy_text(p.value_type,sizeof p.value_type,"integer");umi_ui_ent_copy_text(p.value,sizeof p.value,"bad");if(umi_ui_ent_inspector_validation_run(&p,&r)!=UMI_STATUS_OK||r.valid)return 1;puts("ok");return 0;}
