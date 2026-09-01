/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/lifetime.c
 *
 * PURPOSE:
 *   Track simple freed-pointer names to catch obvious double-free and later
 *   dereference patterns.
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

#include "umicom/codeguard/lifetime.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
static void add_find(UmiCodeGuardResult *r,const char *path,size_t line,const char *id,const char *cwe,UmiCodeGuardSeverity sev,unsigned conf,const char *msg,const char *fix){UmiCodeGuardFinding f={0};(void)snprintf(f.rule_id,sizeof(f.rule_id),"%s",id);(void)snprintf(f.cwe,sizeof(f.cwe),"%s",cwe);f.severity=sev;f.category=UMI_CODEGUARD_CATEGORY_MEMORY;f.confidence=conf;(void)snprintf(f.path,sizeof(f.path),"%s",path);f.line=line;f.column=1U;(void)snprintf(f.message,sizeof(f.message),"%s",msg);(void)snprintf(f.remediation,sizeof(f.remediation),"%s",fix);(void)umi_codeguard_result_add(r,&f);}
static int freed_index(const UmiCodeGuardLifetimeTracker *t,const char *name){size_t i;for(i=0U;i<t->count;++i)if(strcmp(t->freed_names[i],name)==0)return (int)i;return -1;}
static int extract_free(const char *line,char *name,size_t cap){const char *p=strstr(line,"free(");size_t n=0U;if(p==NULL)return 0;p+=5;while(isspace((unsigned char)*p))++p;while((isalnum((unsigned char)*p)||*p=='_')&&n+1U<cap)name[n++]=*p++;name[n]='\0';return n>0U;}
void umi_codeguard_lifetime_init(UmiCodeGuardLifetimeTracker *t){if(t!=NULL)memset(t,0,sizeof(*t));}
void umi_codeguard_lifetime_scan(UmiCodeGuardLifetimeTracker *t,const char *path,size_t line_no,const char *line,UmiCodeGuardResult *r)
{
    char name[96];int idx;size_t i;if(t==NULL||path==NULL||line==NULL||r==NULL)return;
    if(extract_free(line,name,sizeof(name))){idx=freed_index(t,name);if(idx>=0){add_find(r,path,line_no,"CODEGUARD-C-MEM-004","CWE-415",UMI_CODEGUARD_HIGH,98U,"Pointer appears to be freed more than once.","Give the allocation one owner and make cleanup idempotent or clear the pointer after release.");return;}if(t->count<64U){(void)snprintf(t->freed_names[t->count],sizeof(t->freed_names[t->count]),"%s",name);t->freed_lines[t->count]=line_no;++t->count;}return;}
    for(i=0U;i<t->count;++i){char deref[120],index[120],assign[120];(void)snprintf(deref,sizeof(deref),"%s->",t->freed_names[i]);(void)snprintf(index,sizeof(index),"%s[",t->freed_names[i]);(void)snprintf(assign,sizeof(assign),"%s =",t->freed_names[i]);if(strstr(line,assign)!=NULL){t->freed_names[i][0]='\0';continue;}if((strstr(line,deref)!=NULL||strstr(line,index)!=NULL)&&t->freed_names[i][0]!='\0'){add_find(r,path,line_no,"CODEGUARD-C-MEM-003","CWE-416",UMI_CODEGUARD_HIGH,93U,"Pointer appears to be dereferenced after free in the same function/file flow.","Do not access the object after ownership is released; move the access before free or create a new valid owner.");t->freed_names[i][0]='\0';}}
    if(strstr(line,"realloc(")!=NULL){const char *eq=strchr(line,'=');const char *rp=strstr(line,"realloc(");if(eq!=NULL&&rp!=NULL&&eq<rp){char lhs[96]={0},arg[96]={0};size_t n=0U;const char *p=eq;while(p>line&&isspace((unsigned char)p[-1]))--p;const char *s=p;while(s>line&&(isalnum((unsigned char)s[-1])||s[-1]=='_'))--s;while(s<p&&n+1U<sizeof(lhs))lhs[n++]=*s++;lhs[n]='\0';p=rp+8;while(isspace((unsigned char)*p))++p;n=0U;while((isalnum((unsigned char)*p)||*p=='_')&&n+1U<sizeof(arg))arg[n++]=*p++;arg[n]='\0';if(lhs[0]!='\0'&&strcmp(lhs,arg)==0)add_find(r,path,line_no,"CODEGUARD-C-MEM-005","CWE-401",UMI_CODEGUARD_HIGH,90U,"realloc result is assigned directly back to the owning pointer.","Assign realloc to a temporary pointer so allocation failure does not lose the original allocation.");}}
}
