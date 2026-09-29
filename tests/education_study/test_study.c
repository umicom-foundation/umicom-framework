/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_study/test_study.c
 * PURPOSE: Test real progress projections, immutable routes and storage conflicts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education_workspace/study.h"
#include "private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
#define OK(call) CHECK((call)==UMI_STATUS_OK)
typedef struct Fixture {UmiDataServer *server;UmiEducationWorkspace *workspace;UmiEducationStudy *a,*b;char *html;} Fixture;
static const char *Id(size_t i) {return UmiEducationLessonAt(i)->id;}
static UmiStatus Capture(Fixture *f) {return UmiEducationStudyCapture(f->workspace,&f->a);}
static UmiStatus Pass(Fixture *f,size_t i)
{
    const UmiEducationLesson *l=UmiEducationLessonAt(i);uint32_t answers[3];UmiEducationFeedback feedback;
    for(size_t j=0U;j<3U;++j)answers[j]=l->questions[j].correctChoice;
    UmiStatus s=UmiEducationMarkRead(f->workspace,l->id);
    return s==UMI_STATUS_OK?UmiEducationSubmitQuiz(f->workspace,l->id,answers,&feedback):s;
}
static uint64_t Revision(Fixture *f) {UmiEducationSnapshot s={0};(void)UmiEducationSnapshotRead(f->workspace,&s);return s.revision;}
static int Run(const char *name,Fixture *f)
{
    UmiEducationSnapshot snapshot={0};UmiEducationStudyItem item={0};UmiEducationStudyRoute route={0};
    UmiEducationStudyMatches matches={0};UmiEducationStudyQuery query={"","",UMI_EDUCATION_STUDY_ALL};
    if(strcmp(name,"capture_invalid")==0) {
        CHECK(UmiEducationStudyCapture(NULL,&f->a)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationStudyCapture(f->workspace,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strcmp(name,"capture_occupied")==0) {
        OK(Capture(f));UmiEducationStudy *same=f->a;
        CHECK(Capture(f)==UMI_STATUS_INVALID_ARGUMENT);CHECK(f->a==same);return 0;
    }
    if(strncmp(name,"state_",6U)==0||strcmp(name,"sqlite_changed_state")==0) {
        if(strcmp(name,"sqlite_changed_state")==0) {
            UmiEducationClose(f->workspace);f->workspace=NULL;umi_data_server_destroy(f->server);f->server=NULL;
            UmiStatus sql=umi_data_server_create_sqlite(":memory:",&f->server);
            if(sql==UMI_STATUS_NOT_IMPLEMENTED||sql==UMI_STATUS_UNAVAILABLE)return 77;
            OK(sql);OK(UmiEducationOpen(f->server,"learner","Workshop learner",&f->workspace));
        }
        if(strcmp(name,"state_equivalent_history")==0) {
            OK(UmiEducationSaveNote(f->workspace,Id(0),"one"));OK(UmiEducationSaveNote(f->workspace,Id(0),"two"));
            OK(umi_data_server_set(f->server,"education/learner/event/1","E1|3|0|646966666572656e74"));
            OK(UmiEducationMarkRead(f->workspace,Id(0)));CHECK(Revision(f)==3U);return 0;
        }
        if(strcmp(name,"state_changed_note")==0) {
            OK(UmiEducationSaveNote(f->workspace,Id(0),"original"));
            OK(umi_data_server_set(f->server,"education/learner/event/1","E1|3|0|6368616e676564"));
        } else {
            OK(UmiEducationMarkRead(f->workspace,Id(0)));
            if(strcmp(name,"state_other_learner")==0) {
                OK(umi_data_server_set(f->server,"education/another/event/1","unrelated"));
                OK(UmiEducationMarkRead(f->workspace,Id(2)));CHECK(Revision(f)==2U);return 0;
            }
            if(strcmp(name,"state_changed_name")==0)
                OK(umi_data_server_set(f->server,"education/learner/head","E1|1|1|4368616e676564"));
            else OK(umi_data_server_set(f->server,"education/learner/event/1","E1|1|1|"));
        }
        size_t before=umi_data_server_count(f->server);
        UmiStatus result=UmiEducationMarkRead(f->workspace,Id(strcmp(name,"state_changed_noop")==0?0U:2U));
        CHECK(result==UMI_STATUS_BUSY);CHECK(Revision(f)==1U);CHECK(umi_data_server_count(f->server)==before);
        CHECK(!umi_data_server_in_transaction(f->server));
        UmiEducationProgress p={0};OK(UmiEducationProgressRead(f->workspace,Id(2),&p));CHECK(!p.read);
        OK(UmiEducationReload(f->workspace));OK(UmiEducationMarkRead(f->workspace,Id(2)));CHECK(Revision(f)==2U);return 0;
    }
    if(strncmp(name,"library_",8U)==0) {
        UmiEducationLibraryMatches lib={0};
        if(strcmp(name,"library_catalogue")==0) {
            OK(UmiEducationLibraryValidate());CHECK(UmiEducationLibraryCount()==22U);
            CHECK(UmiEducationLibraryAt(SIZE_MAX)==NULL);CHECK(UmiEducationLibraryAt(22U)==NULL);return 0;
        }
        if(strcmp(name,"library_search")==0) {
            OK(UmiEducationLibrarySearch("",&lib));CHECK(lib.count==UmiEducationLibraryCount());
            OK(UmiEducationLibrarySearch("PCM16",&lib));CHECK(lib.count==1U);
            CHECK(strcmp(UmiEducationLibraryAt(lib.indices[0])->id,"audio")==0);
            OK(UmiEducationLibrarySearch("missing-needle-999",&lib));CHECK(lib.count==0U);return 0;
        }
        if(strcmp(name,"library_invalid")==0) {
            memset(&lib,0x5A,sizeof lib);UmiEducationLibraryMatches saved=lib;
            CHECK(UmiEducationLibrarySearch(NULL,&lib)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiEducationLibrarySearch("\xff",&lib)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&lib,&saved,sizeof lib)==0);return 0;
        }
        if(strcmp(name,"library_html_invalid")==0) {
            size_t required=13U;char buf[16]="original";
            CHECK(UmiEducationLibraryHtml(NULL,1U,&required)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiEducationLibraryHtml(buf,sizeof buf,NULL)==UMI_STATUS_INVALID_ARGUMENT);CHECK(buf[0]=='\0');return 0;
        }
        size_t needed=0U;OK(UmiEducationLibraryHtml(NULL,0U,&needed));CHECK(needed>1000U&&needed<150000U);
        f->html=malloc(needed);CHECK(f->html!=NULL);
        if(strcmp(name,"library_html_capacity")==0) {
            for(size_t c=0U;c<needed;c+=997U) {
                memset(f->html,'x',needed);size_t n=0U;
                CHECK(UmiEducationLibraryHtml(f->html,c,&n)==UMI_STATUS_CAPACITY_EXCEEDED);
                CHECK(n==needed);CHECK(f->html[c==0U?0U:c]== 'x');if(c>0U)CHECK(f->html[0]=='\0');
            }
            return 0;
        }
        OK(UmiEducationLibraryHtml(f->html,needed,&needed));CHECK(strlen(f->html)+1U==needed);
        CHECK(strstr(f->html,"data:image/png;base64,")!=NULL);
        CHECK(strstr(f->html,"<script")==NULL);CHECK(strstr(f->html,"Workshop learner")==NULL);
        for(size_t i=0U;i<UmiEducationLibraryCount();++i)CHECK(strstr(f->html,UmiEducationLibraryAt(i)->guideFile)!=NULL);
        return 0;
    }
    if(strcmp(name,"reference_progress")==0) {
        /* Every valid completed-prefix combination for the three independent
         * four-lesson courses: 5*5*5=125 independently checked captures. */
        for(size_t a=0U;a<=4U;++a)for(size_t b=0U;b<=4U;++b)for(size_t c=0U;c<=4U;++c) {
            UmiEducationClose(f->workspace);f->workspace=NULL;umi_data_server_destroy(f->server);f->server=NULL;
            OK(umi_data_server_create_memory(&f->server));OK(UmiEducationOpen(f->server,"learner","Workshop learner",&f->workspace));
            size_t done[3]={a,b,c};
            for(size_t course=0U;course<3U;++course)for(size_t i=0U;i<done[course];++i)OK(Pass(f,course*4U+i));
            OK(Capture(f));query.filter=UMI_EDUCATION_STUDY_PASSED;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==a+b+c);
            for(size_t course=0U;course<3U;++course) {
                OK(UmiEducationStudyPlan(f->a,Id(course*4U+3U),&route));CHECK(route.count==4U-done[course]);
                uint32_t minutes=0U;for(size_t i=done[course];i<4U;++i)minutes+=UmiEducationLessonAt(course*4U+i)->minutes;
                CHECK(route.estimatedMinutes==minutes);
                for(size_t i=0U;i<route.count;++i)CHECK(route.indices[i]==course*4U+done[course]+i);
            }
            UmiEducationStudyDestroy(f->a);f->a=NULL;
        }
        return 0;
    }
    if(strcmp(name,"many_captures")==0) {
        for(size_t n=0U;n<2000U;++n){OK(Capture(f));OK(UmiEducationStudyNext(f->a,"",&item));UmiEducationStudyDestroy(f->a);f->a=NULL;}
        CHECK(Revision(f)==0U);return 0;
    }
    if(strcmp(name,"filters_progress")==0||strcmp(name,"route_partial")==0)OK(Pass(f,0U));
    if(strcmp(name,"route_complete")==0||strcmp(name,"next_complete")==0)for(size_t i=0U;i<4U;++i)OK(Pass(f,i));
    if(strcmp(name,"search_private_note")==0)OK(UmiEducationSaveNote(f->workspace,Id(0),"private-needle-327xyz"));
    if(strcmp(name,"recovery_blocked")==0)f->workspace->recoveryRequired=true;
    OK(Capture(f));
    if(strcmp(name,"capture_empty")==0||strcmp(name,"capture_no_write")==0) {
        OK(UmiEducationStudySnapshotRead(f->a,&snapshot));CHECK(snapshot.revision==0U&&snapshot.passedQuizzes==0U);
        CHECK(umi_data_server_count(f->server)==0U);return 0;
    }
    if(strcmp(name,"capture_immutable")==0) {
        OK(Pass(f,0U));OK(UmiEducationStudyCapture(f->workspace,&f->b));
        OK(UmiEducationStudyItemRead(f->a,0U,&item));CHECK(!item.quizPassed);
        OK(UmiEducationStudyItemRead(f->b,0U,&item));CHECK(item.quizPassed);return 0;
    }
    if(strcmp(name,"capture_after_close")==0) {
        UmiEducationClose(f->workspace);f->workspace=NULL;umi_data_server_destroy(f->server);f->server=NULL;
        OK(UmiEducationStudyNext(f->a,"notes",&item));CHECK(item.catalogueIndex==0U);return 0;
    }
    if(strcmp(name,"item_bounds")==0||strcmp(name,"snapshot_bounds")==0) {
        memset(&item,0x5A,sizeof item);UmiEducationStudyItem saved=item;
        CHECK(UmiEducationStudyItemRead(f->a,SIZE_MAX,&item)==UMI_STATUS_NOT_FOUND);CHECK(memcmp(&item,&saved,sizeof item)==0);
        CHECK(UmiEducationStudyItemRead(NULL,0U,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationStudyItemRead(f->a,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationStudySnapshotRead(NULL,&snapshot)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationStudySnapshotRead(f->a,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strncmp(name,"search_",7U)==0) {
        if(strcmp(name,"search_ascii")==0)query.text="nOtEs";
        if(strcmp(name,"search_course")==0)query.courseId="assembly";
        if(strcmp(name,"search_invalid_course")==0)query.courseId="unrecognised";
        if(strcmp(name,"search_invalid_utf8")==0)query.text="\xc0\xaf";
        if(strcmp(name,"search_private_note")==0)query.text="private-needle-327xyz";
        char longText[130];memset(longText,'a',sizeof longText);longText[129]='\0';
        if(strcmp(name,"search_capacity")==0)query.text=longText;
        if(strstr(name,"invalid")!=NULL||strcmp(name,"search_capacity")==0) {
            memset(&matches,0x5A,sizeof matches);UmiEducationStudyMatches saved=matches;
            CHECK(UmiEducationStudySearch(f->a,&query,&matches)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&matches,&saved,sizeof matches)==0);return 0;
        }
        OK(UmiEducationStudySearch(f->a,&query,&matches));
        if(strcmp(name,"search_all")==0)CHECK(matches.count==12U);
        if(strcmp(name,"search_ascii")==0)CHECK(matches.count>=4U);
        if(strcmp(name,"search_course")==0){CHECK(matches.count==4U);CHECK(matches.indices[0]==4U);}
        if(strcmp(name,"search_private_note")==0)CHECK(matches.count==0U);
        return 0;
    }
    if(strncmp(name,"filters_",8U)==0) {
        size_t passed=strcmp(name,"filters_progress")==0?1U:0U;
        query.filter=UMI_EDUCATION_STUDY_READY;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==3U);
        query.filter=UMI_EDUCATION_STUDY_BLOCKED;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==9U-passed);
        query.filter=UMI_EDUCATION_STUDY_PASSED;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==passed);
        query.filter=UMI_EDUCATION_STUDY_UNREAD;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==12U-passed);
        query.filter=(UmiEducationStudyFilter)99;CHECK(UmiEducationStudySearch(f->a,&query,&matches)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strcmp(name,"recovery_blocked")==0) {
        CHECK(UmiEducationStudyPlan(f->a,Id(3),&route)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiEducationStudyNext(f->a,"",&item)==UMI_STATUS_INVALID_STATE);
        query.filter=UMI_EDUCATION_STUDY_READY;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==0U);
        query.filter=UMI_EDUCATION_STUDY_BLOCKED;OK(UmiEducationStudySearch(f->a,&query,&matches));CHECK(matches.count==12U);return 0;
    }
    if(strncmp(name,"route_",6U)==0||strcmp(name,"courses_independent")==0) {
        uint64_t before=Revision(f);
        if(strcmp(name,"route_invalid")==0) {
            memset(&route,0x5A,sizeof route);UmiEducationStudyRoute saved=route;
            CHECK(UmiEducationStudyPlan(f->a,"absent",&route)==UMI_STATUS_NOT_FOUND);
            CHECK(UmiEducationStudyPlan(f->a,NULL,&route)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&route,&saved,sizeof route)==0);return 0;
        }
        size_t goal=strcmp(name,"route_first")==0?0U:strcmp(name,"route_course")==0||strcmp(name,"courses_independent")==0?7U:3U;
        OK(UmiEducationStudyPlan(f->a,Id(goal),&route));CHECK(route.goalIndex==goal);
        size_t expected=strcmp(name,"route_first")==0?1U:strcmp(name,"route_partial")==0?3U:strcmp(name,"route_complete")==0?0U:4U;
        CHECK(route.count==expected);CHECK(Revision(f)==before);
        if(goal==7U)CHECK(route.indices[0]==4U);
        if(strcmp(name,"route_partial")==0)CHECK(route.indices[0]==1U);
        return 0;
    }
    if(strncmp(name,"next_",5U)==0) {
        if(strcmp(name,"next_invalid")==0) {
            CHECK(UmiEducationStudyNext(f->a,"unknown",&item)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiEducationStudyNext(f->a,NULL,&item)==UMI_STATUS_INVALID_ARGUMENT);return 0;
        }
        if(strcmp(name,"next_complete")==0){CHECK(UmiEducationStudyNext(f->a,"notes",&item)==UMI_STATUS_NOT_FOUND);OK(UmiEducationStudyNext(f->a,"",&item));CHECK(item.catalogueIndex==4U);}
        else {OK(UmiEducationStudyNext(f->a,"",&item));CHECK(item.catalogueIndex==0U);}
        return 0;
    }
    fprintf(stderr,"Unknown case %s\n",name);return 1;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    Fixture f={0};UmiStatus s=umi_data_server_create_memory(&f.server);
    if(s==UMI_STATUS_OK)s=UmiEducationOpen(f.server,"learner","Workshop learner",&f.workspace);
    int result=s==UMI_STATUS_OK?Run(argv[1],&f):1;
    free(f.html);UmiEducationStudyDestroy(f.a);UmiEducationStudyDestroy(f.b);
    UmiEducationClose(f.workspace);umi_data_server_destroy(f.server);return result;
}
