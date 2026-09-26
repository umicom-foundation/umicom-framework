/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/workspace.c
 * PURPOSE: Provide the Framework-owned Education Workspace capability.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "private.h"
#include "umicom/teacher/assessment_scoring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Strict UTF-8 validation also prevents embedded ASCII control characters in
 * names/notes other than tab and line breaks. Byte capacities include NUL. */
bool EwTextValid(const char *text, size_t capacity, bool allowEmpty)
{
    size_t n=0U, i=0U;
    if (text==NULL) return false;
    while (n<capacity && text[n]!='\0') ++n;
    if (n==capacity || (!allowEmpty && n==0U)) return false;
    while (i<n) {
        unsigned char c=(unsigned char)text[i++];
        uint32_t value; size_t extra;
        if (c<0x80U) {
            if ((c<32U && c!='\n' && c!='\r' && c!='\t') || c==127U) return false;
            continue;
        }
        if (c>=0xC2U && c<=0xDFU) {value=c&0x1FU;extra=1U;}
        else if (c>=0xE0U && c<=0xEFU) {value=c&0x0FU;extra=2U;}
        else if (c>=0xF0U && c<=0xF4U) {value=c&0x07U;extra=3U;}
        else return false;
        if (extra>n-i) return false;
        size_t count=extra;
        while (extra-- >0U) {
            unsigned char d=(unsigned char)text[i++];
            if ((d&0xC0U)!=0x80U) return false;
            value=(value<<6U)|(d&0x3FU);
        }
        if ((count==2U && value<0x800U) || (count==3U && value<0x10000U) ||
            (value>=0xD800U && value<=0xDFFFU) || value>0x10FFFFU) return false;
    }
    return true;
}
bool EwIdValid(const char *text)
{
    if (!EwTextValid(text,UMI_EDUCATION_ID_CAPACITY,false)) return false;
    for (size_t i=0U;text[i]!='\0';++i) {
        char c=text[i];
        if (!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'))
            return false;
    }
    return true;
}
size_t EwLessonIndex(const char *id)
{
    if (id==NULL) return SIZE_MAX;
    for (size_t i=0U;i<UMI_EDUCATION_LESSONS;++i)
        if (strcmp(UmiEducationLessonAt(i)->id,id)==0) return i;
    return SIZE_MAX;
}
static bool Prerequisites(const EwState *state,size_t index)
{
    return index==0U || strcmp(UmiEducationLessonAt(index)->courseId,
        UmiEducationLessonAt(index-1U)->courseId)!=0 || state->lessons[index-1U].quizPassed;
}
UmiStatus EwApply(EwState *state,EwAction action,size_t index,const char *payload,
    UmiEducationFeedback *feedback)
{
    UmiEducationProgress *progress;
    if (state==NULL || index>=UMI_EDUCATION_LESSONS || payload==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    progress=&state->lessons[index];
    switch (action) {
    case EW_READ: if (payload[0]!='\0') return UMI_STATUS_PARSE_ERROR; progress->read=true; break;
    case EW_HINT: if (payload[0]!='\0') return UMI_STATUS_PARSE_ERROR; progress->hintViewed=true; break;
    case EW_NOTE:
        if (!EwTextValid(payload,UMI_EDUCATION_NOTE_CAPACITY,true)) return UMI_STATUS_INVALID_ARGUMENT;
        memcpy(progress->note,payload,strlen(payload)+1U); break;
    case EW_QUIZ: {
        uint32_t correct=0U;
        UmiTeacherAssessmentScoring rubric;
        const UmiEducationLesson *lesson=UmiEducationLessonAt(index);
        if (!progress->read || !Prerequisites(state,index)) return UMI_STATUS_INVALID_STATE;
        if (strlen(payload)!=UMI_EDUCATION_QUESTIONS) return UMI_STATUS_PARSE_ERROR;
        for (size_t i=0U;i<UMI_EDUCATION_QUESTIONS;++i) {
            if (payload[i]<'0'||payload[i]>'2') return UMI_STATUS_PARSE_ERROR;
            bool match=(uint32_t)(payload[i]-'0')==lesson->questions[i].correctChoice;
            if (match) ++correct;
            if (feedback!=NULL) {
                feedback->correct[i]=match;
                feedback->explanations[i]=lesson->questions[i].explanation;
            }
        }
        umi_teacher_assessment_scoring_init(&rubric);
        if (umi_teacher_assessment_scoring_configure(&rubric,100U,0U,0U,100U)!=UMI_STATUS_OK)
            return UMI_STATUS_INTERNAL_ERROR;
        progress->latestScore=umi_teacher_assessment_scoring_compute(&rubric,
            umi_teacher_percent(correct,UMI_EDUCATION_QUESTIONS),0U,0U);
        if (progress->latestScore>progress->bestScore) progress->bestScore=progress->latestScore;
        progress->quizPassed=umi_teacher_assessment_scoring_passes(&rubric,progress->bestScore)!=0;
        if (progress->attempts==UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        ++progress->attempts;
        if (feedback!=NULL) {feedback->score=progress->latestScore; feedback->passed=progress->latestScore==100U;}
        break;
    }
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiEducationOpen(UmiDataServer *server,const char *id,const char *displayName,
    UmiEducationWorkspace **outWorkspace)
{
    if (outWorkspace==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWorkspace=NULL;
    if (server==NULL || !EwIdValid(id) || !EwTextValid(displayName,UMI_EDUCATION_NAME_CAPACITY,false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_data_server_in_transaction(server)) return UMI_STATUS_BUSY;
    UmiStatus status=UmiEducationCatalogueValidate();
    if (status!=UMI_STATUS_OK) return status;
    UmiEducationWorkspace *w=calloc(1U,sizeof *w);
    if (w==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    w->server=server;
    memcpy(w->learnerId,id,strlen(id)+1U);
    memcpy(w->state.displayName,displayName,strlen(displayName)+1U);
    int n=snprintf(w->prefix,sizeof w->prefix,"education/%s/",id);
    if (n<0 || (size_t)n>=sizeof w->prefix) {free(w);return UMI_STATUS_CAPACITY_EXCEEDED;}
    status=UmiEducationReload(w);
    if (status!=UMI_STATUS_OK) {free(w);return status;}
    *outWorkspace=w;return UMI_STATUS_OK;
}
void UmiEducationClose(UmiEducationWorkspace *workspace) {free(workspace);}
UmiStatus UmiEducationReload(UmiEducationWorkspace *w)
{
    if (w==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (w->recoveryRequired) return UMI_STATUS_INVALID_STATE;
    if (umi_data_server_in_transaction(w->server)) return UMI_STATUS_BUSY;
    EwState *fresh=calloc(1U,sizeof *fresh);
    if (fresh==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(fresh->displayName,w->state.displayName,sizeof fresh->displayName);
    /* A read transaction gives a coherent manifest plus event sequence on a
     * separate SQLite connection. No learning action is executed by reload. */
    UmiStatus status=umi_data_server_begin(w->server);
    if (status==UMI_STATUS_OK) {
        status=EwLoad(w,fresh);
        UmiStatus rollback=umi_data_server_rollback(w->server);
        if (rollback!=UMI_STATUS_OK) {w->recoveryRequired=true;status=rollback;}
    }
    if (status==UMI_STATUS_OK) w->state=*fresh;
    free(fresh);return status;
}
UmiStatus UmiEducationSnapshotRead(const UmiEducationWorkspace *w,UmiEducationSnapshot *out)
{
    if (w==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    memcpy(out->learnerId,w->learnerId,sizeof out->learnerId);
    memcpy(out->displayName,w->state.displayName,sizeof out->displayName);
    out->revision=w->state.revision;out->recoveryRequired=w->recoveryRequired;
    out->remainingChanges=(uint32_t)(UMI_EDUCATION_MAX_EVENTS-w->state.revision);
    for (size_t i=0U;i<UMI_EDUCATION_LESSONS;++i) {
        out->readLessons+=w->state.lessons[i].read?1U:0U;
        out->passedQuizzes+=w->state.lessons[i].quizPassed?1U:0U;
    }
    out->quizCompletionPercent=umi_teacher_percent(out->passedQuizzes,UMI_EDUCATION_LESSONS);
    return UMI_STATUS_OK;
}
UmiStatus UmiEducationProgressRead(const UmiEducationWorkspace *w,const char *id,UmiEducationProgress *out)
{
    if (w==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t i=EwLessonIndex(id);if (i==SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out=w->state.lessons[i];out->prerequisitesMet=Prerequisites(&w->state,i);
    return UMI_STATUS_OK;
}
static UmiStatus Change(UmiEducationWorkspace *w,const char *id,EwAction action,
    const char *payload,UmiEducationFeedback *feedback)
{
    if (w==NULL || payload==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (w->recoveryRequired) return UMI_STATUS_INVALID_STATE;
    size_t i=EwLessonIndex(id);if (i==SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return EwCommit(w,action,i,payload,feedback);
}
UmiStatus UmiEducationMarkRead(UmiEducationWorkspace *w,const char *id)
{return Change(w,id,EW_READ,"",NULL);}
UmiStatus UmiEducationRevealHint(UmiEducationWorkspace *w,const char *id)
{return Change(w,id,EW_HINT,"",NULL);}
UmiStatus UmiEducationSaveNote(UmiEducationWorkspace *w,const char *id,const char *note)
{
    if (!EwTextValid(note,UMI_EDUCATION_NOTE_CAPACITY,true)) return UMI_STATUS_INVALID_ARGUMENT;
    return Change(w,id,EW_NOTE,note,NULL);
}
UmiStatus UmiEducationSubmitQuiz(UmiEducationWorkspace *w,const char *id,
    const uint32_t answers[UMI_EDUCATION_QUESTIONS],UmiEducationFeedback *feedback)
{
    char payload[UMI_EDUCATION_QUESTIONS+1U]={0};
    if (feedback==NULL || answers==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(feedback,0,sizeof *feedback);
    for (size_t i=0U;i<UMI_EDUCATION_QUESTIONS;++i) {
        if (answers[i]>=UMI_EDUCATION_CHOICES) return UMI_STATUS_INVALID_ARGUMENT;
        payload[i]=(char)('0'+answers[i]);
    }
    return Change(w,id,EW_QUIZ,payload,feedback);
}
