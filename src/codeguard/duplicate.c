/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/duplicate.c
 *
 * PURPOSE:
 *   Collect source fingerprints and report duplicate files without changing
 *   the originals.
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

#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/normalizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiCodeGuardDuplicateSet { UmiCodeGuardDuplicateFile *items; size_t count; size_t capacity; };
/*
 * Initialise codeguard duplicate set from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_codeguard_duplicate_set_create(UmiCodeGuardDuplicateSet **out){UmiCodeGuardDuplicateSet *s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=(UmiCodeGuardDuplicateSet*)calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out=s;return UMI_STATUS_OK;}
/*
 * Release or reset state held by codeguard duplicate set so the same storage can be reused
 * safely.
 */
void umi_codeguard_duplicate_set_destroy(UmiCodeGuardDuplicateSet *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){free(s->items);free(s);}}
/*
 * Return the number of records represented by codeguard duplicate set without changing
 * their state.
 */
size_t umi_codeguard_duplicate_set_count(const UmiCodeGuardDuplicateSet *s){return s!=NULL?s->count:0U;}
/*
 * Add codeguard duplicate set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *s,const char *path){UmiCodeGuardDuplicateFile *p;size_t cap;UmiCodeGuardDuplicateFile f={0};UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||path==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_codeguard_hash_file(path,&f.raw_hash,&f.normalized_hash,&f.size);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;(void)snprintf(f.path,sizeof(f.path),"%s",path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count==s->capacity){cap=s->capacity==0U?128U:s->capacity*2U;p=(UmiCodeGuardDuplicateFile*)realloc(s->items,cap*sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s->items=p;s->capacity=cap;}s->items[s->count++]=f;return UMI_STATUS_OK;}
/* Provide the emit operation used by this module and its client applications. */
static void emit(UmiCodeGuardResult *r,const char *a,const char *b,int normalized){UmiCodeGuardFinding f={0};(void)snprintf(f.rule_id,sizeof(f.rule_id),"%s",normalized?"CODEGUARD-DUP-002":"CODEGUARD-DUP-001");f.severity=UMI_CODEGUARD_MEDIUM;f.category=UMI_CODEGUARD_CATEGORY_DUPLICATION;f.confidence=100U;(void)snprintf(f.path,sizeof(f.path),"%s",b);f.line=1U;f.column=1U;(void)snprintf(f.message,sizeof(f.message),"%s duplicate of %.180s",normalized?"Normalised source is a":"Byte-identical file is a",a);(void)snprintf(f.remediation,sizeof(f.remediation),"Keep one authoritative implementation and have other modules consume its public contract.");(void)umi_codeguard_result_add(r,&f);}
/*
 * Provide the codeguard duplicate emit operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_duplicate_emit(const UmiCodeGuardDuplicateSet *s,UmiCodeGuardResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<s->count;++i)/* Visit each bounded item once so every record receives the same rule. */ for(size_t j=i+1U;j<s->count;++j){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].size==0U||s->items[j].size==0U)continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].raw_hash==s->items[j].raw_hash&&s->items[i].size==s->items[j].size)emit(r,s->items[i].path,s->items[j].path,0);else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].normalized_hash==s->items[j].normalized_hash)emit(r,s->items[i].path,s->items[j].path,1);}return UMI_STATUS_OK;}
