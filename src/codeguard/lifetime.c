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
/* Find add while leaving the underlying catalogue or model owned by this module. */
/* Return finding insertion failures to the analysis caller so missing evidence cannot be reported as completed analysis. The previous implementation is retained for engineering review. */
#if 0
static void add_find(UmiCodeGuardResult *r,const char *path,size_t line,const char *id,const char *cwe,UmiCodeGuardSeverity sev,unsigned conf,const char *msg,const char *fix){UmiCodeGuardFinding f={0};(void)snprintf(f.rule_id,sizeof(f.rule_id),"%s",id);(void)snprintf(f.cwe,sizeof(f.cwe),"%s",cwe);f.severity=sev;f.category=UMI_CODEGUARD_CATEGORY_MEMORY;f.confidence=conf;(void)snprintf(f.path,sizeof(f.path),"%s",path);f.line=line;f.column=1U;(void)snprintf(f.message,sizeof(f.message),"%s",msg);(void)snprintf(f.remediation,sizeof(f.remediation),"%s",fix);(void)umi_codeguard_result_add(r,&f);}
#endif
static UmiStatus add_find(UmiCodeGuardResult *result, const char *path,
    size_t line, const char *id, const char *cwe, UmiCodeGuardSeverity severity,
    unsigned confidence, const char *message, const char *remediation)
{
    UmiCodeGuardFinding finding = {0};
    (void)snprintf(finding.rule_id, sizeof finding.rule_id, "%s", id);
    (void)snprintf(finding.cwe, sizeof finding.cwe, "%s", cwe);
    finding.severity = severity;
    finding.category = UMI_CODEGUARD_CATEGORY_MEMORY;
    finding.confidence = confidence;
    (void)snprintf(finding.path, sizeof finding.path, "%s", path);
    finding.line = line;
    finding.column = 1U;
    (void)snprintf(finding.message, sizeof finding.message, "%s", message);
    (void)snprintf(finding.remediation, sizeof finding.remediation, "%s", remediation);
    return umi_codeguard_result_add(result, &finding);
}
/* Provide the freed index operation used by this module and its client applications. */
static int freed_index(const UmiCodeGuardLifetimeTracker *t,const char *name){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<t->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(t->freed_names[i],name)==0)return (int)i;return -1;}
/* Provide the extract free operation used by this module and its client applications. */
static int extract_free(const char *line,char *name,size_t cap){const char *p=strstr(line,"free(");size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0;p+=5;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*p))++p;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((isalnum((unsigned char)*p)||*p=='_')&&n+1U<cap)name[n++]=*p++;name[n]='\0';return n>0U;}
/*
 * Initialise codeguard lifetime from caller-provided values so later operations receive a
 * known state.
 */
void umi_codeguard_lifetime_init(UmiCodeGuardLifetimeTracker *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t!=NULL)memset(t,0,sizeof(*t));}
/*
 * Provide the codeguard lifetime scan operation used by this module and its client
 * applications.
 */
/* Expose checked lifetime analysis and stage tracker updates, while keeping the existing void entry point for compatibility. The previous implementation remains for review. */
#if 0
void umi_codeguard_lifetime_scan(UmiCodeGuardLifetimeTracker *t,const char *path,size_t line_no,const char *line,UmiCodeGuardResult *r)
{
    char name[96];int idx;size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||path==NULL||line==NULL||r==NULL)return;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(extract_free(line,name,sizeof(name))){idx=freed_index(t,name);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(idx>=0){add_find(r,path,line_no,"CODEGUARD-C-MEM-004","CWE-415",UMI_CODEGUARD_HIGH,98U,"Pointer appears to be freed more than once.","Give the allocation one owner and make cleanup idempotent or clear the pointer after release.");return;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t->count<64U){(void)snprintf(t->freed_names[t->count],sizeof(t->freed_names[t->count]),"%s",name);t->freed_lines[t->count]=line_no;++t->count;}return;}
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<t->count;++i){char deref[120],index[120],assign[120];(void)snprintf(deref,sizeof(deref),"%s->",t->freed_names[i]);(void)snprintf(index,sizeof(index),"%s[",t->freed_names[i]);(void)snprintf(assign,sizeof(assign),"%s =",t->freed_names[i]);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,assign)!=NULL){t->freed_names[i][0]='\0';continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if((strstr(line,deref)!=NULL||strstr(line,index)!=NULL)&&t->freed_names[i][0]!='\0'){add_find(r,path,line_no,"CODEGUARD-C-MEM-003","CWE-416",UMI_CODEGUARD_HIGH,93U,"Pointer appears to be dereferenced after free in the same function/file flow.","Do not access the object after ownership is released; move the access before free or create a new valid owner.");t->freed_names[i][0]='\0';}}
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(strstr(line,"realloc(")!=NULL){const char *eq=strchr(line,'=');const char *rp=strstr(line,"realloc(");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(eq!=NULL&&rp!=NULL&&eq<rp){char lhs[96]={0},arg[96]={0};size_t n=0U;const char *p=eq;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(p>line&&isspace((unsigned char)p[-1]))--p;const char *s=p;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(s>line&&(isalnum((unsigned char)s[-1])||s[-1]=='_'))--s;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(s<p&&n+1U<sizeof(lhs))lhs[n++]=*s++;lhs[n]='\0';p=rp+8;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*p))++p;n=0U;/* Continue only while work remains available; the loop body advances the state on each pass. */ while((isalnum((unsigned char)*p)||*p=='_')&&n+1U<sizeof(arg))arg[n++]=*p++;arg[n]='\0';/* Protect caller-owned memory by checking that required state is available before it is used. */ if(lhs[0]!='\0'&&strcmp(lhs,arg)==0)add_find(r,path,line_no,"CODEGUARD-C-MEM-005","CWE-401",UMI_CODEGUARD_HIGH,90U,"realloc result is assigned directly back to the owning pointer.","Assign realloc to a temporary pointer so allocation failure does not lose the original allocation.");}}
}
#endif
UmiStatus umi_codeguard_lifetime_scan_checked(UmiCodeGuardLifetimeTracker *tracker,
    const char *path, size_t line_number, const char *line, UmiCodeGuardResult *result)
{
    if (tracker == NULL || path == NULL || line == NULL || result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (tracker->count > 64U) return UMI_STATUS_INVALID_STATE;

    /* Tracker changes publish only after this line has been analyzed. Result
     * insertion remains append-only; callers discard a failed scan candidate
     * instead of retrying a partly emitted line as a completed observation. */
    UmiCodeGuardLifetimeTracker candidate = *tracker;
    UmiStatus status = UMI_STATUS_OK;
    char name[96];
    if (extract_free(line, name, sizeof name)) {
        int index = freed_index(&candidate, name);
        if (index >= 0)
            return add_find(result, path, line_number, "CODEGUARD-C-MEM-004",
                "CWE-415", UMI_CODEGUARD_HIGH, 98U,
                "Pointer appears to be freed more than once.",
                "Give the allocation one owner and make cleanup idempotent or clear the pointer after release.");
        size_t slot = 0U;
        while (slot < candidate.count && candidate.freed_names[slot][0] != '\0')
            ++slot;
        if (slot == candidate.count) {
            if (candidate.count == 64U) return UMI_STATUS_CAPACITY_EXCEEDED;
            ++candidate.count;
        }
        (void)snprintf(candidate.freed_names[slot], sizeof candidate.freed_names[slot], "%s", name);
        candidate.freed_lines[slot] = line_number;
        *tracker = candidate;
        return UMI_STATUS_OK;
    }

    for (size_t i = 0U; i < candidate.count; ++i) {
        if (candidate.freed_names[i][0] == '\0') continue;
        char dereference[120], subscript[120], assignment[120];
        (void)snprintf(dereference, sizeof dereference, "%s->", candidate.freed_names[i]);
        (void)snprintf(subscript, sizeof subscript, "%s[", candidate.freed_names[i]);
        (void)snprintf(assignment, sizeof assignment, "%s =", candidate.freed_names[i]);
        if (strstr(line, assignment) != NULL) {
            candidate.freed_names[i][0] = '\0';
            continue;
        }
        if (strstr(line, dereference) != NULL || strstr(line, subscript) != NULL) {
            status = add_find(result, path, line_number, "CODEGUARD-C-MEM-003",
                "CWE-416", UMI_CODEGUARD_HIGH, 93U,
                "Pointer appears to be dereferenced after free in the same function/file flow.",
                "Do not access the object after ownership is released; move the access before free or create a new valid owner.");
            if (status != UMI_STATUS_OK) return status;
            candidate.freed_names[i][0] = '\0';
        }
    }

    const char *reallocation = strstr(line, "realloc(");
    const char *equals = strchr(line, '=');
    if (reallocation != NULL && equals != NULL && equals < reallocation) {
        char left[96] = {0}, argument[96] = {0};
        size_t count = 0U;
        const char *end = equals;
        while (end > line && isspace((unsigned char)end[-1])) --end;
        const char *begin = end;
        while (begin > line && (isalnum((unsigned char)begin[-1]) || begin[-1] == '_')) --begin;
        while (begin < end && count + 1U < sizeof left) left[count++] = *begin++;
        const char *input = reallocation + 8;
        while (isspace((unsigned char)*input)) ++input;
        count = 0U;
        while ((isalnum((unsigned char)*input) || *input == '_') &&
            count + 1U < sizeof argument) argument[count++] = *input++;
        if (left[0] != '\0' && strcmp(left, argument) == 0) {
            status = add_find(result, path, line_number, "CODEGUARD-C-MEM-005",
                "CWE-401", UMI_CODEGUARD_HIGH, 90U,
                "realloc result is assigned directly back to the owning pointer.",
                "Assign realloc to a temporary pointer so allocation failure does not lose the original allocation.");
            if (status != UMI_STATUS_OK) return status;
        }
    }
    *tracker = candidate;
    return UMI_STATUS_OK;
}

/* Compatibility callers retain the historical void entry point. New scan
 * coordinators must use the checked API to distinguish incomplete evidence. */
void umi_codeguard_lifetime_scan(UmiCodeGuardLifetimeTracker *tracker,
    const char *path, size_t line_number, const char *line, UmiCodeGuardResult *result)
{
    (void)umi_codeguard_lifetime_scan_checked(tracker, path, line_number, line, result);
}
