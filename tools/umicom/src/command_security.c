/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_security.c
 *
 * PURPOSE:
 *   Implement native umicom security scan and umicom architecture check
 *   commands using CodeGuard.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "cli.h"
#include "umicom/codeguard/codeguard.h"
#include <stdio.h>
#include <string.h>
static int scan_with(const char *root,UmiCodeGuardProfile profile){UmiCodeGuardResult *r=NULL;UmiCodeGuardConfig c;UmiStatus st;UmiCodeGuardSummary s;if(umi_codeguard_result_create(512U,&r)!=UMI_STATUS_OK)return 3;c=umi_codeguard_config_default(root);c.profile=profile;st=umi_codeguard_scan(&c,r);if(st==UMI_STATUS_OK)st=umi_codeguard_report_write(stdout,UMI_CODEGUARD_REPORT_TEXT,r);s=umi_codeguard_summary_build(r);umi_codeguard_result_destroy(r);return st!=UMI_STATUS_OK?3:(umi_codeguard_summary_failed(&s,profile.fail_on)?1:0);}
int umi_cli_command_security(UmiCliContext *context,int argc,char **argv){const char *root=".";(void)context;if(argc>0&&strcmp(argv[0],"scan")!=0)root=argv[0];else if(argc>1)root=argv[1];return scan_with(root,umi_codeguard_profile_security());}
int umi_cli_command_architecture(UmiCliContext *context,int argc,char **argv){const char *root=".";(void)context;if(argc>0&&strcmp(argv[0],"check")!=0)root=argv[0];else if(argc>1)root=argv[1];return scan_with(root,umi_codeguard_profile_architecture());}
