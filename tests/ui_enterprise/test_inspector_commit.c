/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_commit.c
 *
 * PURPOSE:
 *   Exercise the inspector commit enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_commit.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntInspectorCommit c;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_inspector_commit_create(&c,"width","10","20",1U,2U)!=UMI_STATUS_OK||c.commit_revision!=2U)return 1;puts("ok");return 0;}
