/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/distribution/runtime/evidence.h"
#include <stdlib.h>
#include <string.h>
static int failNext;
void *__real_calloc(size_t count,size_t size);
void *__wrap_calloc(size_t count,size_t size)
{ if(failNext) { failNext=0;return NULL;}return __real_calloc(count,size); }
int main(void)
{
    UmiReleaseContract *c=NULL; UmiReleaseEvidence *e=NULL;
    const char *contract="UMICOM-RELEASE-CONTRACT\t1\n";
    const char *evidence="UMICOM-RELEASE-EVIDENCE\t1\n";
    failNext=1;
    if(UmiReleaseContractParse(contract,strlen(contract),&c)!=UMI_STATUS_OUT_OF_MEMORY || c!=NULL)return 1;
    failNext=1;
    if(UmiReleaseEvidenceParse(evidence,strlen(evidence),&e)!=UMI_STATUS_OUT_OF_MEMORY || e!=NULL)return 2;
    if(UmiReleaseEvidenceParse(evidence,strlen(evidence),&e)!=UMI_STATUS_OK)return 3;
    UmiReleaseEvidenceDestroy(e);return 0;
}
