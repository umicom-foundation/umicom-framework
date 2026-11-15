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
#include <stdint.h>
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
/* Stage complete duplicate identities and check growth arithmetic before publishing a collection entry. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *s,const char *path){UmiCodeGuardDuplicateFile *p;size_t cap;UmiCodeGuardDuplicateFile f={0};UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||path==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_codeguard_hash_file(path,&f.raw_hash,&f.normalized_hash,&f.size);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;(void)snprintf(f.path,sizeof(f.path),"%s",path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count==s->capacity){cap=s->capacity==0U?128U:s->capacity*2U;p=(UmiCodeGuardDuplicateFile*)realloc(s->items,cap*sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s->items=p;s->capacity=cap;}s->items[s->count++]=f;return UMI_STATUS_OK;}
#endif
/* Share checked candidate ownership between the legacy and bounded native fingerprint readers. Retain the prior standalone implementation for review. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *set, const char *path)
{
    if (set == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t maximum = SIZE_MAX / sizeof(*set->items);
    if (set->count > set->capacity || set->capacity > maximum ||
        (set->capacity != 0U && set->items == NULL))
        return UMI_STATUS_INVALID_STATE;
    if (set->count == maximum) return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiCodeGuardDuplicateFile candidate = {0};
    size_t length = 0U;
    while (length < sizeof candidate.path && path[length] != '\0') ++length;
    if (length == sizeof candidate.path) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy identity before hashing or reallocating, including an input path
     * borrowed from existing collection storage. Never silently truncate it. */
    memcpy(candidate.path, path, length + 1U);
    UmiStatus status = umi_codeguard_hash_file(candidate.path,
        &candidate.raw_hash, &candidate.normalized_hash, &candidate.size);
    if (status != UMI_STATUS_OK) return status;
    if (set->count == set->capacity) {
        size_t capacity = set->capacity == 0U ? 128U :
            set->capacity > maximum / 2U ? maximum : set->capacity * 2U;
        if (capacity > maximum) capacity = maximum;
        UmiCodeGuardDuplicateFile *items = realloc(set->items, capacity * sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        set->items = items;
        set->capacity = capacity;
    }
    set->items[set->count++] = candidate;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus umi_codeguard_duplicate_add_input(UmiCodeGuardDuplicateSet *set,
    const char *path, uint64_t maximum_bytes,
    const UmiCancellationToken *cancellation, int bounded)
{
    if (set == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t maximum = SIZE_MAX / sizeof(*set->items);
    if (set->count > set->capacity || set->capacity > maximum ||
        (set->capacity != 0U && set->items == NULL))
        return UMI_STATUS_INVALID_STATE;
    if (set->count == maximum) return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiCodeGuardDuplicateFile candidate = {0};
    size_t length = 0U;
    while (length < sizeof candidate.path && path[length] != '\0') ++length;
    if (length == sizeof candidate.path) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy identity before hashing or reallocating, including an input path
     * borrowed from existing collection storage. Never silently truncate it. */
    memcpy(candidate.path, path, length + 1U);
    UmiStatus status = bounded
        ? umi_codeguard_hash_file_bounded(candidate.path, maximum_bytes,
            &candidate.raw_hash, &candidate.normalized_hash, &candidate.size, cancellation)
        : umi_codeguard_hash_file(candidate.path,
            &candidate.raw_hash, &candidate.normalized_hash, &candidate.size);
    if (status != UMI_STATUS_OK) return status;
    if (set->count == set->capacity) {
        size_t capacity = set->capacity == 0U ? 128U :
            set->capacity > maximum / 2U ? maximum : set->capacity * 2U;
        if (capacity > maximum) capacity = maximum;
        UmiCodeGuardDuplicateFile *items = realloc(set->items, capacity * sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        set->items = items;
        set->capacity = capacity;
    }
    set->items[set->count++] = candidate;
    return UMI_STATUS_OK;
}

/* Keep the established standalone collection API on its legacy reader.
 * Profile-driven scans call the bounded entry point below. Growth and
 * publication are shared so the two paths cannot diverge in ownership. */
UmiStatus umi_codeguard_duplicate_set_add(UmiCodeGuardDuplicateSet *set, const char *path)
{
    return umi_codeguard_duplicate_add_input(set, path, UINT64_MAX, NULL, 0);
}

UmiStatus umi_codeguard_duplicate_set_add_bounded(UmiCodeGuardDuplicateSet *set,
    const char *path, uint64_t maximum_bytes, const UmiCancellationToken *cancellation)
{
    return umi_codeguard_duplicate_add_input(set, path, maximum_bytes, cancellation, 1);
}

/* Provide the emit operation used by this module and its client applications. */
/* Propagate finding insertion status and report only the fingerprint evidence actually collected. The previous implementation is retained for engineering review. */
#if 0
static void emit(UmiCodeGuardResult *r,const char *a,const char *b,int normalized){UmiCodeGuardFinding f={0};(void)snprintf(f.rule_id,sizeof(f.rule_id),"%s",normalized?"CODEGUARD-DUP-002":"CODEGUARD-DUP-001");f.severity=UMI_CODEGUARD_MEDIUM;f.category=UMI_CODEGUARD_CATEGORY_DUPLICATION;f.confidence=100U;(void)snprintf(f.path,sizeof(f.path),"%s",b);f.line=1U;f.column=1U;(void)snprintf(f.message,sizeof(f.message),"%s duplicate of %.180s",normalized?"Normalised source is a":"Byte-identical file is a",a);(void)snprintf(f.remediation,sizeof(f.remediation),"Keep one authoritative implementation and have other modules consume its public contract.");(void)umi_codeguard_result_add(r,&f);}
#endif
static UmiStatus emit(UmiCodeGuardResult *result, const char *first,
    const char *second, int normalized)
{
    UmiCodeGuardFinding item = {0};
    (void)snprintf(item.rule_id, sizeof item.rule_id, "%s",
        normalized ? "CODEGUARD-DUP-002" : "CODEGUARD-DUP-001");
    item.severity = UMI_CODEGUARD_MEDIUM;
    item.category = UMI_CODEGUARD_CATEGORY_DUPLICATION;
    item.confidence = 100U; /* Confidence describes the observed fingerprint match. */
    (void)snprintf(item.path, sizeof item.path, "%s", second);
    item.line = 1U;
    item.column = 1U;
    (void)snprintf(item.message, sizeof item.message, "%s matches %.180s; review candidate.",
        normalized ? "Normalized source fingerprint" : "Raw fingerprint and byte count", first);
    (void)snprintf(item.remediation, sizeof item.remediation,
        "Compare the current files before consolidation; fingerprints do not prove byte identity or semantic equivalence.");
    return umi_codeguard_result_add(result, &item);
}
/*
 * Provide the codeguard duplicate emit operation used by this module and its client
 * applications.
 */
/* Stop on failed evidence storage so the scan service can discard an incomplete candidate. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_duplicate_emit(const UmiCodeGuardDuplicateSet *s,UmiCodeGuardResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<s->count;++i)/* Visit each bounded item once so every record receives the same rule. */ for(size_t j=i+1U;j<s->count;++j){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].size==0U||s->items[j].size==0U)continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].raw_hash==s->items[j].raw_hash&&s->items[i].size==s->items[j].size)emit(r,s->items[i].path,s->items[j].path,0);else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].normalized_hash==s->items[j].normalized_hash)emit(r,s->items[i].path,s->items[j].path,1);}return UMI_STATUS_OK;}
#endif
UmiStatus umi_codeguard_duplicate_emit(const UmiCodeGuardDuplicateSet *set,
    UmiCodeGuardResult *result)
{
    if (set == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (set->count > set->capacity ||
        set->capacity > SIZE_MAX / sizeof(*set->items) ||
        (set->capacity != 0U && set->items == NULL))
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < set->count; ++i) {
        for (size_t j = i + 1U; j < set->count; ++j) {
            if (set->items[i].size == 0U || set->items[j].size == 0U) continue;
            UmiStatus status = UMI_STATUS_OK;
            if (set->items[i].raw_hash == set->items[j].raw_hash &&
                set->items[i].size == set->items[j].size)
                status = emit(result, set->items[i].path, set->items[j].path, 0);
            else if (set->items[i].normalized_hash == set->items[j].normalized_hash)
                status = emit(result, set->items[i].path, set->items[j].path, 1);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}
