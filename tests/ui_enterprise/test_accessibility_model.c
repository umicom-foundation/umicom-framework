/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_accessibility_model.c
 *
 * PURPOSE:
 *   Exercise the accessibility model enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/accessibility_model.h"
#include <stdio.h>
int main(void){UmiUiEntAccessibilityModel v;if(umi_ui_ent_accessibility_model_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.role,sizeof v.role,"id")!=UMI_STATUS_OK)return 2;if(umi_ui_ent_copy_text(v.name,sizeof v.name,"Row")!=UMI_STATUS_OK)return 3;if(!umi_ui_ent_accessibility_model_validate(&v))return 9;puts("ok");return 0;}
