/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/education_study/lesson.c
 * PURPOSE: Demonstrate explicit study capture without granting code-test credit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "lesson.h"
#include "umicom/education_workspace/study.h"
#include <stdio.h>
/* The supplied quiz answers are used only by this explanatory fixture. They
 * demonstrate the progression contract, not a method of assessing a learner. */
int UmiEducationStudyLesson(void)
{
    UmiDataServer *server=NULL;
    UmiEducationWorkspace *workspace=NULL;
    UmiEducationStudy *before=NULL,*after=NULL;
    UmiEducationStudyRoute original={0},updated={0},retained={0};
    UmiStatus status=umi_data_server_create_memory(&server);
    if(status==UMI_STATUS_OK)status=UmiEducationOpen(server,"workshop","Workshop learner",&workspace);
    const UmiEducationLesson *first=UmiEducationLessonAt(0U),*goal=UmiEducationLessonAt(3U);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyCapture(workspace,&before);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyPlan(before,goal->id,&original);
    if(status==UMI_STATUS_OK&&umi_data_server_count(server)!=0U)status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK)status=UmiEducationMarkRead(workspace,first->id);
    uint32_t answers[UMI_EDUCATION_QUESTIONS];UmiEducationFeedback feedback={0};
    for(size_t i=0U;i<UMI_EDUCATION_QUESTIONS;++i)answers[i]=first->questions[i].correctChoice;
    if(status==UMI_STATUS_OK)status=UmiEducationSubmitQuiz(workspace,first->id,answers,&feedback);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyCapture(workspace,&after);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyPlan(after,goal->id,&updated);
    UmiEducationClose(workspace);umi_data_server_destroy(server);
    /* These captures own the progress they need, and no longer borrow services. */
    if(status==UMI_STATUS_OK)status=UmiEducationStudyPlan(before,goal->id,&retained);
    if(status==UMI_STATUS_OK&&(original.count!=4U||updated.count!=3U||retained.count!=4U))
        status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK) {
        puts("The Notes goal begins with four practice quizzes remaining.");
        puts("After one explicit quiz attempt: the new capture has three; the old capture still has four.");
        puts("Both captures remain readable after the learner workspace closes.");
        puts("Practice complete. Memory only; no compiler, files, network or certificate was used.");
    } else fprintf(stderr,"Study lesson: %s\n",umi_status_text(status));
    UmiEducationStudyDestroy(before);UmiEducationStudyDestroy(after);
    return status==UMI_STATUS_OK?0:1;
}
