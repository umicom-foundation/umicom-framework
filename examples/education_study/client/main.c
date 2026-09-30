/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/education_study/client/main.c
 * PURPOSE:
 *   No private headers, global source path or replacement service.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * No private headers, global source path or replacement service. */
#include "umicom/education_workspace/study.h"
#include <stdio.h>
int main(void)
{
    UmiDataServer *server=NULL;UmiEducationWorkspace *workspace=NULL;UmiEducationStudy *study=NULL;
    UmiStatus status=umi_data_server_create_memory(&server);
    if(status==UMI_STATUS_OK)status=UmiEducationOpen(server,"client","Client learner",&workspace);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyCapture(workspace,&study);
    UmiEducationClose(workspace);umi_data_server_destroy(server);
    UmiEducationStudyItem next={0};
    if(status==UMI_STATUS_OK)status=UmiEducationStudyNext(study,"notes",&next);
    if(status==UMI_STATUS_OK)printf("Next lesson: %s\n",next.lesson->id);
    UmiEducationStudyDestroy(study);return status==UMI_STATUS_OK?0:1;
}
