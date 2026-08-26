/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_commit.c
 *
 * PURPOSE:
 *   Exercise the inspector commit enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_commit.h"
#include <stdio.h>
int main(void){UmiUiEntInspectorCommit c;if(umi_ui_ent_inspector_commit_create(&c,"width","10","20",1U,2U)!=UMI_STATUS_OK||c.commit_revision!=2U)return 1;puts("ok");return 0;}
