/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A linker-only fault injector; it is never compiled into the product. */
#include "fixture.h"
#include <stddef.h>
static long allocationCountdown=-1;
void *__real_malloc(size_t);
void *__real_calloc(size_t,size_t);
void *__wrap_malloc(size_t bytes)
{
    if(allocationCountdown==0){allocationCountdown=-1;return NULL;}
    if(allocationCountdown>0)--allocationCountdown;
    return __real_malloc(bytes);
}
void *__wrap_calloc(size_t count,size_t bytes)
{
    if(allocationCountdown==0){allocationCountdown=-1;return NULL;}
    if(allocationCountdown>0)--allocationCountdown;
    return __real_calloc(count,bytes);
}
int main(void)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);CHECK(RunFixture(&f)==0);
    UmiAiEvidenceReview *review=NULL;
    allocationCountdown=0;CHECK(UmiAiEvidenceCapture(f.workspace,"job",&review)==UMI_STATUS_OUT_OF_MEMORY && review==NULL);
    OK(UmiAiEvidenceCapture(f.workspace,"job",&review));
    char output[1024];memset(output,'Z',sizeof output);size_t bytes=7U;
    allocationCountdown=0;CHECK(UmiAiEvidenceFormat(review,output,sizeof output,&bytes)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(output[0]=='Z' && bytes==7U);
    UmiAiWorkspaceJob *job=calloc(1U,sizeof *job);UmiAiRequest *request=malloc(sizeof *request),*before=malloc(sizeof *before);
    CHECK(job&&request&&before);OK(UmiAiWorkspaceJobFind(f.workspace,"job",job));
    memset(request,0x4D,sizeof *request);memcpy(before,request,sizeof *request);
    allocationCountdown=0;CHECK(UmiAiWorkspaceBuildRequest(job,request)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(memcmp(request,before,sizeof *request)==0);
    UmiAiWorkspaceSnapshot start,after;OK(UmiAiWorkspaceSnapshotRead(f.workspace,&start));
    allocationCountdown=0;CHECK(PrepareFixture(&f,"new-job",f.evidence,2U)==UMI_STATUS_OUT_OF_MEMORY);
    OK(UmiAiWorkspaceSnapshotRead(f.workspace,&after));CHECK(start.revision==after.revision);
    free(job);free(request);free(before);UmiAiEvidenceDestroy(review);CloseFixture(&f);return 0;
}
