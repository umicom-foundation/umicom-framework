/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_allocation.c
 *
 * PURPOSE:
 *   Inject parser allocation failures and verify native dataset output, progress and ownership remain valid.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_pairs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static void *live[128];
static size_t attempts, failAt, liveCount;
static int trackerError;
static size_t Slot(void *value)
{
    for (size_t i=0;i<sizeof(live)/sizeof(live[0]);++i)
        if (live[i]==value) return i;
    return sizeof(live)/sizeof(live[0]);
}
/* Preserve the original allocation observer for review. Return NULL if the observer itself reaches capacity instead of returning a pointer it just freed; test instrumentation must obey the same lifetime discipline as production. The former implementation is retained for owner review. */
#if 0
static void Remember(void *value)
{
    if(value==NULL)return;
    size_t slot=Slot(NULL);
    if(slot==sizeof(live)/sizeof(live[0])) { trackerError=1; free(value); return; }
    live[slot]=value; ++liveCount;
}
static void *ObservedMalloc(size_t bytes)
{
    ++attempts; if(attempts==failAt)return NULL;
    void *value=malloc(bytes);Remember(value);return value;
}
static void *ObservedCalloc(size_t count,size_t bytes)
{
    ++attempts; if(attempts==failAt)return NULL;
    void *value=calloc(count,bytes);Remember(value);return value;
}
static void *ObservedRealloc(void *value,size_t bytes)
{
    ++attempts; if(attempts==failAt)return NULL;
    if(value==NULL){void *allocated=realloc(NULL,bytes);Remember(allocated);return allocated;}
    size_t slot=Slot(value);
    if(slot==sizeof(live)/sizeof(live[0])||bytes==0U){trackerError=1;return NULL;}
    void *resized=realloc(value,bytes);
    if(resized!=NULL)live[slot]=resized;
    return resized;
}

#endif
static void *Remember(void *value)
{
    if(value==NULL)return NULL;
    size_t slot=Slot(NULL);
    if(slot==sizeof(live)/sizeof(live[0])) { trackerError=1; free(value); return NULL; }
    live[slot]=value; ++liveCount;return value;
}
static void *ObservedMalloc(size_t bytes)
{
    ++attempts; if(attempts==failAt)return NULL;
    void *value=malloc(bytes);return Remember(value);
}
static void *ObservedCalloc(size_t count,size_t bytes)
{
    ++attempts; if(attempts==failAt)return NULL;
    /* Propagate observer rejection just as malloc and realloc do. The
     * original implementation is retained in the disabled observer above. */
    void *value=calloc(count,bytes);return Remember(value);
}
static void *ObservedRealloc(void *value,size_t bytes)
{
    ++attempts; if(attempts==failAt)return NULL;
    if(value==NULL){void *allocated=realloc(NULL,bytes);return Remember(allocated);}
    size_t slot=Slot(value);
    if(slot==sizeof(live)/sizeof(live[0])||bytes==0U){trackerError=1;return NULL;}
    void *resized=realloc(value,bytes);
    if(resized!=NULL)live[slot]=resized;
    return resized;
}
static void ObservedFree(void *value)
{
    if(value==NULL)return;
    size_t slot=Slot(value);
    if(slot==sizeof(live)/sizeof(live[0])){trackerError=1;return;}
    live[slot]=NULL;--liveCount;free(value);
}
/* Include the real JSON owner with allocator observation. The normal archive
 * supplies its unmodified dependencies; these definitions prevent extraction
 * of another JSON-owner object. No freed memory is read by the fixture. */
#define malloc ObservedMalloc
#define calloc ObservedCalloc
#define realloc ObservedRealloc
#define free ObservedFree
#include "../../src/language_runtime/json_tree.c"
#undef malloc
#undef calloc
#undef realloc
#undef free
#include "../../src/ai/training/jsonl_pairs.c"
#define malloc ObservedMalloc
#define free ObservedFree
#include "../../src/ai/training/jsonl_writer.c"
#include "../../src/ai/training/jsonl_summary.c"
#define calloc ObservedCalloc
#include "../../src/ai/training/dataset_group_split.c"
#include "../../src/ai/training/jsonl_partition.c"
#include "../../src/ai/training/jsonl_selection.c"
#include "../../src/ai/training/jsonl_overlap.c"
#include "../../src/ai/training/jsonl_review.c"
#undef calloc
#undef malloc
#undef free

static UmiStatus Count(void *context,const UmiAiTrainingPair *pair,size_t line)
{
    size_t *count=context;
    if(pair->sourceId[0]==0||line!=*count+1U)return UMI_STATUS_INVALID_STATE;
    ++*count;return UMI_STATUS_OK;
}
int main(void)
{
    const char record[]="{\"source_id\":\"one\",\"prompt\":\"input\",\"completion\":\"target\"}";
    UmiAiTrainingPair value;
    CHECK(UmiAiTrainingPairRead(record,sizeof(record)-1U,NULL,&value)==UMI_STATUS_OK);
    size_t allocations=attempts;
    CHECK(allocations>0U&&liveCount==0U&&!trackerError);
    for(size_t failure=1U;failure<=allocations;++failure){
        attempts=0U;failAt=failure;memset(&value,0x5a,sizeof(value));
        UmiAiTrainingPair before=value;
        CHECK(UmiAiTrainingPairRead(record,sizeof(record)-1U,NULL,&value)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(&before,&value,sizeof(value))==0&&liveCount==0U&&!trackerError);
    }
    /* An allocation failure during the second record must preserve exactly
     * the first callback's progress and release the failing record's owners. */
    char two[sizeof(record)*2U+1U];
    int length=snprintf(two,sizeof(two),"%s\n%s",record,record);
    CHECK(length>0&&(size_t)length<sizeof(two));
    for(size_t failure=allocations+1U;failure<=allocations*2U;++failure){
        attempts=0U;failAt=failure;size_t count=0U;
        UmiAiTrainingJsonlReport report;
        CHECK(UmiAiTrainingJsonlVisit(two,(size_t)length,NULL,Count,&count,&report)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(count==1U&&report.acceptedRows==1U&&report.failedLine==2U);
        CHECK(report.consumedBytes==sizeof(record)&&liveCount==0U&&!trackerError);
    }
    failAt=0U;
    for(size_t iteration=0U;iteration<1000U;++iteration){
        CHECK(UmiAiTrainingPairRead(record,sizeof(record)-1U,NULL,&value)==UMI_STATUS_OK);
        CHECK(liveCount==0U&&!trackerError);
    }

    /* The emitter owns exactly one bounded scratch allocation. Failing that
     * allocation must preserve both the byte buffer and its length output. */
    attempts=0U;failAt=1U;
    char output[512],beforeOutput[512];memset(output,'?',sizeof(output));memcpy(beforeOutput,output,sizeof(output));
    size_t emitted=777U;
    CHECK(UmiAiTrainingPairWrite(&value,output,sizeof(output),&emitted)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(emitted==777U&&memcmp(output,beforeOutput,sizeof(output))==0&&liveCount==0U&&!trackerError);
    attempts=0U;failAt=0U;
    CHECK(UmiAiTrainingPairWrite(&value,output,sizeof(output),&emitted)==UMI_STATUS_OK);
    CHECK(attempts==1U&&liveCount==0U&&!trackerError);
    /* Statistics must not expose a partial dataset when any group/parser
     * allocation fails, including allocations after a previously read row. */
    UmiAiTrainingJsonlSummary summary;
    UmiAiTrainingJsonlReport summaryReport;
    attempts=0U;failAt=0U;
    CHECK(UmiAiTrainingJsonlSummarize(two,(size_t)length,NULL,&summary,&summaryReport)==UMI_STATUS_OK);
    size_t summaryAllocations=attempts;
    CHECK(summary.rows==2U&&summaryAllocations>allocations&&liveCount==0U&&!trackerError);
    for(size_t failure=1U;failure<=summaryAllocations;++failure) {
        attempts=0U;failAt=failure;memset(&summary,0x5a,sizeof(summary));
        UmiAiTrainingJsonlSummary original=summary;
        CHECK(UmiAiTrainingJsonlSummarize(two,(size_t)length,NULL,&summary,&summaryReport)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(&summary,&original,sizeof(summary))==0&&liveCount==0U&&!trackerError);
    }
    /* Every allocation in parsing, exact-name ranking and shared splitting
     * can fail without exposing membership or leaking a temporary owner. */
    const char *partitionRows="{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
        "{\"source_id\":\"b\",\"prompt\":\"p\",\"completion\":\"c\"}";
    unsigned char membership[2],originalMembership[2];
    UmiAiTrainingJsonlPartitionSummary partitionSummary,originalPartition;
    attempts=0U;failAt=0U;
    CHECK(UmiAiTrainingJsonlPartition(partitionRows,strlen(partitionRows),1U,42U,NULL,
        membership,sizeof(membership),&partitionSummary,&summaryReport)==UMI_STATUS_OK);
    size_t partitionAllocations=attempts;
    CHECK(partitionSummary.rows==2U&&partitionAllocations>summaryAllocations&&liveCount==0U&&!trackerError);
    for(size_t failure=1U;failure<=partitionAllocations;++failure) {
        attempts=0U;failAt=failure;memset(membership,0x5a,sizeof(membership));
        memcpy(originalMembership,membership,sizeof(membership));
        memset(&partitionSummary,0x5a,sizeof(partitionSummary));originalPartition=partitionSummary;
        CHECK(UmiAiTrainingJsonlPartition(partitionRows,strlen(partitionRows),1U,42U,NULL,
            membership,sizeof(membership),&partitionSummary,&summaryReport)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(membership,originalMembership,sizeof(membership))==0);
        CHECK(memcmp(&partitionSummary,&originalPartition,sizeof(partitionSummary))==0&&liveCount==0U&&!trackerError);
    }
    /* A second scan is necessary because a writer/parser allocation can fail
     * after the first pass measured valid data. Neither pass may publish a
     * prefix or retain a temporary allocation on any failed allocation. */
    const unsigned char selectedMembership[2] = {0U, 1U};
    UmiAiTrainingJsonlSelectionSummary selectionSummary, originalSelection;
    attempts=0U;failAt=0U;
    CHECK(UmiAiTrainingJsonlSelectionWrite(partitionRows,strlen(partitionRows),
        selectedMembership,2U,0U,NULL,output,sizeof(output),&selectionSummary,&summaryReport)==UMI_STATUS_OK);
    size_t selectionAllocations=attempts;
    CHECK(selectionSummary.selectedRows==1U&&selectionAllocations>allocations*2U&&liveCount==0U&&!trackerError);
    for(size_t failure=1U;failure<=selectionAllocations;++failure) {
        attempts=0U;failAt=failure;memset(output,'?',sizeof(output));memcpy(beforeOutput,output,sizeof(output));
        memset(&selectionSummary,0x5a,sizeof(selectionSummary));originalSelection=selectionSummary;
        CHECK(UmiAiTrainingJsonlSelectionWrite(partitionRows,strlen(partitionRows),
            selectedMembership,2U,0U,NULL,output,sizeof(output),&selectionSummary,&summaryReport)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(output,beforeOutput,sizeof(output))==0);
        CHECK(memcmp(&selectionSummary,&originalSelection,sizeof(selectionSummary))==0&&liveCount==0U&&!trackerError);
    }
    /* Fail every owned staging and parser allocation in both overlap scans.
     * No failed scan publishes summary counters or retains a temporary owner. */
    UmiAiTrainingJsonlOverlapSummary overlapSummary, originalOverlap;
    attempts=0U;failAt=0U;
    CHECK(UmiAiTrainingJsonlAuditOverlap(partitionRows,strlen(partitionRows),
        selectedMembership,2U,SIZE_MAX,NULL,&overlapSummary,&summaryReport)==UMI_STATUS_OK);
    const size_t overlapAllocations=attempts;
    CHECK(overlapSummary.sharedPairs==1U && overlapAllocations>allocations*2U && liveCount==0U && !trackerError);
    for(size_t failure=1U;failure<=overlapAllocations;++failure) {
        attempts=0U;failAt=failure;memset(&overlapSummary,0x5a,sizeof(overlapSummary));originalOverlap=overlapSummary;
        CHECK(UmiAiTrainingJsonlAuditOverlap(partitionRows,strlen(partitionRows),
            selectedMembership,2U,SIZE_MAX,NULL,&overlapSummary,&summaryReport)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(&overlapSummary,&originalOverlap,sizeof(overlapSummary))==0 && liveCount==0U && !trackerError);
    }
    /* Failure in the final audit must not leak an already computed split. */
    UmiAiTrainingJsonlReviewSummary reviewSummary,originalReview;
    UmiAiTrainingJsonlReviewOptions reviewOptions={1U,42U,SIZE_MAX};
    attempts=0U;failAt=0U;
    CHECK(UmiAiTrainingJsonlReview(partitionRows,strlen(partitionRows),&reviewOptions,NULL,
        membership,sizeof(membership),&reviewSummary,&summaryReport)==UMI_STATUS_OK);
    const size_t reviewAllocations=attempts;
    CHECK(reviewSummary.overlap.sharedPairs==1U && reviewAllocations>overlapAllocations && liveCount==0U && !trackerError);
    for(size_t failure=1U;failure<=reviewAllocations;++failure){
        attempts=0U;failAt=failure;
        memset(membership,0x5a,sizeof(membership));memcpy(originalMembership,membership,sizeof(membership));
        memset(&reviewSummary,0x5a,sizeof(reviewSummary));originalReview=reviewSummary;
        CHECK(UmiAiTrainingJsonlReview(partitionRows,strlen(partitionRows),&reviewOptions,NULL,
            membership,sizeof(membership),&reviewSummary,&summaryReport)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(membership,originalMembership,sizeof(membership))==0);
        CHECK(memcmp(&reviewSummary,&originalReview,sizeof(reviewSummary))==0 && liveCount==0U && !trackerError);
    }
    /* Exercise the observer boundary itself. A full tracker returns NULL,
     * never a freed address that the subject could dereference. */
    failAt=0U;
    void *slots[sizeof(live)/sizeof(live[0])];
    for(size_t i=0U;i<sizeof(slots)/sizeof(slots[0]);++i) { slots[i]=ObservedMalloc(1U);CHECK(slots[i]!=NULL); }
    CHECK(ObservedMalloc(1U)==NULL&&trackerError);
    trackerError=0;CHECK(ObservedCalloc(1U,1U)==NULL&&trackerError);
    trackerError=0;CHECK(ObservedRealloc(NULL,1U)==NULL&&trackerError);
    for(size_t i=0U;i<sizeof(slots)/sizeof(slots[0]);++i)ObservedFree(slots[i]);
    CHECK(liveCount==0U);
    return 0;
}
