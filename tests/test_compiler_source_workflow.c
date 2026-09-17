/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_source_workflow.c
 * PURPOSE: Check compiler text, source navigation and bounded problem projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/parser.h"
#include "umicom/diagnostics/compiler_parser.h"
#include "umicom/diagnostic_ui/build_projection.h"
#include "umicom/diagnostic_ui/navigation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } } while (0)

typedef struct Case { const char *name, *text, *path; size_t line, column; UmiBuildDiagnosticSeverity severity; const char *code; } Case;
static const Case cases[] = {
    {"gcc", "src/notes.c:12:7: error: missing semicolon", "src/notes.c", 12U, 7U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"windows-slashes", "C:/Umicom Notes/src/main.c:9:2: warning: unused variable [-Wunused-variable]", "C:/Umicom Notes/src/main.c", 9U, 2U, UMI_BUILD_DIAGNOSTIC_WARNING, "-Wunused-variable"},
    {"windows-backslashes", "C:\\Umicom Notes\\src\\main.c:9:2: error: expected expression", "C:\\Umicom Notes\\src\\main.c", 9U, 2U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"unc", "\\\\server\\notes\\main.c:3:1: error: expected expression", "\\\\server\\notes\\main.c", 3U, 1U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"gnu-line-only", "C:/work/main.c:42: warning: unused function", "C:/work/main.c", 42U, 0U, UMI_BUILD_DIAGNOSTIC_WARNING, ""},
    {"gnu-parentheses", "/work/Notes (training)/main.c:3:8: error: unknown name", "/work/Notes (training)/main.c", 3U, 8U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"msvc", "C:\\Notes\\main.c(15,6): error C2143: syntax error", "C:\\Notes\\main.c", 15U, 6U, UMI_BUILD_DIAGNOSTIC_ERROR, "C2143"},
    {"msvc-line-only", "C:/Notes/main.c(15): warning C4996: obsolete call", "C:/Notes/main.c", 15U, 0U, UMI_BUILD_DIAGNOSTIC_WARNING, "C4996"},
    {"msvc-parentheses", "C:/Notes (example)/main.c(5,2): error C1000: invalid call", "C:/Notes (example)/main.c", 5U, 2U, UMI_BUILD_DIAGNOSTIC_ERROR, "C1000"},
    {"ansi", "\033[01mC:/Notes/main.c:4:2:\033[0m \033[01;31merror:\033[0m invalid expression\r\n", "C:/Notes/main.c", 4U, 2U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"note", "notes.c:4:2: note: the text mentions error: here", "notes.c", 4U, 2U, UMI_BUILD_DIAGNOSTIC_NOTE, ""},
    {"fatal", "notes.c:4:2: fatal error: header not found", "notes.c", 4U, 2U, UMI_BUILD_DIAGNOSTIC_FATAL, ""},
    {"tool", "clang: error: unsupported argument", "", 0U, 0U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"ninja", "ninja: error: loading build.ninja", "", 0U, 0U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"cmake", "CMake Error at C:/Notes/CMakeLists.txt:8 (message):", "C:/Notes/CMakeLists.txt", 8U, 0U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
    {"cmake-dev", "CMake Warning (dev) at /notes/CMakeLists.txt:4 (project):", "/notes/CMakeLists.txt", 4U, 0U, UMI_BUILD_DIAGNOSTIC_WARNING, ""},
    {"leading-space", "  src/main.c:8:1: error: missing name\r", "src/main.c", 8U, 1U, UMI_BUILD_DIAGNOSTIC_ERROR, ""},
};

static int ParseCase(const Case *test)
{
    UmiBuildDiagnostic diagnostic;
    UmiCompilerDiagnosticFields fields;
    CHECK(umi_build_parse_diagnostic_line(test->text, &diagnostic) == UMI_STATUS_OK);
    CHECK(strcmp(diagnostic.file, test->path) == 0);
    CHECK(diagnostic.line == test->line && diagnostic.column == test->column);
    CHECK(diagnostic.severity == test->severity);
    CHECK(strcmp(diagnostic.code, test->code) == 0);
    CHECK(UmiCompilerDiagnosticParseText(test->text, &fields) == UMI_STATUS_OK);
    CHECK(strcmp(fields.path, diagnostic.file) == 0);
    CHECK(strcmp(fields.message, diagnostic.message) == 0);
    return EXIT_SUCCESS;
}

static int Noise(void)
{
    const char *noise[] = {"0 errors and 0 warnings", "[23/40] Building C object warning_notes.c.o",
        "    printf(\"error: example\");", "compilation terminated.", "", "a.c:1:2: informational: note"};
    for (size_t i=0U; i<sizeof noise/sizeof noise[0]; ++i) {
        UmiBuildDiagnostic d;
        CHECK(umi_build_parse_diagnostic_line(noise[i], &d) == UMI_STATUS_NOT_FOUND);
    }
    return 0;
}
static int Malformed(void)
{
    const char *bad[] = {"main.c:0:2: error: zero", "main.c:-1:2: error: signed", "main.c:999999999999999999999999999999:2: error: overflow",
        "main.c:1:+2: error: signed column", "main.c(0): error C1000: zero"};
    for (size_t i=0U; i<sizeof bad/sizeof bad[0]; ++i) {
        UmiBuildDiagnostic d;
        UmiStatus status = umi_build_parse_diagnostic_line(bad[i], &d);
        CHECK(status == UMI_STATUS_NOT_FOUND || (status == UMI_STATUS_OK && d.file[0] == '\0' && d.line == 0U));
    }
    return 0;
}
static int Capacity(void)
{
    UmiBuildDiagnostic d;
    UmiBuildDiagnosticList *list = calloc(1U,sizeof *list);
    char *text=malloc(20000U);
    CHECK(list != NULL && text != NULL);
    strcpy(text, "a.c:1:1: error: "); size_t n=strlen(text);
    memset(text+n,'x',1500U); text[n+1500U]='\0';
    CHECK(umi_build_parse_diagnostic_line(text,&d)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_build_parse_output(text,list)==UMI_STATUS_OK && list->count==0U && list->dropped==1U);
    memset(text,'x',9000U); strcpy(text+9000U,"\na.c:3:4: error: retained\n");
    CHECK(umi_build_parse_output(text,list)==UMI_STATUS_OK && list->count==1U && list->dropped==1U);
    CHECK(list->items[0].line==3U);
    /* Keep the build API's full path capacity. */
    memset(text,'p',1500U); strcpy(text+1500U,"/notes.c:3:4: error: long path");
    CHECK(umi_build_parse_diagnostic_line(text,&d)==UMI_STATUS_OK && strlen(d.file)==1508U);
    free(text); free(list); return 0;
}
static int ListBound(void)
{
    UmiBuildDiagnosticList *list=calloc(1U,sizeof *list);
    const char *line="a.c:1:2: error: broken\n";
    size_t length=strlen(line), count=UMI_BUILD_MAX_DIAGNOSTICS+3U;
    char *text=calloc(count*length+1U,1U);
    CHECK(text!=NULL && list!=NULL);
    for(size_t i=0U;i<count;++i) memcpy(text+i*length,line,length);
    CHECK(umi_build_parse_output(text,list)==UMI_STATUS_OK);
    CHECK(list->count==UMI_BUILD_MAX_DIAGNOSTICS && list->dropped==3U);
    free(text); free(list); return 0;
}
static int NullInput(void)
{
    UmiBuildDiagnostic d;
    UmiCompilerDiagnosticFields f;
    CHECK(umi_build_parse_diagnostic_line(NULL,&d)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_build_parse_diagnostic_line("error: bad",NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCompilerDiagnosticParseText(NULL,&f)==UMI_STATUS_INVALID_ARGUMENT);
    memset(&f,0x41,sizeof f); UmiCompilerDiagnosticFields before=f;
    CHECK(UmiCompilerDiagnosticParseText("normal output",&f)==UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&f,&before,sizeof f)==0);
    return 0;
}
static int Navigation(void)
{
    UmiDiagnosticModel *model=NULL;
    UmiDiagnosticModelConfig config={512U};
    UmiDiagnosticNavigation navigation={0U,1};
    UmiDiagnosticFilter filter;
    UmiDiagnosticSnapshot d,out;
    CHECK(umi_diagnostic_model_create(&config,&model)==UMI_STATUS_OK);
    umi_diagnostic_filter_init(&filter);
    for(size_t i=0U;i<300U;++i) {
        char id[64]; snprintf(id,sizeof id,"noise-%zu",i);
        CHECK(umi_diagnostic_snapshot_init(&d,id,UMI_DIAGNOSTIC_ERROR,UMI_DIAGNOSTIC_KIND_GENERAL,"test","no source")==UMI_STATUS_OK);
        CHECK(umi_diagnostic_model_upsert(model,&d)==UMI_STATUS_OK);
    }
    for(size_t i=0U;i<3U;++i) {
        char id[64]; snprintf(id,sizeof id,"source-%zu",i);
        CHECK(umi_diagnostic_snapshot_init(&d,id,UMI_DIAGNOSTIC_ERROR,UMI_DIAGNOSTIC_KIND_COMPILER,"test","source issue")==UMI_STATUS_OK);
        strcpy(d.uri,"notes.c"); d.line=(uint32_t)i+1U;
        CHECK(umi_diagnostic_model_upsert(model,&d)==UMI_STATUS_OK);
    }
    CHECK(umi_diagnostic_model_resolve(model,"source-1",1)==UMI_STATUS_OK);
    CHECK(UmiDiagnosticNavigationSource(&navigation,model,&filter,0,&out)==UMI_STATUS_OK && out.line==1U);
    CHECK(UmiDiagnosticNavigationSource(&navigation,model,&filter,0,&out)==UMI_STATUS_OK && out.line==3U);
    navigation.wrap=0;
    CHECK(UmiDiagnosticNavigationSource(&navigation,model,&filter,0,&out)==UMI_STATUS_NOT_FOUND);
    navigation.wrap=1;
    CHECK(UmiDiagnosticNavigationSource(&navigation,model,&filter,0,&out)==UMI_STATUS_OK && out.line==1U);
    CHECK(UmiDiagnosticNavigationSource(&navigation,model,&filter,1,&out)==UMI_STATUS_OK && out.line==3U);
    umi_diagnostic_model_destroy(model); return 0;
}
static int Projection(void)
{
    UmiDiagnosticPipeline *pipeline=NULL;
    UmiBuildResult *result=NULL;
    UmiDiagnosticSnapshot diagnostic;
    UmiDiagnosticPipelineConfig config=umi_diagnostic_pipeline_config_default();
    CHECK(umi_diagnostic_pipeline_create(&config,&pipeline)==UMI_STATUS_OK);
    CHECK(umi_build_result_create(&result)==UMI_STATUS_OK);
    umi_build_result_init(result,10U,UMI_BUILD_PHASE_BUILD,"test");
    CHECK(umi_build_parse_output("C:/notes.c:4:2: error: wrong type\n",&result->diagnostics)==UMI_STATUS_OK);
    CHECK(umi_diagnostic_build_result_ingest(pipeline,result,"compiler")==UMI_STATUS_OK);
    UmiDiagnosticModel *model=umi_diagnostic_pipeline_model(pipeline);
    CHECK(umi_diagnostic_model_find(model,"build-10-0",&diagnostic)==UMI_STATUS_OK);
    CHECK(strcmp(diagnostic.uri,"C:/notes.c")==0 && diagnostic.line==4U && diagnostic.column==2U);
    CHECK(umi_diagnostic_snapshot_init(&diagnostic,"independent",UMI_DIAGNOSTIC_ERROR,UMI_DIAGNOSTIC_KIND_LANGUAGE,"language","not from build")==UMI_STATUS_OK);
    CHECK(umi_diagnostic_model_upsert(model,&diagnostic)==UMI_STATUS_OK);
    umi_build_result_init(result,11U,UMI_BUILD_PHASE_BUILD,"test");
    CHECK(umi_diagnostic_build_result_ingest(pipeline,result,"compiler")==UMI_STATUS_OK);
    CHECK(umi_diagnostic_model_find(model,"build-10-0",&diagnostic)==UMI_STATUS_OK && diagnostic.resolved);
    CHECK(umi_diagnostic_model_find(model,"independent",&diagnostic)==UMI_STATUS_OK && !diagnostic.resolved);
    result->diagnostics.count=UMI_BUILD_MAX_DIAGNOSTICS+1U;
    CHECK(umi_diagnostic_build_result_ingest(pipeline,result,"compiler")==UMI_STATUS_INVALID_ARGUMENT);
    umi_build_result_init(result,12U,UMI_BUILD_PHASE_BUILD,"test");
    result->diagnostics.count=1U; strcpy(result->diagnostics.items[0].message,"too long for a model URI");
    memset(result->diagnostics.items[0].file,'a',1500U); result->diagnostics.items[0].file[1500]='\0'; result->diagnostics.items[0].line=1U;
    CHECK(umi_diagnostic_build_result_ingest(pipeline,result,"compiler")==UMI_STATUS_OK);
    CHECK(umi_diagnostic_model_find(model,"build-12-0",&diagnostic)==UMI_STATUS_OK);
    CHECK(diagnostic.uri[0]=='\0' && diagnostic.detail[0]!='\0');
    umi_build_result_destroy(result); umi_diagnostic_pipeline_destroy(pipeline); return 0;
}
/* Exercise the same complete-text caret operation used by the native adapter. */
static int Positions(void)
{
    UmiCommandRegistry *commands=NULL;
    UmiUiWorkbench *workbench=NULL;
    UmiDocumentStore *store=NULL;
    UmiDocumentCoordinator *documents=NULL;
    UmiUiDocumentViewSnapshot view,before;
    UmiDocumentWorkingCopySnapshot state;
    char id[UMI_UI_ID_CAPACITY]; size_t offset=999U;
    CHECK(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.compiler-position",commands,&workbench)==UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store)==UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store,workbench,NULL,&documents)==UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents,"Notes",id,sizeof id)==UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),id,&view)==UMI_STATUS_OK);
    const char *text="\t\xce\xb1" "beta\n\nlast";
    view.dirty=1;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,text,strlen(text))==UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorGoToPosition(documents,1U,3U,&offset)==UMI_STATUS_OK && offset==1U);
    CHECK(UmiDocumentCoordinatorGoToPosition(documents,1U,999U,&offset)==UMI_STATUS_OK && offset==7U);
    CHECK(UmiDocumentCoordinatorGoToPosition(documents,2U,5U,&offset)==UMI_STATUS_OK && offset==8U);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),id,&before)==UMI_STATUS_OK);
    offset=999U;
    CHECK(UmiDocumentCoordinatorGoToPosition(documents,10U,1U,&offset)==UMI_STATUS_NOT_FOUND && offset==999U);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),id,&view)==UMI_STATUS_OK);
    CHECK(view.cursor_offset==before.cursor_offset);
    CHECK(UmiDocumentCoordinatorGoToPosition(documents,0U,1U,&offset)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_document_coordinator_active_snapshot(documents,&state)==UMI_STATUS_OK && state.dirty && state.undo_count==0U);
    UmiDiagnosticSnapshot problem;
    CHECK(umi_diagnostic_snapshot_init(&problem,"external",UMI_DIAGNOSTIC_ERROR,UMI_DIAGNOSTIC_KIND_COMPILER,"test","unsafe location")==UMI_STATUS_OK);
    strcpy(problem.uri,"https://example.invalid/source.c");problem.line=1U;
    CHECK(UmiDiagnosticOpenSource(documents,&problem,NULL,NULL)==UMI_STATUS_NOT_FOUND);
    strcpy(problem.uri,"file:///tmp/notes%00.c");
    CHECK(UmiDiagnosticOpenSource(documents,&problem,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_document_coordinator_count(documents)==1U);
    umi_document_coordinator_destroy(documents);umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);umi_command_registry_destroy(commands);
    return 0;
}

int main(int argc,char **argv)
{
    CHECK(argc==2);
    if(strcmp(argv[1],"positions")==0) return Positions();
    for(size_t i=0U;i<sizeof cases/sizeof cases[0];++i) if(strcmp(argv[1],cases[i].name)==0) return ParseCase(&cases[i]);
    if(strcmp(argv[1],"noise")==0) return Noise();
    if(strcmp(argv[1],"malformed")==0) return Malformed();
    if(strcmp(argv[1],"capacity")==0) return Capacity();
    if(strcmp(argv[1],"list-bound")==0) return ListBound();
    if(strcmp(argv[1],"null-input")==0) return NullInput();
    if(strcmp(argv[1],"navigation")==0) return Navigation();
    if(strcmp(argv[1],"projection")==0) return Projection();
    fprintf(stderr,"Unknown test: %s\n",argv[1]); return EXIT_FAILURE;
}
