/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/codeguard/main.c
 *
 * PURPOSE:
 *   Provide the standalone umicom-codeguard command for source security and
 *   architecture scans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/codeguard.h"
#include <stdio.h>
#include <string.h>
static void usage(void){puts("Usage: umicom-codeguard [root] [--security|--architecture|--ci] [--json file] [--sarif file] [--no-duplicates]");}
int main(int argc,char **argv){const char *root=".";const char *json=NULL,*sarif=NULL;UmiCodeGuardProfile p=umi_codeguard_profile_default();UmiCodeGuardResult *r=NULL;UmiCodeGuardConfig c;UmiStatus st;int i=1;if(i<argc&&argv[i][0]!='-')root=argv[i++];for(;i<argc;++i){if(strcmp(argv[i],"--security")==0)p=umi_codeguard_profile_security();else if(strcmp(argv[i],"--architecture")==0)p=umi_codeguard_profile_architecture();else if(strcmp(argv[i],"--ci")==0)p=umi_codeguard_profile_ci();else if(strcmp(argv[i],"--no-duplicates")==0)p.scan_duplicates=0;else if(strcmp(argv[i],"--json")==0&&i+1<argc)json=argv[++i];else if(strcmp(argv[i],"--sarif")==0&&i+1<argc)sarif=argv[++i];else if(strcmp(argv[i],"--help")==0){usage();return 0;}else{fprintf(stderr,"Unknown option: %s\n",argv[i]);usage();return 2;}}st=umi_codeguard_result_create(512U,&r);if(st!=UMI_STATUS_OK)return 3;c=umi_codeguard_config_default(root);c.profile=p;st=umi_codeguard_scan(&c,r);if(st==UMI_STATUS_OK)st=umi_codeguard_report_write(stdout,UMI_CODEGUARD_REPORT_TEXT,r);if(st==UMI_STATUS_OK&&json!=NULL)st=umi_codeguard_report_file(json,UMI_CODEGUARD_REPORT_JSON,r);if(st==UMI_STATUS_OK&&sarif!=NULL)st=umi_codeguard_report_file(sarif,UMI_CODEGUARD_REPORT_SARIF,r);if(st!=UMI_STATUS_OK){fprintf(stderr,"CodeGuard failed with status %d\n",(int)st);umi_codeguard_result_destroy(r);return 3;}UmiCodeGuardSummary s=umi_codeguard_summary_build(r);int failed=umi_codeguard_summary_failed(&s,p.fail_on);umi_codeguard_result_destroy(r);return failed?1:0;}
