/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_section.c
 *
 * PURPOSE:
 *   Exercise the inspector section enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_section.h"
#include <stdio.h>
int main(void){UmiUiEntInspectorSection v;if(umi_ui_ent_inspector_section_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.section_id,sizeof v.section_id,"id")!=UMI_STATUS_OK)return 2;if(!umi_ui_ent_inspector_section_validate(&v))return 9;puts("ok");return 0;}
