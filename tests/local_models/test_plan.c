/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_plan.c
 *
 * PURPOSE:
 *   Exercise model-source admission, consent boundaries and literal process plans without network access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#ifdef _WIN32
#define MODEL_PATH "C:/umicom-test/model with spaces.gguf"
#define SERVER_PATH "C:/umicom-test/llama-server.exe"
#else
#define MODEL_PATH "/umicom-test/model with spaces.gguf"
#define SERVER_PATH "/umicom-test/llama-server"
#endif
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    const UmiModelCatalogueEntry *entry = UmiModelCatalogueAt(0U);
    CHECK(entry != NULL);
    UmiModelSource source = entry->source;
    UmiModelDownloadPlan download;
    UmiModelRunPlan run;
    if (strcmp(name, "catalogue") == 0) {
        CHECK(UmiModelCatalogueCount() > 0U);
        for (size_t i = 0U; i < UmiModelCatalogueCount(); ++i)
            CHECK(UmiModelSourceValidate(&UmiModelCatalogueAt(i)->source) == UMI_STATUS_OK);
        CHECK(UmiModelCatalogueAt(SIZE_MAX) == NULL);
        CHECK(UmiModelCatalogueFind(entry->id) == entry);
        CHECK(UmiModelCatalogueFind("unknown") == NULL && UmiModelCatalogueFind(NULL) == NULL);
        return 0;
    }
    if (strcmp(name, "source-rejection") == 0) {
        const char *bad[] = {"owner/../repo", "owner/repo/extra", "owner/repo?token=x", "owner/repo#x", "/owner/repo", "owner//repo"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            source = entry->source; (void)snprintf(source.repository, sizeof(source.repository), "%s", bad[i]);
            CHECK(UmiModelSourceValidate(&source) == UMI_STATUS_INVALID_ARGUMENT);
        }
        source = entry->source; (void)snprintf(source.revision, sizeof(source.revision), "main");
        CHECK(UmiModelSourceValidate(&source) == UMI_STATUS_INVALID_ARGUMENT);
        source = entry->source; source.sha256[0] = 'z';
        CHECK(UmiModelSourceValidate(&source) == UMI_STATUS_INVALID_ARGUMENT);
        source = entry->source; source.bytes = UMI_MODEL_MAX_BYTES + 1U;
        CHECK(UmiModelSourceValidate(&source) == UMI_STATUS_INVALID_ARGUMENT);
        source = entry->source; (void)snprintf(source.filename, sizeof(source.filename), "../model.gguf");
        CHECK(UmiModelSourceValidate(&source) == UMI_STATUS_INVALID_ARGUMENT);
        source = entry->source; memset(source.filename, 'x', sizeof(source.filename));
        CHECK(UmiModelSourceValidate(&source) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "download-admission") == 0) {
        CHECK(UmiModelDownloadPlanCreate(&source, MODEL_PATH, &download) == UMI_STATUS_OK);
        CHECK(strstr(download.url, source.revision) != NULL && strstr(download.url, source.filename) != NULL);
        UmiModelInspection inspected;
        CHECK(UmiModelDownload(&download, false, NULL, &inspected) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(inspected.bytes == 0U);
        (void)snprintf(download.url, sizeof(download.url), "http://127.0.0.1/private");
        CHECK(UmiModelDownload(&download, true, NULL, &inspected) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelDownloadPlanCreate(&source, "relative.gguf", &download) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelDownloadPlanCreate(&source, SERVER_PATH, &download) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "download-cancelled") == 0) {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiModelDownloadPlanCreate(&source, MODEL_PATH, &download) == UMI_STATUS_OK);
        UmiModelInspection inspected;
        CHECK(UmiModelDownload(&download, true, cancel, &inspected) == UMI_STATUS_CANCELLED);
        umi_cancellation_token_destroy(cancel);
        return 0;
    }
    CHECK(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, source.sha256, "my-model", 8080U, 2048U, 4U, &run) == UMI_STATUS_OK);
    if (strcmp(name, "run-arguments") == 0) {
        const char *arguments[UMI_MODEL_RUN_ARGUMENTS]; size_t count = 99U;
        CHECK(UmiModelRunArguments(&run, arguments, UMI_MODEL_RUN_ARGUMENTS - 1U, &count) == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
        CHECK(UmiModelRunArguments(&run, arguments, UMI_MODEL_RUN_ARGUMENTS, &count) == UMI_STATUS_OK);
        CHECK(count == 18U && strcmp(arguments[1], MODEL_PATH) == 0);
        CHECK(strcmp(arguments[3], "127.0.0.1") == 0 && strcmp(arguments[11], "0") == 0);
        CHECK(strcmp(arguments[13], "1") == 0 && strcmp(arguments[16], "--offline") == 0);
        CHECK(strcmp(arguments[17], "--no-webui") == 0);
        CHECK(strstr(run.guard, ".gguf.umicom-run.lock") != NULL);
        UmiModelSession *session = NULL;
        CHECK(UmiModelSessionStart(&run, false, &session) == UMI_STATUS_PERMISSION_DENIED && session == NULL);
        (void)snprintf(run.directory, sizeof(run.directory), "relative");
        CHECK(UmiModelSessionStart(&run, true, &session) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "run-limits") == 0) {
        CHECK(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, source.sha256, "bad alias", 8080U, 2048U, 4U, &run) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, source.sha256, "-flag", 8080U, 2048U, 4U, &run) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, source.sha256, "valid", 80U, 2048U, 4U, &run) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, source.sha256, "valid", 8080U, 32769U, 4U, &run) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH, source.sha256, "valid", 8080U, 2048U, 0U, &run) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelRunPlanCreate("llama-server", MODEL_PATH, source.sha256, "valid", 8080U, 2048U, 4U, &run) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "cli-admission") == 0) {
        CHECK(UmiModelsCommand(-1, NULL) == 2 && UmiModelsCommand(1, NULL) == 2);
        char *nullItem[] = {NULL}; CHECK(UmiModelsCommand(1, nullItem) == 2);
        char *missing[] = {"download"}; CHECK(UmiModelsCommand(1, missing) == 2);
        char *badPort[] = {"chat", "65536", "model", "hello", "--send-reviewed-prompt"};
        CHECK(UmiModelsCommand(5, badPort) == 2);
        char *badFlag[] = {"chat", "8080", "model", "hello", "--apply"};
        CHECK(UmiModelsCommand(5, badFlag) == 2);
        char *tooMany[10] = {0}; CHECK(UmiModelsCommand(10, tooMany) == 2);
        return 0;
    }
    if (strcmp(name, "preview") == 0) {
        char *chat[] = {"chat", "8080", "my-model", "A reviewed greeting"};
        CHECK(UmiModelsCommand(4, chat) == 0);
        char *downloadArgs[] = {"download", "qwen2.5-0.5b-q4", MODEL_PATH};
        CHECK(UmiModelsCommand(3, downloadArgs) == 0);
        char *runArgs[] = {"run", "qwen2.5-0.5b-q4", SERVER_PATH, MODEL_PATH, "8080", "2048", "4"};
        CHECK(UmiModelsCommand(7, runArgs) == 0);
        return 0;
    }
    return 2;
}
