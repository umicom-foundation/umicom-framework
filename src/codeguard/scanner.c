/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/scanner.c
 *
 * PURPOSE:
 *   Coordinate recursive source scanning, lifetime checks, architecture
 *   metrics and duplicate detection.
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

#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/lifetime.h"
#include "umicom/codeguard/pattern_scan.h"
#include "umicom/codeguard/rule_registry.h"
#include "umicom/codeguard/sanitise.h"
#include "umicom/codeguard/source.h"
#include "umicom/platform/directory.h"
#include <stdio.h>
#include <string.h>
typedef struct ScanState { const UmiCodeGuardConfig *config; UmiCodeGuardRuleRegistry *rules; UmiCodeGuardDuplicateSet *dups; UmiCodeGuardResult *result; UmiStatus status; } ScanState;
static int excluded(const char *p){return strstr(p,"/.git/")!=NULL||strstr(p,"\\.git\\")!=NULL||strstr(p,"/build/")!=NULL||strstr(p,"\\build\\")!=NULL||strstr(p,"/third_party/")!=NULL||strstr(p,"\\third_party\\")!=NULL||strstr(p,"/codeguard/rules/")!=NULL||strstr(p,"\\codeguard\\rules\\")!=NULL;}
UmiStatus umi_codeguard_scan_file(const UmiCodeGuardConfig *c,const char *path,UmiCodeGuardResult *r){FILE *f;char raw[8192],code[8192];size_t ln=0U;int block=0;UmiCodeGuardRuleRegistry *reg=NULL;UmiCodeGuardLifetimeTracker life;UmiStatus st;if(c==NULL||path==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(!umi_codeguard_source_supported(path,c->profile.scan_cpp,c->profile.scan_headers))return UMI_STATUS_OK;f=fopen(path,"rb");if(f==NULL)return UMI_STATUS_IO_ERROR;st=umi_codeguard_rule_registry_create(&reg);if(st==UMI_STATUS_OK)st=umi_codeguard_rule_registry_add_builtin(reg);umi_codeguard_lifetime_init(&life);while(st==UMI_STATUS_OK&&fgets(raw,sizeof(raw),f)!=NULL){++ln;if(strlen(raw)>c->profile.max_line_length){UmiCodeGuardFinding x={0};(void)snprintf(x.rule_id,sizeof(x.rule_id),"CODEGUARD-QUALITY-LINE-001");x.severity=UMI_CODEGUARD_LOW;x.category=UMI_CODEGUARD_CATEGORY_QUALITY;x.confidence=100U;(void)snprintf(x.path,sizeof(x.path),"%s",path);x.line=ln;x.column=c->profile.max_line_length+1U;(void)snprintf(x.message,sizeof(x.message),"Source line exceeds the configured readability limit.");(void)snprintf(x.remediation,sizeof(x.remediation),"Split the expression or statement so reviews and diagnostics remain readable.");st=umi_codeguard_result_add(r,&x);if(st!=UMI_STATUS_OK)break;}umi_codeguard_sanitise_code_line(raw,code,sizeof(code),&block);st=umi_codeguard_pattern_scan_line(reg,path,ln,raw,code,r);if(st==UMI_STATUS_OK)umi_codeguard_lifetime_scan(&life,path,ln,code,r);}fclose(f);umi_codeguard_rule_registry_destroy(reg);if(st==UMI_STATUS_OK&&c->profile.scan_architecture)st=umi_codeguard_architecture_scan_file(c->root,path,&c->profile,r);return st;}
static UmiStatus visit(const UmiFileInfo *info,void *user){ScanState *s=(ScanState*)user;if(s==NULL||info==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(s->status!=UMI_STATUS_OK)return s->status;if(info->kind!=UMI_FILE_KIND_REGULAR||excluded(info->path))return UMI_STATUS_OK;if(!umi_codeguard_source_supported(info->path,s->config->profile.scan_cpp,s->config->profile.scan_headers))return UMI_STATUS_OK;s->status=umi_codeguard_scan_file(s->config,info->path,s->result);if(s->status==UMI_STATUS_OK&&s->config->profile.scan_duplicates)s->status=umi_codeguard_duplicate_set_add(s->dups,info->path);return s->status;}
UmiStatus umi_codeguard_scan(const UmiCodeGuardConfig *c,UmiCodeGuardResult *r){UmiDirectoryWalkOptions o;ScanState s={0};UmiStatus st;if(c==NULL||c->root==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_codeguard_duplicate_set_create(&s.dups);if(st!=UMI_STATUS_OK)return st;s.config=c;s.result=r;s.status=UMI_STATUS_OK;o=umi_directory_walk_options_default();o.recursive=1;o.include_files=1;o.include_directories=0;o.include_hidden=0;o.follow_symbolic_links=0;o.max_depth=64U;st=umi_directory_walk(c->root,&o,visit,&s);if(st==UMI_STATUS_OK)st=s.status;if(st==UMI_STATUS_OK&&c->profile.scan_duplicates)st=umi_codeguard_duplicate_emit(s.dups,r);umi_codeguard_duplicate_set_destroy(s.dups);return st;}
