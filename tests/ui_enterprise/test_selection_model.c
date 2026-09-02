/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_selection_model.c
 *
 * PURPOSE:
 *   Exercise the selection model enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/selection_model.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntSelectionModel m;umi_ui_ent_selection_model_init(&m,UMI_UI_ENT_SELECTION_MULTIPLE);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_selection_model_add(&m,(UmiUiEntSelectionRange){1U,2U,1U,2U})!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_selection_model_contains(&m,2U,2U))return 2;puts("ok");return 0;}
