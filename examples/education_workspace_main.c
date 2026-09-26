/* Umicom Framework | Education workspace console
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT. */
#include "umicom/education_workspace/workspace.h"
#include "umicom/education_workspace/projects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv)
{
    if(argc==4 && strcmp(argv[1],"--export-project")==0){
        size_t written=0U;UmiStatus s=UmiEducationExportProject(argv[2],argv[3],&written);
        printf("Project export: status %d; %zu complete files.\n",(int)s,written);
        return s==UMI_STATUS_OK?0:1;
    }
    if(argc!=1){fputs("Usage: umicom-education-workspace-example [--export-project notes|assembly|framework NEW_ABSOLUTE_DIRECTORY]\n",stderr);return 2;}
    UmiDataServer *server=NULL;UmiEducationWorkspace *workspace=NULL;
    UmiStatus s=umi_data_server_create_memory(&server);
    if(s==UMI_STATUS_OK)s=UmiEducationOpen(server,"practice","Workshop learner",&workspace);
    const UmiEducationLesson *lesson=UmiEducationLessonAt(0U);uint32_t answers[3];UmiEducationFeedback feedback;
    for(size_t i=0U;i<3U;++i)answers[i]=lesson->questions[i].correctChoice;
    if(s==UMI_STATUS_OK)s=UmiEducationMarkRead(workspace,lesson->id);
    if(s==UMI_STATUS_OK)s=UmiEducationSubmitQuiz(workspace,lesson->id,answers,&feedback);
    UmiEducationSnapshot snapshot;
    if(s==UMI_STATUS_OK)s=UmiEducationSnapshotRead(workspace,&snapshot);
    if(s==UMI_STATUS_OK){printf("%s\nQuiz score: %u/100\nSaved quiz completion: %u of %u\n",lesson->title,feedback.score,snapshot.passedQuizzes,UMI_EDUCATION_LESSONS);puts("Practice complete. Memory only; no compiler, project program or network was started.");}
    UmiEducationClose(workspace);umi_data_server_destroy(server);return s==UMI_STATUS_OK?0:1;
}
