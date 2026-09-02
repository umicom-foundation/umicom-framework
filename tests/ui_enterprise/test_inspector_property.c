/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_property.c
 *
 * PURPOSE:
 *   Exercise the inspector property enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_property.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntInspectorProperty v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_inspector_property_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.property_id,sizeof v.property_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.section_id,sizeof v.section_id,"general")!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.label,sizeof v.label,"Name")!=UMI_STATUS_OK)return 4;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_inspector_property_validate(&v))return 9;puts("ok");return 0;}
