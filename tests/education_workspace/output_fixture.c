/* Umicom Framework | Education independent-output fixture
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This fixture exports only to paths explicitly supplied by its test caller. */
#include "umicom/education_workspace/workspace.h"
#include "umicom/education_workspace/projects.h"
#include "umicom/teacher/types.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv)
{
    if(argc==2 && strcmp(argv[1],"--percent")==0){
        uint64_t value,maximum;
        while(scanf("%" SCNu64 " %" SCNu64,&value,&maximum)==2)
            printf("%u\n",umi_teacher_percent(value,maximum));
        return ferror(stdin)?1:0;
    }
    if(argc==4 && strcmp(argv[1],"--export")==0){
        size_t written=0U;UmiStatus s=UmiEducationExportProject(argv[2],argv[3],&written);
        printf("%d %zu\n",(int)s,written);return s==UMI_STATUS_OK?0:1;
    }
    if(argc!=3 || strcmp(argv[1],"--report")!=0)return 2;
    UmiDataServer *server=NULL;UmiEducationWorkspace *workspace=NULL;
    UmiStatus s=umi_data_server_create_memory(&server);
    if(s==UMI_STATUS_OK)s=UmiEducationOpen(server,"fixture","A & <B>",&workspace);
    if(s==UMI_STATUS_OK)s=UmiEducationSaveNote(workspace,"notes.values","<script>alert(1)</script> & a \"quote\"");
    if(s==UMI_STATUS_OK)s=UmiEducationMarkRead(workspace,"notes.values");
    uint32_t answers[3];UmiEducationFeedback feedback;
    const UmiEducationLesson *lesson=UmiEducationLessonAt(0U);
    for(size_t i=0U;i<3U;++i)answers[i]=lesson->questions[i].correctChoice;
    if(s==UMI_STATUS_OK)s=UmiEducationSubmitQuiz(workspace,"notes.values",answers,&feedback);
    size_t size=0U;char *html=NULL;
    if(s==UMI_STATUS_OK)s=UmiEducationReportHtml(workspace,NULL,0U,&size);
    if(s==UMI_STATUS_OK){html=malloc(size);if(html==NULL)s=UMI_STATUS_OUT_OF_MEMORY;}
    if(s==UMI_STATUS_OK)s=UmiEducationReportHtml(workspace,html,size,&size);
    if(s==UMI_STATUS_OK){FILE *file=fopen(argv[2],"wbx");
        if(file==NULL)s=UMI_STATUS_IO_ERROR;
        else{bool ok=fwrite(html,1U,size-1U,file)==size-1U;if(fclose(file)!=0)ok=false;if(!ok)s=UMI_STATUS_IO_ERROR;}}
    free(html);UmiEducationClose(workspace);umi_data_server_destroy(server);return s==UMI_STATUS_OK?0:1;
}
