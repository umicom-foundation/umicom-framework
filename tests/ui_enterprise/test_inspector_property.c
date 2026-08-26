/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_property.c
 *
 * PURPOSE:
 *   Exercise the inspector property enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_property.h"
#include <stdio.h>
int main(void){UmiUiEntInspectorProperty v;if(umi_ui_ent_inspector_property_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.property_id,sizeof v.property_id,"id")!=UMI_STATUS_OK)return 2;if(umi_ui_ent_copy_text(v.section_id,sizeof v.section_id,"general")!=UMI_STATUS_OK)return 3;if(umi_ui_ent_copy_text(v.label,sizeof v.label,"Name")!=UMI_STATUS_OK)return 4;if(!umi_ui_ent_inspector_property_validate(&v))return 9;puts("ok");return 0;}
