/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_review/test_review.c
 *
 * PURPOSE:
 *   Exercise actual history snapshots, diagnostic adapters and read-only report handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review.h"
#include "umicom/build/parser.h"
#include "umicom/compiler/diagnostic.h"
#include "umicom/diagnostics/compiler_parser.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static UmiBuildResult *Result(void)
{
    UmiBuildResult *r = calloc(1U, sizeof *r);
    if (r == NULL) abort();
    r->operation_id = 7U; r->phase = UMI_BUILD_PHASE_BUILD;
    r->state = UMI_BUILD_STATE_SUCCEEDED; r->status = UMI_STATUS_OK;
    strcpy(r->profile_id, "notes.debug");
    strcpy(r->command, "cmake --build build");
    strcpy(r->output, "src/notes.c:8:5: warning: unused variable [-Wunused-variable]\n");
    if (umi_build_parse_output(r->output, &r->diagnostics) != UMI_STATUS_OK) abort();
    return r;
}
static int Grammar(const char *name)
{
    static const struct {const char *name, *line, *path; uint32_t row, column; UmiCompilerDiagnosticSeverity severity;} cases[] = {
        {"gcc", "notes.c:12:4: error: expected expression", "notes.c",12,4,UMI_COMPILER_DIAGNOSTIC_ERROR},
        {"drive", "C:/Umicom Notes/src/main.c:8:5: error: missing semicolon", "C:/Umicom Notes/src/main.c",8,5,UMI_COMPILER_DIAGNOSTIC_ERROR},
        {"backslash", "D:\\Notes\\main.c:9:2: warning: unused [-Wunused]", "D:\\Notes\\main.c",9,2,UMI_COMPILER_DIAGNOSTIC_WARNING},
        {"msvc", "C:\\Notes (practice)\\main.c(10,3): error C2143: missing ';'", "C:\\Notes (practice)\\main.c",10,3,UMI_COMPILER_DIAGNOSTIC_ERROR},
        {"line_only", "notes.c:12: note: previous declaration", "notes.c",12,0,UMI_COMPILER_DIAGNOSTIC_NOTE},
        {"ansi", "\x1b[31mnotes.c:12:4: error: invalid\x1b[0m", "notes.c",12,4,UMI_COMPILER_DIAGNOSTIC_ERROR},
        {"fatal", "notes.c:1:2: fatal error: missing file", "notes.c",1,2,UMI_COMPILER_DIAGNOSTIC_FATAL},
        {"cmake_error", "CMake Error at C:/Notes/CMakeLists.txt:3 (target_link_libraries):", "C:/Notes/CMakeLists.txt",3,0,UMI_COMPILER_DIAGNOSTIC_ERROR},
        {"cmake_warning", "CMake Warning at CMakeLists.txt:13 (target_link_libraries):", "CMakeLists.txt",13,0,UMI_COMPILER_DIAGNOSTIC_WARNING},
        {"cmake_dev", "CMake Warning (dev) at CMakeLists.txt:13 (target_link_libraries):", "CMakeLists.txt",13,0,UMI_COMPILER_DIAGNOSTIC_WARNING},
        {"cmake_author", "CMake Warning (author) at C:/umicom/Umicom-Applications/framework/tests/data_safety/CMakeLists.txt:13 (target_link_libraries):", "C:/umicom/Umicom-Applications/framework/tests/data_safety/CMakeLists.txt",13,0,UMI_COMPILER_DIAGNOSTIC_WARNING},
        {"cmake_deprecation", "CMake Deprecation Warning at CMakeLists.txt:2 (cmake_minimum_required):", "CMakeLists.txt",2,0,UMI_COMPILER_DIAGNOSTIC_WARNING},
        {"tool", "clang: error: linker failed", "",0,0,UMI_COMPILER_DIAGNOSTIC_ERROR},
        {"crlf", "notes.c:5:3: error: invalid\r\n", "notes.c",5,3,UMI_COMPILER_DIAGNOSTIC_ERROR}
    };
    for (size_t i=0U;i<sizeof cases/sizeof cases[0];++i) if(strcmp(name,cases[i].name)==0) {
        UmiCompilerDiagnostic d;
        UmiBuildDiagnostic b;
        CHECK(umi_compiler_diagnostic_parse_line(cases[i].line,&d)==UMI_STATUS_OK);
        CHECK(umi_build_parse_diagnostic_line(cases[i].line,&b)==UMI_STATUS_OK);
        CHECK(strcmp(d.file,cases[i].path)==0 && strcmp(d.file,b.file)==0);
        CHECK(d.line==cases[i].row && d.column==cases[i].column && d.line==b.line && d.column==b.column);
        CHECK(d.severity==cases[i].severity && strcmp(d.message,b.message)==0);
        return 0;
    }
    UmiCompilerDiagnostic d;
    memset(&d,0x55,sizeof d);
    if(strcmp(name,"ordinary")==0) {
        CHECK(umi_compiler_diagnostic_parse_line("100% tests passed",&d)==UMI_STATUS_PARSE_ERROR);
        CHECK(d.file[0]==0 && d.message[0]==0); return 0;
    }
    if(strcmp(name,"position_overflow")==0) {
        CHECK(umi_compiler_diagnostic_parse_line("notes.c:4294967296:2: error: invalid",&d)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(d.line==0 && d.file[0]==0);return 0;
    }
    if(strcmp(name,"message_capacity")==0 || strcmp(name,"code_capacity")==0) {
        char line[1600];
        if(strcmp(name,"message_capacity")==0) {
            strcpy(line,"notes.c:1:2: error: "); size_t n=strlen(line);memset(line+n,'x',600U);line[n+600U]=0;
        } else {strcpy(line,"notes.c(1,2): error ");size_t n=strlen(line);memset(line+n,'C',70U);strcpy(line+n+70U,": invalid");}
        CHECK(umi_compiler_diagnostic_parse_line(line,&d)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;
    }
    if(strcmp(name,"null")==0){CHECK(umi_compiler_diagnostic_parse_line(NULL,&d)==UMI_STATUS_INVALID_ARGUMENT);CHECK(umi_compiler_diagnostic_parse_line("x",NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(strcmp(name,"escape")==0){CHECK(umi_compiler_diagnostic_parse_line("\x1b]window title",&d)==UMI_STATUS_PARSE_ERROR);return 0;}
    return 1;
}
static int Collection(const char *name)
{
    UmiCompilerDiagnosticSet *s=calloc(1U,sizeof *s);
    UmiCompilerDiagnostic d={0};
    CHECK(s!=NULL);strcpy(d.message,"practice diagnostic");d.severity=UMI_COMPILER_DIAGNOSTIC_WARNING;
    if(strcmp(name,"normal")==0){CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_OK);CHECK(s->count==1U && s->warnings==1U && s->revision==1U);CHECK(umi_compiler_diagnostic_set_at(s,0U)!=NULL);}
    else if(strcmp(name,"overflow")==0){s->count=UMI_COMPILER_MAX_DIAGNOSTICS+1U;CHECK(umi_compiler_diagnostic_set_at(s,UMI_COMPILER_MAX_DIAGNOSTICS)==NULL);CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_INVALID_STATE);}
    else if(strcmp(name,"full")==0){s->count=UMI_COMPILER_MAX_DIAGNOSTICS;CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_CAPACITY_EXCEEDED);}
    else if(strcmp(name,"revision")==0){s->revision=UINT64_MAX;CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(s->count==0U);}
    else if(strcmp(name,"unterminated")==0){memset(d.message,'x',sizeof d.message);CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_INVALID_ARGUMENT);}
    else if(strcmp(name,"severity")==0){d.severity=(UmiCompilerDiagnosticSeverity)100;CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_INVALID_ARGUMENT);}
    else if(strcmp(name,"counts")==0){s->errors=SIZE_MAX;CHECK(umi_compiler_diagnostic_set_add(s,&d)==UMI_STATUS_INVALID_STATE);}
    else {free(s);return 1;}
    free(s);return 0;
}
static int Invalid(const char *name)
{
    UmiBuildResult *r=Result();UmiBuildReview *review=(UmiBuildReview *)(uintptr_t)1U;
    if(strcmp(name,"count")==0)r->diagnostics.count=UMI_BUILD_MAX_DIAGNOSTICS+1U;
    else if(strcmp(name,"profile")==0)memset(r->profile_id,'x',sizeof r->profile_id);
    else if(strcmp(name,"command")==0)memset(r->command,'x',sizeof r->command);
    else if(strcmp(name,"output")==0)memset(r->output,'x',sizeof r->output);
    else if(strcmp(name,"path")==0)memset(r->diagnostics.items[0].file,'x',sizeof r->diagnostics.items[0].file);
    else if(strcmp(name,"message")==0)memset(r->diagnostics.items[0].message,'x',sizeof r->diagnostics.items[0].message);
    else if(strcmp(name,"code")==0)memset(r->diagnostics.items[0].code,'x',sizeof r->diagnostics.items[0].code);
    else if(strcmp(name,"severity")==0)r->diagnostics.items[0].severity=(UmiBuildDiagnosticSeverity)-1;
    else if(strcmp(name,"position")==0){r->diagnostics.items[0].line=0U;r->diagnostics.items[0].column=1U;}
    else if(strcmp(name,"empty_message")==0)r->diagnostics.items[0].message[0]=0;
    else if(strcmp(name,"phase")==0)r->phase=(UmiBuildPhase)100;
    else if(strcmp(name,"state")==0)r->state=(UmiBuildState)100;
    else if(strcmp(name,"status")==0)r->status=(UmiStatus)100;
    else {free(r);return 1;}
    CHECK(UmiBuildReviewFromResult(r,&review)==UMI_STATUS_INVALID_STATE && review==NULL);
    free(r);return 0;
}
static int OutcomeCase(const char *name)
{
    UmiBuildResult *r=Result();UmiBuildReview *review=NULL;UmiBuildReviewSummary summary;
    UmiBuildReviewOutcome expected=UMI_BUILD_REVIEW_SUCCEEDED;
    if(strcmp(name,"created")==0){r->state=UMI_BUILD_STATE_CREATED;expected=UMI_BUILD_REVIEW_NOT_FINISHED;}
    else if(strcmp(name,"running")==0){r->state=UMI_BUILD_STATE_RUNNING;expected=UMI_BUILD_REVIEW_NOT_FINISHED;}
    else if(strcmp(name,"failed")==0){r->state=UMI_BUILD_STATE_FAILED;r->exit_code=1;r->status=UMI_STATUS_IO_ERROR;expected=UMI_BUILD_REVIEW_FAILED;r->diagnostics.count=0U;}
    else if(strcmp(name,"cancelled")==0){r->state=UMI_BUILD_STATE_CANCELLED;r->status=UMI_STATUS_CANCELLED;expected=UMI_BUILD_REVIEW_CANCELLED;}
    else if(strcmp(name,"timeout")==0){r->state=UMI_BUILD_STATE_TIMED_OUT;r->status=UMI_STATUS_TIMEOUT;expected=UMI_BUILD_REVIEW_TIMED_OUT;}
    else if(strcmp(name,"inconsistent_success")==0){r->exit_code=1;expected=UMI_BUILD_REVIEW_INCONSISTENT;}
    else if(strcmp(name,"inconsistent_failure")==0){r->state=UMI_BUILD_STATE_FAILED;expected=UMI_BUILD_REVIEW_INCONSISTENT;}
    else if(strcmp(name,"inconsistent_cancel")==0){r->state=UMI_BUILD_STATE_CANCELLED;expected=UMI_BUILD_REVIEW_INCONSISTENT;}
    else if(strcmp(name,"inconsistent_timeout")==0){r->state=UMI_BUILD_STATE_TIMED_OUT;expected=UMI_BUILD_REVIEW_INCONSISTENT;}
    else if(strcmp(name,"success")!=0){free(r);return 1;}
    CHECK(UmiBuildReviewFromResult(r,&review)==UMI_STATUS_OK);free(r);
    CHECK(UmiBuildReviewSummarise(review,0U,NULL,&summary)==UMI_STATUS_OK);
    CHECK(summary.outcome==expected && !summary.sourceRevisionKnown && !summary.workingDirectoryKnown && !summary.outputCompletenessKnown);
    UmiBuildReviewDestroy(review);return 0;
}
typedef struct Producer {UmiBuildHistory *history;atomic_int done;int status;} Producer;
static int Produce(void *opaque)
{
    Producer *p=opaque;UmiBuildResult *r=Result();p->status=0;
    for(uint64_t i=1U;i<=1000U;++i){r->operation_id=i;(void)snprintf(r->output,sizeof r->output,"operation %" PRIu64,i);if(umi_build_history_append(p->history,r)!=UMI_STATUS_OK)p->status=1;}
    free(r);atomic_store(&p->done,1);return p->status;
}
static int History(const char *name)
{
    UmiBuildHistory *history=NULL;UmiBuildReview *review=NULL;UmiBuildResult *r=Result();UmiBuildReviewSummary summary;
    CHECK(umi_build_history_create(4U,&history)==UMI_STATUS_OK);
    if(strcmp(name,"empty")==0){CHECK(UmiBuildReviewCapture(history,2U,&review)==UMI_STATUS_OK);CHECK(UmiBuildReviewCount(review)==0U);CHECK(UmiBuildReviewSummarise(review,0U,NULL,&summary)==UMI_STATUS_NOT_FOUND);}
    else if(strcmp(name,"invalid_args")==0){CHECK(UmiBuildReviewCapture(history,0U,&review)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiBuildReviewCapture(history,17U,&review)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiBuildReviewCapture(NULL,1U,&review)==UMI_STATUS_INVALID_ARGUMENT);size_t n=5U;CHECK(UmiBuildHistoryCopyRecent(history,r,1U,&n,&n)==UMI_STATUS_INVALID_ARGUMENT);CHECK(n==0U);}
    else if(strcmp(name,"ring")==0 || strcmp(name,"detached")==0){
        for(uint64_t i=1U;i<=8U;++i){r->operation_id=i;CHECK(umi_build_history_append(history,r)==UMI_STATUS_OK);}
        CHECK(UmiBuildReviewCapture(history,2U,&review)==UMI_STATUS_OK);CHECK(UmiBuildReviewOmitted(review)==2U);
        umi_build_history_clear(history);umi_build_history_destroy(history);history=NULL;
        CHECK(UmiBuildReviewCount(review)==2U);CHECK(UmiBuildReviewSummarise(review,0U,NULL,&summary)==UMI_STATUS_OK && summary.operationId==7U);CHECK(UmiBuildReviewSummarise(review,1U,NULL,&summary)==UMI_STATUS_OK && summary.operationId==8U);
        if(strcmp(name,"detached")==0){char *text=NULL;size_t size=0U;CHECK(UmiBuildReviewRender(review,0U,NULL,&text,&size)==UMI_STATUS_OK);CHECK(strstr(text,"Operation: 7")!=NULL && size==strlen(text));UmiBuildReviewTextFree(text);}

    }else if(strcmp(name,"invalid_record")==0){r->diagnostics.count=SIZE_MAX;CHECK(umi_build_history_append(history,r)==UMI_STATUS_OK);CHECK(UmiBuildReviewCapture(history,1U,&review)==UMI_STATUS_INVALID_STATE && review==NULL);}
    else if(strcmp(name,"concurrent")==0){
        Producer p={.history=history,.done=0,.status=0};UmiThread *thread=NULL;int exitCode=1;
        CHECK(umi_thread_start(Produce,&p,&thread)==UMI_STATUS_OK);
        do {CHECK(UmiBuildReviewCapture(history,4U,&review)==UMI_STATUS_OK);uint64_t prev=0;
            for(size_t i=0;i<UmiBuildReviewCount(review);++i){CHECK(UmiBuildReviewSummarise(review,i,NULL,&summary)==UMI_STATUS_OK);CHECK(prev==0U || summary.operationId==prev+1U);prev=summary.operationId;}
            UmiBuildReviewDestroy(review);review=NULL;
        }while(!atomic_load(&p.done));
        CHECK(umi_thread_join(thread,&exitCode)==UMI_STATUS_OK && exitCode==0);umi_thread_destroy(thread);
    }else{free(r);umi_build_history_destroy(history);return 1;}
    free(r);UmiBuildReviewDestroy(review);umi_build_history_destroy(history);return 0;
}
static int View(const char *name)
{
    UmiBuildResult *r=Result();UmiBuildReview *review=NULL;UmiBuildReviewSummary summary={0};UmiBuildReviewFilter filter={UMI_BUILD_DIAGNOSTIC_NOTE,""};UmiBuildDiagnostic d;
    char *report=NULL;size_t length=0U;
    r->diagnostics.items[1]=r->diagnostics.items[0];r->diagnostics.items[1].severity=UMI_BUILD_DIAGNOSTIC_ERROR;strcpy(r->diagnostics.items[1].message,"missing semicolon");r->diagnostics.count=2U;
    if(strcmp(name,"safe_text")==0)strcpy(r->output,"read \x1b[31mred\r\ninvalid \xff\nUnicode: \xc2\xa3\nBidi: \xe2\x80\xae\n");
    if(strcmp(name,"maximum_output")==0){memset(r->output,'x',sizeof r->output-1U);r->output[sizeof r->output-1U]=0;}
    if(strcmp(name,"dropped")==0)r->diagnostics.dropped=SIZE_MAX;
    CHECK(UmiBuildReviewFromResult(r,&review)==UMI_STATUS_OK);free(r);
    if(strcmp(name,"filter")==0){filter.minimumSeverity=UMI_BUILD_DIAGNOSTIC_ERROR;CHECK(UmiBuildReviewSummarise(review,0U,&filter,&summary)==UMI_STATUS_OK);CHECK(summary.visibleDiagnostics==1U && summary.totalDiagnostics==2U && summary.errors==1U);CHECK(summary.outcome==UMI_BUILD_REVIEW_SUCCEEDED);}
    else if(strcmp(name,"search")==0){strcpy(filter.contains,"semicolon");CHECK(UmiBuildReviewDiagnostic(review,0U,&filter,0U,&d)==UMI_STATUS_OK);CHECK(strstr(d.message,"semicolon")!=NULL);CHECK(UmiBuildReviewDiagnostic(review,0U,&filter,1U,&d)==UMI_STATUS_NOT_FOUND);}
    else if(strcmp(name,"invalid_filter")==0){memset(filter.contains,'x',sizeof filter.contains);CHECK(UmiBuildReviewSummarise(review,0U,&filter,&summary)==UMI_STATUS_INVALID_ARGUMENT);}
    else if(strcmp(name,"invalid_minimum")==0){filter.minimumSeverity=(UmiBuildDiagnosticSeverity)99;CHECK(UmiBuildReviewSummarise(review,0U,&filter,&summary)==UMI_STATUS_INVALID_ARGUMENT);}
    else if(strcmp(name,"bad_index")==0){CHECK(UmiBuildReviewRender(review,100U,NULL,&report,&length)==UMI_STATUS_NOT_FOUND && report==NULL && length==0U);}
    else {
        CHECK(UmiBuildReviewRender(review,0U,&filter,&report,&length)==UMI_STATUS_OK);CHECK(length==strlen(report));
        CHECK(strstr(report,"Output completeness: unknown")!=NULL);
        if(strcmp(name,"safe_text")==0){CHECK(strchr(report,27)==NULL);CHECK(strstr(report,"\\x1B[31mred\\x0D")!=NULL);CHECK(strstr(report,"\\xFF")!=NULL);CHECK(strstr(report,"\xc2\xa3")!=NULL);CHECK(strstr(report,"\\xE2\\x80\\xAE")!=NULL);}
        else if(strcmp(name,"maximum_output")==0)CHECK(length>65535U);
        else if(strcmp(name,"dropped")==0){CHECK(UmiBuildReviewSummarise(review,0U,NULL,&summary)==UMI_STATUS_OK);CHECK(summary.droppedDiagnostics==SIZE_MAX);}
        else if(strcmp(name,"render")!=0){UmiBuildReviewTextFree(report);UmiBuildReviewDestroy(review);return 1;}
    }
    UmiBuildReviewTextFree(report);UmiBuildReviewDestroy(review);return 0;
}
static int Import(const char *name)
{
    UmiBuildReview *review=NULL;UmiBuildReviewSummary summary;
    const char *text="notes.c:3:1: error: invalid\nAll tests passed\n";size_t length=strlen(text);char *owned=NULL;
    if(strcmp(name,"embedded_nul")==0){CHECK(UmiBuildReviewImportLog("a\0b",3U,&review)==UMI_STATUS_PARSE_ERROR);return 0;}
    if(strcmp(name,"too_large")==0){CHECK(UmiBuildReviewImportLog("x",65536U,&review)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
    if(strcmp(name,"empty")==0){text="";length=0U;}
    else if(strcmp(name,"long_line")==0){owned=malloc(9001U);CHECK(owned!=NULL);memset(owned,'x',9000U);owned[9000]=0;text=owned;length=9000U;}
    else if(strcmp(name,"capacity")==0){owned=calloc(1U,20000U);CHECK(owned!=NULL);for(size_t i=0U;i<300U;++i)strcat(owned,"x.c:1:1: error: invalid\n");text=owned;length=strlen(text);}
    else if(strcmp(name,"normal")!=0)return 1;
    CHECK(UmiBuildReviewImportLog(text,length,&review)==UMI_STATUS_OK);free(owned);
    CHECK(UmiBuildReviewSummarise(review,0U,NULL,&summary)==UMI_STATUS_OK);CHECK(summary.outcome==UMI_BUILD_REVIEW_UNRECORDED);
    if(strcmp(name,"normal")==0)CHECK(summary.errors==1U);
    if(strcmp(name,"empty")==0)CHECK(summary.outputBytes==0U);
    if(strcmp(name,"long_line")==0)CHECK(summary.droppedDiagnostics==1U && summary.totalDiagnostics==0U);
    if(strcmp(name,"capacity")==0)CHECK(summary.totalDiagnostics==256U && summary.droppedDiagnostics==44U);
    UmiBuildReviewDestroy(review);return 0;
}
static int Mutations(void)
{
    char text[256];uint32_t random=31U;
    for(size_t round=0U;round<10000U;++round){size_t length=round%255U;for(size_t i=0U;i<length;++i){random=random*1664525U+1013904223U;text[i]=(char)(1U+random%255U);}text[length]=0;UmiBuildReview *review=NULL;char *report=NULL;size_t n=0U;CHECK(UmiBuildReviewImportLog(text,length,&review)==UMI_STATUS_OK);CHECK(UmiBuildReviewRender(review,0U,NULL,&report,&n)==UMI_STATUS_OK);CHECK(n==strlen(report));UmiBuildReviewTextFree(report);UmiBuildReviewDestroy(review);}
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(strcmp(argv[1],"grammar")==0)return Grammar(argv[2]);
    if(strcmp(argv[1],"collection")==0)return Collection(argv[2]);
    if(strcmp(argv[1],"invalid")==0)return Invalid(argv[2]);
    if(strcmp(argv[1],"outcome")==0)return OutcomeCase(argv[2]);
    if(strcmp(argv[1],"history")==0)return History(argv[2]);
    if(strcmp(argv[1],"view")==0)return View(argv[2]);
    if(strcmp(argv[1],"import")==0)return Import(argv[2]);
    if(strcmp(argv[1],"mutation")==0)return Mutations();
    return 2;
}
