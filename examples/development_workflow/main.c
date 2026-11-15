/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/development_workflow/main.c
 *
 * PURPOSE:
 *   Preview a native build plan, or generate a real Notes project, compile and test it, package it, verify its checksum, run the extracted executable, and install it to a private folder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/cmake_plan.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/developer_project/new_project.h"
#include "umicom/build/project_session.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#value); return EXIT_FAILURE; } } while (0)

/* This helper uses the production worker. Polling copies completed results;
 * it does not execute compiler work on a graphical owner's thread. */
static int Run(UmiBuildProjectSession *session, const UmiBuildProfile *profile,
    UmiBuildPhase phase, int expectSuccess, size_t *completed)
{
    CHECK(umi_build_project_session_submit(session, profile, phase, true) == UMI_STATUS_OK);
    UmiBuildResult *result = NULL; CHECK(umi_build_result_create(&result) == UMI_STATUS_OK);
    size_t shown = 0U;
    UmiBuildProjectSessionSnapshot snapshot = {0};
    for (unsigned attempt = 0U; attempt < 20000U; ++attempt) {
        CHECK(umi_build_project_session_snapshot(session, &snapshot) == UMI_STATUS_OK);
        while (shown < snapshot.completed_phase_count) {
            CHECK(umi_build_project_session_result_at(session, shown++, result) == UMI_STATUS_OK);
            printf("%s: %s (exit %d)\n%s\n", umi_build_phase_text(result->phase),
                umi_status_text(result->status), result->exit_code, result->output);
        }
        if (!snapshot.active) break;
        umi_thread_sleep_ms(10U);
    }
    umi_build_result_destroy(result);
    CHECK(!snapshot.active);
    CHECK((snapshot.status == UMI_STATUS_OK) == expectSuccess);
    *completed = snapshot.completed_phase_count;
    return EXIT_SUCCESS;
}

/* argv is passed to the platform runner directly, never through a shell. */
static int Execute(const char *program, const char *const *args, size_t count,
    const char *directory, const char *expectedPrefix)
{
    UmiProcessRequest request = {0};
    UmiProcessResult *result = calloc(1U, sizeof(*result)); CHECK(result != NULL);
    request.program = program; request.arguments = args; request.argument_count = count;
    request.working_directory = directory; request.capture_stdout = 1; request.capture_stderr = 1;
    request.timeout_ms = 30000U; request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiStatus status = umi_process_execute(&request, result);
    printf("%s: %s\n%s\n", program, umi_status_text(status), result->output);
    int passed = status == UMI_STATUS_OK && result->exit_code == 0 &&
        (expectedPrefix == NULL || strncmp(result->output, expectedPrefix, strlen(expectedPrefix)) == 0);
    free(result); CHECK(passed); return EXIT_SUCCESS;
}

typedef struct Found { char zip[UMI_PATH_CAPACITY]; char program[UMI_PATH_CAPACITY]; size_t zips; } Found;
static UmiStatus Find(const UmiFileInfo *info, void *context)
{
    Found *found = context;
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;
    size_t n = strlen(info->name);
    if (n > 4U && strcmp(info->name + n - 4U, ".zip") == 0) {
        ++found->zips; (void)snprintf(found->zip, sizeof(found->zip), "%s", info->path);
    }
    if (strcmp(info->name, "umicom_notes") == 0 || strcmp(info->name, "umicom_notes.exe") == 0)
        (void)snprintf(found->program, sizeof(found->program), "%s", info->path);
    return UMI_STATUS_OK;
}

/* This inspection path composes the public Framework planner. It deliberately
 * owns no project generator, filesystem service or process executor. */
static int Preview(int argc, char **argv)
{
    if ((argc != 3 && argc != 4) || argv[0][0] == '\0' || argv[1][0] == '\0') {
        fputs("Usage: umicom-development-workflow-example preview SOURCE BUILD CONFIGURATION [GENERATOR]\n", stderr);
        return 2;
    }
    UmiDeveloperPipeline *pipeline = NULL;
    UmiDeveloperOperationSnapshot *operation = NULL;
    char *json = NULL;
    UmiStatus status = umi_developer_pipeline_create(&pipeline);
    if (status != UMI_STATUS_OK) goto finished;
    UmiDeveloperCMakePlanRequest request = {0};
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    request.plan_id = "sdk.preview";
    request.configuration_id = argv[2];
    request.source_directory = argv[0];
    request.build_directory = argv[1];
    request.generator = argc == 4 ? argv[3] : "Ninja";
    request.include_configure = 1;
    request.include_build = 1;
    request.include_tests = 1;
    UmiDeveloperCMakePlanSnapshot plan;
    status = umi_developer_cmake_plan_submit(pipeline, &request, &plan);
    if (status != UMI_STATUS_OK) goto finished;
    operation = malloc(sizeof(*operation));
    /* Every string is owned bounded operation storage. Six bytes per input
     * byte covers JSON control escaping; punctuation and field names use the
     * remaining allowance. The shared writer still checks every append. */
    const size_t per_operation =
        (UMI_DEVELOPER_ID_CAPACITY + 2U * UMI_DEVELOPER_PATH_CAPACITY +
         UMI_DEVELOPER_MAX_ARGUMENTS * UMI_DEVELOPER_ARGUMENT_CAPACITY) * 6U + 512U;
    const size_t capacity = plan.operation_count * per_operation + 128U;
    json = malloc(capacity);
    if (operation == NULL || json == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto finished; }
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, json, capacity);
    (void)UmiJsonWriterRaw(&writer, "{\"mode\":\"preview\",\"executed\":false,\"operations\":[");
    const char *ids[] = {plan.configure_operation_id, plan.build_operation_id, plan.test_operation_id};
    for (size_t index = 0U; index < sizeof(ids) / sizeof(ids[0]); ++index) {
        status = umi_developer_pipeline_find(pipeline, ids[index], operation);
        if (status != UMI_STATUS_OK) goto finished;
        if (index != 0U) (void)UmiJsonWriterRaw(&writer, ",");
        (void)UmiJsonWriterRaw(&writer, "{\"id\":");
        (void)UmiJsonWriterString(&writer, operation->id);
        (void)UmiJsonWriterRaw(&writer, ",\"program\":");
        (void)UmiJsonWriterString(&writer, operation->program);
        (void)UmiJsonWriterRaw(&writer, ",\"workingDirectory\":");
        (void)UmiJsonWriterString(&writer, operation->working_directory);
        (void)UmiJsonWriterRaw(&writer, ",\"arguments\":[");
        for (size_t argument = 0U; argument < operation->argument_count; ++argument) {
            if (argument != 0U) (void)UmiJsonWriterRaw(&writer, ",");
            (void)UmiJsonWriterString(&writer, operation->arguments[argument]);
        }
        (void)UmiJsonWriterRaw(&writer, "]}");
    }
    (void)UmiJsonWriterRaw(&writer, "]}");
    status = writer.status;
/* Check the buffered preview delivery before reporting success; planning remains free of process execution. The former implementation is retained for owner review. */
#if 0
    if (status == UMI_STATUS_OK && puts(json) == EOF) status = UMI_STATUS_IO_ERROR;
#endif
    if (status == UMI_STATUS_OK) {
        int written = puts(json);
        int flushed = fflush(stdout);
        int stream_failed = ferror(stdout);
        if (written == EOF || flushed != 0 || stream_failed != 0)
            status = UMI_STATUS_IO_ERROR;
    }
finished:
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Cannot preview development plan: %s\n", umi_status_text(status));
    free(json);
    free(operation);
    umi_developer_pipeline_destroy(pipeline);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "preview") == 0) return Preview(argc - 2, argv + 2);
    const char *mode = argc == 2 ? argv[1] : "complete";
    int compileFailure = strcmp(mode,"compile-failure") == 0;
    int testFailure = strcmp(mode,"test-failure") == 0;
    int noTests = strcmp(mode,"no-tests") == 0;
    int noCPack = strcmp(mode,"no-cpack") == 0;
    int deployFailure = strcmp(mode,"deploy-test-failure") == 0;
    CHECK(argc <= 2 && (strcmp(mode,"complete") == 0 || compileFailure || testFailure || noTests || noCPack || deployFailure));
    UmiDeveloperProjectService *projects = NULL;
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
    UmiBuildProjectSession *session = NULL;
    UmiBuildProjectSessionConfig config = {0};
    char temporary[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], name[160], path[UMI_PATH_CAPACITY];
    struct timespec now; CHECK(timespec_get(&now, TIME_UTC) == TIME_UTC);
    (void)snprintf(name,sizeof(name),"Umicom Notes-%lld-%ld-%s",(long long)now.tv_sec,now.tv_nsec,mode);
    CHECK(umi_fs_temp_directory(temporary,sizeof(temporary)) == UMI_STATUS_OK);
    CHECK(umi_fs_join(root,sizeof(root),temporary,name) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    printf("Private generated project: %s\n",root);
    CHECK(umi_developer_project_service_create(&projects) == UMI_STATUS_OK);
    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id,"developer.template.c23-console");
    strcpy(request.application_name,"Umicom Notes"); strcpy(request.application_id,"org.umicom.notes");
    strcpy(request.repository_name,"umicom-notes"); strcpy(request.target_name,"umicom_notes");
    strcpy(request.project_root,root);
    CHECK(UmiDeveloperProjectCreateNew(projects,&request,&report,&model,&profile) == UMI_STATUS_OK);
    CHECK(report.files_created >= 6U);
    CHECK(UmiDeveloperProjectCreateNew(projects,&request,&report,&model,&profile) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_fs_join(path,sizeof(path),root,"src/main.c") == UMI_STATUS_OK);
    const char *source = compileFailure ? "int main(void) { syntax error }\n" :
        "#include <stdio.h>\nint main(void) { unsigned savedNotes = 2U; printf(\"Umicom Notes: %u saved\\n\", savedNotes); return 0; }\n";
    CHECK(umi_fs_write_text(path,source) == UMI_STATUS_OK);
    CHECK(umi_fs_join(path,sizeof(path),root,"tests/test_main.c") == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(path,(testFailure || deployFailure) ? "int main(void) { return 7; }\n" :
        "int main(void) { unsigned count = 2U; return count == 2U ? 0 : 1; }\n") == UMI_STATUS_OK);
    if (noTests || noCPack) {
        CHECK(umi_fs_join(path,sizeof(path),root,"CMakeLists.txt") == UMI_STATUS_OK);
        char *cmake = NULL; CHECK(umi_fs_read_text(path,&cmake,NULL) == UMI_STATUS_OK);
        char *line = strstr(cmake,noTests ? "    add_test(" : "include(CPack)"); CHECK(line != NULL);
        char *end = strchr(line,'\n'); CHECK(end != NULL);
        memset(line,' ',(size_t)(end-line));
        CHECK(umi_fs_write_text(path,cmake) == UMI_STATUS_OK); umi_fs_free_text(cmake);
    }
    /* Generated profiles may use paths relative to their source root. Convert
     * this example's paths for independent filesystem verification below. */
    if (!umi_fs_is_absolute(profile.build_directory)) {
        CHECK(umi_fs_join(path,sizeof(path),root,profile.build_directory) == UMI_STATUS_OK);
        strcpy(profile.build_directory,path);
    }
    if (!umi_fs_is_absolute(profile.install_directory)) {
        CHECK(umi_fs_join(path,sizeof(path),root,profile.install_directory) == UMI_STATUS_OK);
        strcpy(profile.install_directory,path);
    }
    profile.timeout_ms = 30000U; profile.parallel_jobs = 2U;
    CHECK(umi_build_project_session_create(&config,&session) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_submit(session,&profile,UMI_BUILD_PHASE_PACKAGE,false) == UMI_STATUS_PERMISSION_DENIED);
    size_t phases = 0U;
    if (compileFailure || testFailure || noTests || noCPack || deployFailure) {
        CHECK(Run(session,&profile,deployFailure ? UMI_BUILD_PHASE_DEPLOY : UMI_BUILD_PHASE_PACKAGE,0,&phases) == EXIT_SUCCESS);
        CHECK(phases == (compileFailure ? 2U : noCPack ? 4U : 3U));
        CHECK(!umi_fs_is_directory(profile.install_directory));
    } else {
        CHECK(Run(session,&profile,UMI_BUILD_PHASE_PACKAGE,1,&phases) == EXIT_SUCCESS && phases == 4U);
        CHECK(umi_fs_join(path,sizeof(path),profile.build_directory,"packages") == UMI_STATUS_OK);
        UmiDirectoryWalkOptions options = umi_directory_walk_options_default(); options.recursive = 0;
        Found found = {0}; CHECK(umi_directory_walk(path,&options,Find,&found) == UMI_STATUS_OK);
        CHECK(found.zips == 1U);
        char checksumPath[UMI_PATH_CAPACITY], *checksum = NULL;
        CHECK(snprintf(checksumPath,sizeof(checksumPath),"%s.sha256",found.zip) < (int)sizeof(checksumPath));
        CHECK(umi_fs_read_text(checksumPath,&checksum,NULL) == UMI_STATUS_OK && strlen(checksum) >= 64U);
        char hash[65]; memcpy(hash,checksum,64U); hash[64] = '\0'; umi_fs_free_text(checksum);
        const char *hashArgs[] = {"-E","sha256sum",found.zip};
        CHECK(Execute("cmake",hashArgs,3U,root,hash) == EXIT_SUCCESS);
        char extracted[UMI_PATH_CAPACITY]; CHECK(umi_fs_join(extracted,sizeof(extracted),root,"extracted") == UMI_STATUS_OK);
        CHECK(umi_fs_make_directories(extracted) == UMI_STATUS_OK);
        const char *extractArgs[] = {"-E","tar","xvf",found.zip};
        CHECK(Execute("cmake",extractArgs,4U,extracted,NULL) == EXIT_SUCCESS);
        options.recursive = 1; memset(&found,0,sizeof(found));
        CHECK(umi_directory_walk(extracted,&options,Find,&found) == UMI_STATUS_OK && found.program[0] != '\0');
        CHECK(Execute(found.program,NULL,0U,extracted,"Umicom Notes: 2 saved") == EXIT_SUCCESS);
        CHECK(Run(session,&profile,UMI_BUILD_PHASE_DEPLOY,1,&phases) == EXIT_SUCCESS && phases == 4U);
        memset(&found,0,sizeof(found));
        CHECK(umi_directory_walk(profile.install_directory,&options,Find,&found) == UMI_STATUS_OK && found.program[0] != '\0');
        CHECK(Execute(found.program,NULL,0U,profile.install_directory,"Umicom Notes: 2 saved") == EXIT_SUCCESS);
        CHECK(Run(session,&profile,UMI_BUILD_PHASE_REBUILD,1,&phases) == EXIT_SUCCESS && phases == 3U);
    }
    umi_build_project_session_destroy(session); umi_developer_project_service_destroy(projects);
    /* This path was created above by this process; no caller-selected folder is removed. */
    CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    puts("Notes workflow checks passed."); return EXIT_SUCCESS;
}
