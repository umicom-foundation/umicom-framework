/* Umicom Foundation | Sammy Hegab | MIT | Linux linker-wrap allocation fixture. */
#include "umicom/education_workspace/study.h"
#include <stddef.h>
#include <stdio.h>
void *__real_calloc(size_t count,size_t size);
static int fail;
void *__wrap_calloc(size_t count,size_t size)
{if(fail){fail=0;return NULL;}return __real_calloc(count,size);}
int main(void)
{
    UmiDataServer *s=NULL;UmiEducationWorkspace *w=NULL;UmiEducationStudy *study=NULL;
    UmiStatus status=umi_data_server_create_memory(&s);
    if(status==UMI_STATUS_OK)status=UmiEducationOpen(s,"alloc","Allocator test",&w);
    int rc=1;
    if(status==UMI_STATUS_OK) {
        fail=1;status=UmiEducationStudyCapture(w,&study);fail=0;
        if(status==UMI_STATUS_OUT_OF_MEMORY&&study==NULL&&umi_data_server_count(s)==0U&&
            UmiEducationStudyCapture(w,&study)==UMI_STATUS_OK)rc=0;
    }
    UmiEducationStudyDestroy(study);UmiEducationClose(w);umi_data_server_destroy(s);
    if(rc!=0)fputs("allocation recovery failed\n",stderr);
    return rc;
}
