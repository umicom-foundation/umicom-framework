/* Umicom Framework | Education Workspace regression tests
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Assertions remain active in Release; tests use the actual Data Server. */
#include "umicom/education_workspace/workspace.h"
#include "umicom/education_workspace/projects.h"
#include "umicom/teacher/assessment_scoring.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
typedef struct Fixture {UmiDataServer *server;UmiEducationWorkspace *workspace;UmiEducationWorkspace *other;UmiDataServer *second;} Fixture;
static const char *First(void){return UmiEducationLessonAt(0U)->id;}
static UmiStatus Open(Fixture *f){return UmiEducationOpen(f->server,"learner","Workshop learner",&f->workspace);}
static UmiStatus Pass(Fixture *f,size_t index)
{
    const UmiEducationLesson *l=UmiEducationLessonAt(index);uint32_t answers[3];UmiEducationFeedback feedback;
    for(size_t i=0U;i<3U;++i)answers[i]=l->questions[i].correctChoice;
    UmiStatus s=UmiEducationMarkRead(f->workspace,l->id);
    return s==UMI_STATUS_OK?UmiEducationSubmitQuiz(f->workspace,l->id,answers,&feedback):s;
}
static uint64_t Revision(Fixture *f){UmiEducationSnapshot s;(void)UmiEducationSnapshotRead(f->workspace,&s);return s.revision;}
static int Run(const char *name,Fixture *f)
{
    UmiEducationSnapshot snapshot;UmiEducationProgress progress;UmiEducationFeedback feedback;
    if(strcmp(name,"catalogue")==0){
        CHECK(UmiEducationCatalogueValidate()==UMI_STATUS_OK);CHECK(UmiEducationCourseCount()==3U);
        CHECK(UmiEducationCourseAt(3U)==NULL);CHECK(UmiEducationLessonAt(12U)==NULL);
        CHECK(UmiEducationLessonFind("absent")==NULL);CHECK(UmiEducationLessonFind(NULL)==NULL);
        for(size_t i=0U;i<12U;++i)CHECK(UmiEducationLessonFind(UmiEducationLessonAt(i)->id)==UmiEducationLessonAt(i));
        return 0;
    }
    if(strcmp(name,"percentage_boundaries")==0){
        CHECK(umi_teacher_percent(UINT64_MAX-1U,UINT64_MAX)==99U);
        CHECK(umi_teacher_percent(UINT64_MAX/2U,UINT64_MAX)==49U);
        CHECK(umi_teacher_percent(UINT64_MAX/2U+1U,UINT64_MAX)==50U);
        CHECK(umi_teacher_percent(0U,0U)==0U);CHECK(umi_teacher_percent(100U,0U)==0U);
        CHECK(umi_teacher_percent(1U,3U)==33U);CHECK(umi_teacher_percent(UINT64_MAX,1U)==100U);return 0;
    }
    if(strcmp(name,"percentage_reference")==0){
        for(uint64_t m=1U;m<300U;++m) {
            for(uint64_t v=0U;v<=m;++v) { CHECK(umi_teacher_percent(v,m)==(uint32_t)(v*100U/m)); }
        }
        return 0;
    }
    if(strcmp(name,"scoring_wraparound")==0){
        UmiTeacherAssessmentScoring r;umi_teacher_assessment_scoring_init(&r);UmiTeacherAssessmentScoring before=r;
        CHECK(umi_teacher_assessment_scoring_configure(&r,UINT32_MAX,101U,0U,70U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&r,&before,sizeof r)==0);
        CHECK(umi_teacher_assessment_scoring_configure(&r,100U,0U,0U,100U)==UMI_STATUS_OK);
        CHECK(umi_teacher_assessment_scoring_compute(&r,66U,100U,100U)==66U);
        CHECK(!umi_teacher_assessment_scoring_passes(&r,66U));return 0;
    }
    if(strcmp(name,"scoring_default")==0){
        UmiTeacherAssessmentScoring r;umi_teacher_assessment_scoring_init(&r);
        CHECK(umi_teacher_assessment_scoring_compute(&r,100U,100U,100U)==100U);
        CHECK(umi_teacher_assessment_scoring_compute(&r,50U,50U,50U)==50U);
        CHECK(umi_teacher_assessment_scoring_configure(&r,60U,25U,15U,101U)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    CHECK(Open(f)==UMI_STATUS_OK);
    if(strcmp(name,"open_no_write")==0){CHECK(umi_data_server_count(f->server)==0U);CHECK(Revision(f)==0U);return 0;}
    if(strcmp(name,"invalid_learner")==0){
        const char *bad[]={"","../escape","a/b","a.b","a b","x:y","a|b","\xc0\x80"};
        for(size_t i=0U;i<sizeof bad/sizeof bad[0];++i){UmiEducationWorkspace *w=NULL;CHECK(UmiEducationOpen(f->server,bad[i],"Name",&w)==UMI_STATUS_INVALID_ARGUMENT);CHECK(w==NULL);}
        char longId[60];memset(longId,'a',59U);longId[59]='\0';
        CHECK(UmiEducationOpen(f->server,longId,"Name",&f->other)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strcmp(name,"invalid_name")==0){
        CHECK(UmiEducationOpen(f->server,"valid","",&f->other)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationOpen(f->server,"valid","\xed\xa0\x80",&f->other)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationOpen(NULL,"valid","Name",&f->other)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strcmp(name,"unknown_lesson")==0){
        CHECK(UmiEducationMarkRead(f->workspace,"unknown")==UMI_STATUS_NOT_FOUND);
        CHECK(UmiEducationProgressRead(f->workspace,NULL,&progress)==UMI_STATUS_NOT_FOUND);CHECK(Revision(f)==0U);return 0;
    }
    if(strcmp(name,"read_idempotent")==0){
        CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_OK);CHECK(Revision(f)==1U);
        CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_OK);CHECK(Revision(f)==1U);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK && progress.read && !progress.quizPassed);return 0;
    }
    if(strcmp(name,"hint_idempotent")==0){
        CHECK(UmiEducationRevealHint(f->workspace,First())==UMI_STATUS_OK);
        CHECK(UmiEducationRevealHint(f->workspace,First())==UMI_STATUS_OK);CHECK(Revision(f)==1U);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK && progress.hintViewed && !progress.quizPassed);return 0;
    }
    if(strcmp(name,"notes_roundtrip")==0){
        const char *note="Room plan\nمرحبا\n<angle> & | quotes \" '";
        CHECK(UmiEducationSaveNote(f->workspace,First(),note)==UMI_STATUS_OK);
        CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_OK);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK);CHECK(strcmp(progress.note,note)==0);return 0;
    }
    if(strcmp(name,"notes_capacity")==0){
        char note[UMI_EDUCATION_NOTE_CAPACITY+1U];memset(note,'n',sizeof note);note[UMI_EDUCATION_NOTE_CAPACITY-1U]='\0';
        CHECK(UmiEducationSaveNote(f->workspace,First(),note)==UMI_STATUS_OK);
        note[UMI_EDUCATION_NOTE_CAPACITY-1U]='n';note[UMI_EDUCATION_NOTE_CAPACITY]='\0';
        CHECK(UmiEducationSaveNote(f->workspace,First(),note)==UMI_STATUS_INVALID_ARGUMENT);CHECK(Revision(f)==1U);return 0;
    }
    if(strcmp(name,"notes_invalid_utf8")==0){
        const char *bad[]={"\x80","\xc0\xaf","\xf4\x90\x80\x80","\xe2\x82","bad\x01","\xed\xbf\xbf"};
        for(size_t i=0U;i<sizeof bad/sizeof bad[0];++i)CHECK(UmiEducationSaveNote(f->workspace,First(),bad[i])==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Revision(f)==0U);return 0;
    }
    if(strcmp(name,"notes_idempotent_clear")==0){
        CHECK(UmiEducationSaveNote(f->workspace,First(),"")==UMI_STATUS_OK && Revision(f)==0U);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"Text")==UMI_STATUS_OK);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"Text")==UMI_STATUS_OK && Revision(f)==1U);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"")==UMI_STATUS_OK && Revision(f)==2U);return 0;
    }
    if(strcmp(name,"quiz_requires_read")==0){
        uint32_t a[]={1U,2U,0U};CHECK(UmiEducationSubmitQuiz(f->workspace,First(),a,&feedback)==UMI_STATUS_INVALID_STATE);CHECK(Revision(f)==0U);return 0;
    }
    if(strcmp(name,"quiz_invalid_answers")==0){
        uint32_t a[]={0U,3U,0U};CHECK(UmiEducationSubmitQuiz(f->workspace,First(),a,&feedback)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationSubmitQuiz(f->workspace,First(),NULL,&feedback)==UMI_STATUS_INVALID_ARGUMENT);CHECK(Revision(f)==0U);return 0;
    }
    if(strcmp(name,"quiz_prerequisite")==0){
        CHECK(UmiEducationMarkRead(f->workspace,"notes.contracts")==UMI_STATUS_OK);
        uint32_t a[]={1U,0U,2U};CHECK(UmiEducationSubmitQuiz(f->workspace,"notes.contracts",a,&feedback)==UMI_STATUS_INVALID_STATE);
        CHECK(Pass(f,0U)==UMI_STATUS_OK);CHECK(UmiEducationSubmitQuiz(f->workspace,"notes.contracts",a,&feedback)==UMI_STATUS_OK && feedback.passed);return 0;
    }
    if(strcmp(name,"courses_independent")==0){
        CHECK(Pass(f,4U)==UMI_STATUS_OK);CHECK(Pass(f,8U)==UMI_STATUS_OK);CHECK(Revision(f)==4U);return 0;
    }
    if(strcmp(name,"quiz_feedback")==0){
        CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_OK);
        uint32_t a[]={1U,2U,1U};CHECK(UmiEducationSubmitQuiz(f->workspace,First(),a,&feedback)==UMI_STATUS_OK);
        CHECK(feedback.score==66U && !feedback.passed && feedback.correct[0] && feedback.correct[1] && !feedback.correct[2]);
        CHECK(strstr(feedback.explanations[2],"success")!=NULL);return 0;
    }
    if(strcmp(name,"quiz_best_retained")==0){
        CHECK(Pass(f,0U)==UMI_STATUS_OK);uint32_t a[]={0U,0U,2U};
        CHECK(UmiEducationSubmitQuiz(f->workspace,First(),a,&feedback)==UMI_STATUS_OK && feedback.score==0U);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK);
        CHECK(progress.bestScore==100U && progress.latestScore==0U && progress.quizPassed && progress.attempts==2U);return 0;
    }
    if(strcmp(name,"course_completion")==0){
        for(size_t i=0U;i<12U;++i)CHECK(Pass(f,i)==UMI_STATUS_OK);
        CHECK(UmiEducationSnapshotRead(f->workspace,&snapshot)==UMI_STATUS_OK);
        CHECK(snapshot.passedQuizzes==12U && snapshot.quizCompletionPercent==100U && snapshot.readLessons==12U);return 0;
    }
    if(strcmp(name,"resume_progress")==0){
        CHECK(Pass(f,0U)==UMI_STATUS_OK);CHECK(UmiEducationSaveNote(f->workspace,First(),"A saved explanation")==UMI_STATUS_OK);
        UmiEducationClose(f->workspace);f->workspace=NULL;CHECK(Open(f)==UMI_STATUS_OK);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK && progress.quizPassed);
        CHECK(strcmp(progress.note,"A saved explanation")==0 && Revision(f)==3U);return 0;
    }
    if(strcmp(name,"stale_writer")==0 || strcmp(name,"stale_noop")==0){
        CHECK(UmiEducationOpen(f->server,"learner","Other label",&f->other)==UMI_STATUS_OK);
        CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_OK);
        CHECK(UmiEducationMarkRead(f->other,First())==UMI_STATUS_BUSY);
        CHECK(UmiEducationReload(f->other)==UMI_STATUS_OK);
        CHECK(UmiEducationMarkRead(f->other,First())==UMI_STATUS_OK);
        if(strcmp(name,"stale_noop")==0){CHECK(UmiEducationSaveNote(f->workspace,First(),"Changed")==UMI_STATUS_OK);CHECK(UmiEducationMarkRead(f->other,First())==UMI_STATUS_BUSY);}
        return 0;
    }
    if(strcmp(name,"learner_isolation")==0){
        CHECK(Pass(f,0U)==UMI_STATUS_OK);CHECK(UmiEducationOpen(f->server,"second","Second",&f->other)==UMI_STATUS_OK);
        CHECK(UmiEducationProgressRead(f->other,First(),&progress)==UMI_STATUS_OK && !progress.read && progress.attempts==0U);return 0;
    }
    if(strcmp(name,"stored_name")==0){
        CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_OK);CHECK(UmiEducationOpen(f->server,"learner","Renamed",&f->other)==UMI_STATUS_OK);
        CHECK(UmiEducationSnapshotRead(f->other,&snapshot)==UMI_STATUS_OK && strcmp(snapshot.displayName,"Workshop learner")==0);return 0;
    }
    if(strcmp(name,"transaction_busy")==0){
        CHECK(umi_data_server_begin(f->server)==UMI_STATUS_OK);CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_BUSY);
        CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_BUSY);CHECK(umi_data_server_rollback(f->server)==UMI_STATUS_OK);return 0;
    }
    if(strcmp(name,"journal_capacity")==0){
        for(size_t i=0U;i<UMI_EDUCATION_MAX_EVENTS;++i)CHECK(UmiEducationSaveNote(f->workspace,First(),i%2U==0U?"A":"B")==UMI_STATUS_OK);
        CHECK(Revision(f)==UMI_EDUCATION_MAX_EVENTS);CHECK(UmiEducationSaveNote(f->workspace,First(),"overflow")==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"B")==UMI_STATUS_OK);CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_OK);return 0;
    }
    if(strcmp(name,"report_escaping")==0 || strcmp(name,"report_capacity")==0){
        CHECK(UmiEducationSaveNote(f->workspace,First(),"<script>alert('x')</script> & \"safe\"")==UMI_STATUS_OK);
        size_t required=0U;CHECK(UmiEducationReportHtml(f->workspace,NULL,0U,&required)==UMI_STATUS_OK && required>100U);
        char *html=malloc(required);CHECK(html!=NULL);
        if(strcmp(name,"report_capacity")==0){UmiStatus s=UmiEducationReportHtml(f->workspace,html,required-1U,&required);int ok=s==UMI_STATUS_CAPACITY_EXCEEDED&&html[0]=='\0';free(html);CHECK(ok);}
        else {UmiStatus s=UmiEducationReportHtml(f->workspace,html,required,&required);int ok=s==UMI_STATUS_OK&&strstr(html,"&lt;script&gt;")&&strstr(html,"data:image/png;base64,")&&!strstr(html,"<script>");free(html);CHECK(ok);}return 0;
    }
    if(strncmp(name,"corrupt_",8U)==0){
        CHECK(UmiEducationMarkRead(f->workspace,First())==UMI_STATUS_OK);
        const char *key="education/learner/event/1",*value=NULL;
        if(strcmp(name,"corrupt_format")==0)value="E2|1|0|";
        else if(strcmp(name,"corrupt_index")==0)value="E1|1|99|";
        else if(strcmp(name,"corrupt_action")==0)value="E1|9|0|";
        else if(strcmp(name,"corrupt_hex")==0)value="E1|3|0|zz";
        else if(strcmp(name,"corrupt_null")==0)value="E1|3|0|00";
        else if(strcmp(name,"corrupt_utf8")==0)value="E1|3|0|c0af";
        else if(strcmp(name,"corrupt_quiz_state")==0)value="E1|4|0|313230";
        else if(strcmp(name,"corrupt_leading_zero")==0)value="E1|01|0|";
        else if(strcmp(name,"corrupt_unknown_record")==0){key="education/learner/surprise";value="extra";}
        else if(strcmp(name,"corrupt_extra_event")==0){key="education/learner/event/2";value="E1|1|0|";}
        else if(strcmp(name,"corrupt_catalogue")==0){key="education/learner/head";value="E1|2|1|41";}
        else if(strcmp(name,"corrupt_count")==0){key="education/learner/head";value="E1|1|999|41";}
        else if(strcmp(name,"corrupt_missing_event")==0){CHECK(umi_data_server_delete(f->server,key)==UMI_STATUS_OK);}
        else {CHECK(0);}
        if(value!=NULL)CHECK(umi_data_server_set(f->server,key,value)==UMI_STATUS_OK);
        CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_PARSE_ERROR);CHECK(Revision(f)==1U);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK && progress.read);return 0;
    }
    if(strcmp(name,"orphan_event")==0){
        CHECK(umi_data_server_set(f->server,"education/learner/event/1","E1|1|0|")==UMI_STATUS_OK);
        CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_PARSE_ERROR && Revision(f)==0U);return 0;
    }
    if(strcmp(name,"unrelated_records")==0){
        CHECK(umi_data_server_set(f->server,"other/value","Unknown data")==UMI_STATUS_OK);
        CHECK(Pass(f,0U)==UMI_STATUS_OK);CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_OK);return 0;
    }
    if(strcmp(name,"project_catalogue")==0){
        const char *projects[]={"notes","assembly","framework"};
        for(size_t p=0U;p<3U;++p){size_t count=UmiEducationProjectFileCount(projects[p]);CHECK(count>=6U);bool cmake=false;
            for(size_t i=0U;i<count;++i){UmiEducationProjectFile file;CHECK(UmiEducationProjectFileAt(projects[p],i,&file)==UMI_STATUS_OK);CHECK(file.size>0U);if(strcmp(file.name,"CMakeLists.txt")==0)cmake=true;}CHECK(cmake);}
        CHECK(UmiEducationProjectFileCount("unknown")==0U);return 0;
    }
    if(strcmp(name,"export_invalid_path")==0){
        size_t written=9U;CHECK(UmiEducationExportProject("notes","relative-path",&written)==UMI_STATUS_INVALID_ARGUMENT && written==0U);
        CHECK(UmiEducationExportProject("unknown","relative-path",&written)==UMI_STATUS_NOT_FOUND);return 0;
    }
    if(strcmp(name,"sqlite_restart")==0){
        CHECK(Pass(f,0U)==UMI_STATUS_OK);UmiEducationClose(f->workspace);f->workspace=NULL;
        umi_data_server_destroy(f->server);f->server=NULL;
        CHECK(umi_data_server_create_sqlite("workspace.sqlite",&f->server)==UMI_STATUS_OK);CHECK(Open(f)==UMI_STATUS_OK);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK && progress.quizPassed && progress.attempts==1U);return 0;
    }
    if(strcmp(name,"sqlite_connections")==0){
        CHECK(umi_data_server_create_sqlite("workspace.sqlite",&f->second)==UMI_STATUS_OK);
        CHECK(UmiEducationOpen(f->second,"learner","Name",&f->other)==UMI_STATUS_OK);
        CHECK(Pass(f,0U)==UMI_STATUS_OK);CHECK(UmiEducationMarkRead(f->other,First())==UMI_STATUS_BUSY);
        CHECK(UmiEducationReload(f->other)==UMI_STATUS_OK);CHECK(UmiEducationMarkRead(f->other,First())==UMI_STATUS_OK);return 0;
    }
    if(strcmp(name,"sqlite_write_rollback")==0 || strcmp(name,"sqlite_failure_second_write")==0){
        CHECK(Pass(f,0U)==UMI_STATUS_OK);uint64_t before=Revision(f);
        /* Fail either event insertion or manifest update using real SQLite. */
        const char *sql=strcmp(name,"sqlite_write_rollback")==0?
            "CREATE TRIGGER fail_event BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'education/%/event/%' BEGIN SELECT RAISE(ABORT,'injected event failure'); END;":
            "CREATE TRIGGER fail_head BEFORE UPDATE ON umicom_kv WHEN NEW.key LIKE 'education/%/head' BEGIN SELECT RAISE(ABORT,'injected manifest failure'); END;";
        CHECK(umi_data_server_execute(f->server,sql)==UMI_STATUS_OK);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"Not saved")==UMI_STATUS_IO_ERROR);
        CHECK(Revision(f)==before);CHECK(!umi_data_server_in_transaction(f->server));
        CHECK(UmiEducationReload(f->workspace)==UMI_STATUS_OK);CHECK(Revision(f)==before);
        CHECK(UmiEducationProgressRead(f->workspace,First(),&progress)==UMI_STATUS_OK && progress.note[0]=='\0');return 0;
    }
    if(strcmp(name,"sqlite_failed_rollback")==0){
        CHECK(Pass(f,0U)==UMI_STATUS_OK);
        CHECK(umi_data_server_execute(f->server,"CREATE TRIGGER lose_transaction BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'education/%/event/%' BEGIN SELECT RAISE(ROLLBACK,'injected transaction loss'); END;")==UMI_STATUS_OK);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"Not saved")!=UMI_STATUS_OK);
        CHECK(UmiEducationSnapshotRead(f->workspace,&snapshot)==UMI_STATUS_OK && snapshot.recoveryRequired);
        CHECK(UmiEducationSaveNote(f->workspace,First(),"Retry")==UMI_STATUS_INVALID_STATE);return 0;
    }
    fprintf(stderr,"Unknown test: %s\n",name);return 1;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    Fixture f={0};bool sqlite=strncmp(argv[1],"sqlite_",7U)==0;
#ifndef UMICOM_HAS_SQLITE
    if(sqlite){puts("SQLite backend disabled: case skipped");return 77;}
#endif
    if(sqlite){(void)remove("workspace.sqlite");(void)remove("workspace.sqlite-wal");(void)remove("workspace.sqlite-shm");}
    UmiStatus status=sqlite?umi_data_server_create_sqlite("workspace.sqlite",&f.server):umi_data_server_create_memory(&f.server);
    if(status!=UMI_STATUS_OK)return 1;
    int result=Run(argv[1],&f);
    UmiEducationClose(f.other);UmiEducationClose(f.workspace);
    umi_data_server_destroy(f.second);umi_data_server_destroy(f.server);
    if(result==0)printf("PASS %s\n",argv[1]);
    return result;
}
