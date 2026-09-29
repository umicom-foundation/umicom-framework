/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/study.c
 * PURPOSE: Project canonical lesson progress into an immutable study route.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education_workspace/study.h"
#include "private.h"
#include "study_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiEducationStudy {
    UmiEducationSnapshot snapshot;
    UmiEducationStudyItem items[UMI_EDUCATION_LESSONS];
};
static bool Course(const char *id)
{
    if (!EwTextValid(id,UMI_EDUCATION_ID_CAPACITY,true)) return false;
    if (*id=='\0') return true;
    for (size_t i=0U;i<UmiEducationCourseCount();++i)
        if (strcmp(UmiEducationCourseAt(i)->id,id)==0) return true;
    return false;
}
static unsigned char Fold(unsigned char c)
{return c>='A'&&c<='Z'?(unsigned char)(c+('a'-'A')):c;}
/* Internal reuse avoids allocating lower-case copies and does not depend on
 * the process locale. Both inputs must already be bounded valid text. */
bool EwStudyContains(const char *text,const char *query)
{
    if (*query=='\0') return true;
    for (;*text!='\0';++text) {
        size_t n=0U;
        while (query[n]!='\0'&&text[n]!='\0'&&
            Fold((unsigned char)text[n])==Fold((unsigned char)query[n])) ++n;
        if (query[n]=='\0') return true;
    }
    return false;
}
UmiStatus UmiEducationStudyCapture(const UmiEducationWorkspace *workspace,UmiEducationStudy **out)
{
    if (workspace==NULL||out==NULL||*out!=NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiEducationCatalogueValidate();
    if (status!=UMI_STATUS_OK) return status;
    UmiEducationStudy *candidate=calloc(1U,sizeof *candidate);
    if (candidate==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status=UmiEducationSnapshotRead(workspace,&candidate->snapshot);
    for (size_t i=0U;status==UMI_STATUS_OK&&i<UMI_EDUCATION_LESSONS;++i) {
        UmiEducationProgress progress={0};
        UmiEducationStudyItem *item=&candidate->items[i];
        item->lesson=UmiEducationLessonAt(i);item->catalogueIndex=i;
        status=UmiEducationProgressRead(workspace,item->lesson->id,&progress);
        if (status!=UMI_STATUS_OK) break;
        item->read=progress.read;item->hintViewed=progress.hintViewed;
        item->quizPassed=progress.quizPassed;item->prerequisitesMet=progress.prerequisitesMet;
        item->attempts=progress.attempts;item->latestScore=progress.latestScore;item->bestScore=progress.bestScore;
        item->action=progress.quizPassed?UMI_EDUCATION_STUDY_COMPLETE:
            !progress.prerequisitesMet?UMI_EDUCATION_STUDY_PREREQUISITE:
            !progress.read?UMI_EDUCATION_STUDY_READ:UMI_EDUCATION_STUDY_QUIZ;
    }
    if (status!=UMI_STATUS_OK) {free(candidate);return status;}
    *out=candidate;return UMI_STATUS_OK;
}
void UmiEducationStudyDestroy(UmiEducationStudy *study) {free(study);}
UmiStatus UmiEducationStudySnapshotRead(const UmiEducationStudy *study,UmiEducationSnapshot *out)
{
    if (study==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out=study->snapshot;return UMI_STATUS_OK;
}
UmiStatus UmiEducationStudyItemRead(const UmiEducationStudy *study,size_t index,UmiEducationStudyItem *out)
{
    if (study==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index>=UMI_EDUCATION_LESSONS) return UMI_STATUS_NOT_FOUND;
    *out=study->items[index];return UMI_STATUS_OK;
}
UmiStatus UmiEducationStudySearch(const UmiEducationStudy *study,
    const UmiEducationStudyQuery *query,UmiEducationStudyMatches *out)
{
    if (study==NULL||query==NULL||out==NULL||!Course(query->courseId)||
        !EwTextValid(query->text,UMI_EDUCATION_SEARCH_CAPACITY,true)||
        query->filter<UMI_EDUCATION_STUDY_ALL||query->filter>UMI_EDUCATION_STUDY_UNREAD)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEducationStudyMatches result={0};
    for (size_t i=0U;i<UMI_EDUCATION_LESSONS;++i) {
        const UmiEducationStudyItem *item=&study->items[i];
        const UmiEducationLesson *lesson=item->lesson;
        if (*query->courseId!='\0'&&strcmp(query->courseId,lesson->courseId)!=0) continue;
        bool ready=item->prerequisitesMet&&!item->quizPassed&&!study->snapshot.recoveryRequired;
        if ((query->filter==UMI_EDUCATION_STUDY_READY&&!ready)||
            (query->filter==UMI_EDUCATION_STUDY_BLOCKED&&(item->quizPassed||
                (item->prerequisitesMet&&!study->snapshot.recoveryRequired)))||
            (query->filter==UMI_EDUCATION_STUDY_PASSED&&!item->quizPassed)||
            (query->filter==UMI_EDUCATION_STUDY_UNREAD&&item->read)) continue;
        if (!EwStudyContains(lesson->id,query->text)&&!EwStudyContains(lesson->title,query->text)&&
            !EwStudyContains(lesson->courseId,query->text)&&!EwStudyContains(lesson->explanation,query->text)&&
            !EwStudyContains(lesson->practice,query->text)&&!EwStudyContains(lesson->projectId,query->text)) continue;
        result.indices[result.count++]=i;
    }
    *out=result;return UMI_STATUS_OK;
}
UmiStatus UmiEducationStudyPlan(const UmiEducationStudy *study,const char *id,UmiEducationStudyRoute *out)
{
    if (study==NULL||out==NULL||!EwTextValid(id,UMI_EDUCATION_SEARCH_CAPACITY,false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (study->snapshot.recoveryRequired) return UMI_STATUS_INVALID_STATE;
    size_t goal=UMI_EDUCATION_LESSONS;
    for (size_t i=0U;i<UMI_EDUCATION_LESSONS;++i)
        if (strcmp(study->items[i].lesson->id,id)==0) {goal=i;break;}
    if (goal==UMI_EDUCATION_LESSONS) return UMI_STATUS_NOT_FOUND;
    UmiEducationStudyRoute result={0};
    result.goalIndex=goal;result.capturedRevision=study->snapshot.revision;
    const char *course=study->items[goal].lesson->courseId;
    for (size_t i=0U;i<=goal;++i) {
        const UmiEducationStudyItem *item=&study->items[i];
        if (strcmp(item->lesson->courseId,course)!=0||item->quizPassed) continue;
        if (UINT32_MAX-result.estimatedMinutes<item->lesson->minutes) return UMI_STATUS_CAPACITY_EXCEEDED;
        result.indices[result.count++]=i;result.estimatedMinutes+=item->lesson->minutes;
    }
    *out=result;return UMI_STATUS_OK;
}
UmiStatus UmiEducationStudyNext(const UmiEducationStudy *study,const char *course,UmiEducationStudyItem *out)
{
    if (study==NULL||out==NULL||!Course(course)) return UMI_STATUS_INVALID_ARGUMENT;
    if (study->snapshot.recoveryRequired) return UMI_STATUS_INVALID_STATE;
    for (size_t i=0U;i<UMI_EDUCATION_LESSONS;++i) {
        const UmiEducationStudyItem *item=&study->items[i];
        if ((*course=='\0'||strcmp(course,item->lesson->courseId)==0)&&!item->quizPassed) {
            *out=*item;return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
const char *UmiEducationStudyActionText(UmiEducationStudyAction action)
{
    switch(action) {
    case UMI_EDUCATION_STUDY_READ:return "Read the lesson";
    case UMI_EDUCATION_STUDY_QUIZ:return "Try the practice quiz";
    case UMI_EDUCATION_STUDY_PREREQUISITE:return "Complete the earlier quiz first";
    case UMI_EDUCATION_STUDY_COMPLETE:return "Practice quiz passed";
    default:return "Unknown study action";
    }
}
