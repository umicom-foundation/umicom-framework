/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/architecture.c
 *
 * PURPOSE:
 *   Enforce Framework/Studio boundaries and flag excessive source-level
 *   coupling.
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

#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/source.h"
#include <stdio.h>
#include <string.h>
/* Provide the finding operation used by this module and its client applications. */
static void finding(UmiCodeGuardResult *r,const char *id,UmiCodeGuardSeverity sev,unsigned conf,const char *path,const char *msg,const char *fix){UmiCodeGuardFinding f={0};(void)snprintf(f.rule_id,sizeof(f.rule_id),"%s",id);f.severity=sev;f.category=UMI_CODEGUARD_CATEGORY_ARCHITECTURE;f.confidence=conf;(void)snprintf(f.path,sizeof(f.path),"%s",path);f.line=1U;f.column=1U;(void)snprintf(f.message,sizeof(f.message),"%s",msg);(void)snprintf(f.remediation,sizeof(f.remediation),"%s",fix);(void)umi_codeguard_result_add(r,&f);}
/*
 * Provide the codeguard architecture scan file operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_architecture_scan_file(const char *root,const char *path,const UmiCodeGuardProfile *p,UmiCodeGuardResult *r){UmiCodeGuardSourceInfo info;FILE *f;char line[8192];size_t ln=0U;(void)root;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(path==NULL||p==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_codeguard_source_measure(path,&info)!=UMI_STATUS_OK)return UMI_STATUS_IO_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(info.include_count>p->max_include_count)finding(r,"CODEGUARD-ARCH-COUPLING-002",UMI_CODEGUARD_MEDIUM,85U,path,"Source file has unusually high include fan-out.","Split responsibilities or depend on narrower public contracts.");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(info.lines>p->max_source_lines)finding(r,"CODEGUARD-ARCH-GOD-001",UMI_CODEGUARD_MEDIUM,82U,path,"Source file is large enough to merit a responsibility review.","Split cohesive responsibilities into independently testable modules without fragmenting trivial code.");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(path,"framework/include/")!=NULL){f=fopen(path,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return UMI_STATUS_IO_ERROR;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(fgets(line,sizeof(line),f)!=NULL){++ln;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"gtk/gtk.h")!=NULL||strstr(line,"GtkWidget")!=NULL){UmiCodeGuardFinding x={0};(void)snprintf(x.rule_id,sizeof(x.rule_id),"CODEGUARD-ARCH-UI-002");x.severity=UMI_CODEGUARD_HIGH;x.category=UMI_CODEGUARD_CATEGORY_ARCHITECTURE;x.confidence=99U;(void)snprintf(x.path,sizeof(x.path),"%s",path);x.line=ln;(void)snprintf(x.message,sizeof(x.message),"Toolkit-specific GTK type/header leaked into a public Framework header.");(void)snprintf(x.remediation,sizeof(x.remediation),"Move GTK details behind the Framework GTK adapter and expose toolkit-neutral data/contracts.");(void)umi_codeguard_result_add(r,&x);}}fclose(f);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(path,"applications/studio/")!=NULL){f=fopen(path,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return UMI_STATUS_IO_ERROR;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(fgets(line,sizeof(line),f)!=NULL){++ln;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"sqlite3.h")!=NULL||strstr(line,"sqlite3_")!=NULL)finding(r,"CODEGUARD-ARCH-DATA-002",UMI_CODEGUARD_HIGH,98U,path,"Studio appears to access SQLite directly instead of the Data Server.","Route persistence through Umicom Data Server repositories and transactions.");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"framework/src/")!=NULL)finding(r,"CODEGUARD-ARCH-PRIVATE-002",UMI_CODEGUARD_HIGH,99U,path,"Studio references Framework private implementation source.","Use only installed/public include/umicom APIs and exported CMake targets.");}fclose(f);}return UMI_STATUS_OK;}
