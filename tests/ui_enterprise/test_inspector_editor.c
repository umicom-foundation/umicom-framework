/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_editor.c
 *
 * PURPOSE:
 *   Exercise the inspector editor enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_editor.h"
#include <stdio.h>
int main(void){UmiUiEntInspectorEditor v;if(umi_ui_ent_inspector_editor_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.property_id,sizeof v.property_id,"id")!=UMI_STATUS_OK)return 2;if(umi_ui_ent_copy_text(v.editor_kind,sizeof v.editor_kind,"text")!=UMI_STATUS_OK)return 3;if(!umi_ui_ent_inspector_editor_validate(&v))return 9;puts("ok");return 0;}
