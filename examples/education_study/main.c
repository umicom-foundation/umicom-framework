/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/education_study/main.c
 * PURPOSE:
 *   Native study tools. No command writes learner progress or launches a guide.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native study tools. No command writes learner progress or launches a guide. */
#include "lesson.h"
#include "umicom/education_workspace/study.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int Library(const char *query)
{
    UmiEducationLibraryMatches result={0};
    UmiStatus status=UmiEducationLibrarySearch(query,&result);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Library: %s\n",umi_status_text(status));return 1;}
    for(size_t i=0U;i<result.count;++i) {
        const UmiEducationLibraryEntry *entry=UmiEducationLibraryAt(result.indices[i]);
        printf("%s | %s | %s\n  docs/learning/%s\n  %s\n",entry->id,entry->category,entry->title,entry->guideFile,entry->boundary);
    }
    printf("%zu guides. File availability and reading progress were not checked.\n",result.count);
    return ferror(stdout)?1:0;
}
static int Html(void)
{
    size_t required=0U;UmiStatus status=UmiEducationLibraryHtml(NULL,0U,&required);
    char *text=status==UMI_STATUS_OK?malloc(required):NULL;
    if(status==UMI_STATUS_OK&&text==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=UmiEducationLibraryHtml(text,required,&required);
    if(status==UMI_STATUS_OK&&fputs(text,stdout)==EOF)status=UMI_STATUS_IO_ERROR;
    free(text);return status==UMI_STATUS_OK?0:1;
}
static int Plan(const char *id)
{
    UmiDataServer *server=NULL;UmiEducationWorkspace *workspace=NULL;UmiEducationStudy *study=NULL;
    UmiEducationStudyRoute route={0};UmiStatus status=umi_data_server_create_memory(&server);
    if(status==UMI_STATUS_OK)status=UmiEducationOpen(server,"practice","Fresh practice record",&workspace);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyCapture(workspace,&study);
    if(status==UMI_STATUS_OK)status=UmiEducationStudyPlan(study,id,&route);
    if(status==UMI_STATUS_OK) {
        printf("Fresh practice route: %zu quizzes; %u catalogue minutes (estimate only).\n",route.count,route.estimatedMinutes);
        for(size_t i=0U;i<route.count;++i) {
            UmiEducationStudyItem item={0};status=UmiEducationStudyItemRead(study,route.indices[i],&item);
            if(status!=UMI_STATUS_OK)break;
            printf("%s | %s | %s\n",item.lesson->id,item.lesson->title,UmiEducationStudyActionText(item.action));
        }
        puts("No existing learner record was read or changed.");
    } else fprintf(stderr,"Plan: %s\n",umi_status_text(status));
    UmiEducationStudyDestroy(study);UmiEducationClose(workspace);umi_data_server_destroy(server);
    return status==UMI_STATUS_OK&&!ferror(stdout)?0:1;
}
int main(int argc,char **argv)
{
    if(argc==2&&(strcmp(argv[1],"--self-test")==0||strcmp(argv[1],"demo")==0))return UmiEducationStudyLesson();
    if((argc==2||argc==3)&&strcmp(argv[1],"library")==0)return Library(argc==3?argv[2]:"");
    if(argc==2&&strcmp(argv[1],"library-html")==0)return Html();
    if(argc==3&&strcmp(argv[1],"plan")==0)return Plan(argv[2]);
    if(argc==2&&strcmp(argv[1],"lessons")==0) {
        for(size_t i=0U;i<UMI_EDUCATION_LESSONS;++i) {
            const UmiEducationLesson *l=UmiEducationLessonAt(i);printf("%s | %s | %s\n",l->id,l->courseId,l->title);
        }
        return ferror(stdout)?1:0;
    }
    puts("Usage: umicom-study --self-test | demo | lessons | plan LESSON_ID | library [TEXT] | library-html");
    puts("Planning uses a new memory-only practice record; library-html writes UTF-8 to stdout only.");
    return argc==2&&strcmp(argv[1],"--help")==0?0:2;
}
