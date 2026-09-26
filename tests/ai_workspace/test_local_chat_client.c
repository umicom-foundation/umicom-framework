/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_client.c
 *
 * PURPOSE:
 *   Run the actual loopback provider through reviewed workspace jobs against an inert local HTTP fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/providers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
static DWORD WINAPI CancelLater(LPVOID token) { Sleep(120U); UmiAiWorkspaceCancellationRequest(token); return 0U; }
#else
#include <pthread.h>
#include <time.h>
static void *CancelLater(void *token)
{
    struct timespec delay = {0, 120000000L}; (void)nanosleep(&delay, NULL);
    UmiAiWorkspaceCancellationRequest(token); return NULL;
}
#endif
int main(int argc, char **argv)
{
    if (argc < 4) return 2;
    if (!UmiAiWorkspaceLocalProviderAvailable()) return 77;
    unsigned long port = strtoul(argv[1], NULL, 10); int expected = atoi(argv[2]); unsigned long timeout = strtoul(argv[3], NULL, 10);
    if (port < 1024UL || port > 65535UL || timeout < 100UL || timeout > 60000UL) return 2;
    UmiDataServer *data = NULL; UmiAiRuntime runtime; UmiAiProvider provider = {0};
    UmiAiWorkspace *workspace = NULL; UmiAiWorkspaceCancellation *token = NULL; UmiStatus status;
    umi_ai_runtime_init(&runtime);
    status = UmiAiWorkspaceCancellationCreate(&token);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceLocalProviderCreate((uint16_t)port, (uint32_t)timeout, token, &provider);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_provider_registry_add(&runtime.providers, &provider);
        if (status != UMI_STATUS_OK && provider.destroy != NULL) provider.destroy(provider.instance);
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_create_memory(&data);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceCreate(data, &runtime, "http-test", &workspace);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspacePrepare(workspace, "http.job", UMI_AI_WORKSPACE_DRAFT,
        "umicom.local-chat", "test-model", NULL, "Write a short workshop notice.", "writer", 32U);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceReview(workspace, "http.job", "reviewer", true);
    bool concurrentCancel = argc > 4 && strcmp(argv[4], "cancel") == 0;
#ifdef _WIN32
    HANDLE worker = NULL;
    if (status == UMI_STATUS_OK && concurrentCancel) { worker = CreateThread(NULL, 0U, CancelLater, token, 0U, NULL); if (worker == NULL) status = UMI_STATUS_INTERNAL_ERROR; }
#else
    pthread_t worker; bool created = false;
    if (status == UMI_STATUS_OK && concurrentCancel) { created = pthread_create(&worker, NULL, CancelLater, token) == 0; if (!created) status = UMI_STATUS_INTERNAL_ERROR; }
#endif
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceRun(workspace, "http.job", token);
#ifdef _WIN32
    if (worker != NULL) { (void)WaitForSingleObject(worker, INFINITE); (void)CloseHandle(worker); }
#else
    if (created) (void)pthread_join(worker, NULL);
#endif
    int result = (int)status == expected ? 0 : 1;
    if (result != 0) fprintf(stderr, "expected %d, received %d\n", expected, (int)status);
    if (result == 0 && status == UMI_STATUS_OK) {
        UmiAiWorkspaceJob *job = calloc(1U, sizeof(*job));
        if (job == NULL || UmiAiWorkspaceJobFind(workspace, "http.job", job) != UMI_STATUS_OK ||
            job->state != UMI_AI_WORKSPACE_SUCCEEDED || strcmp(job->response.text, "A local draft.") != 0) result = 1;
        free(job);
    }
    UmiAiWorkspaceDestroy(workspace); umi_data_server_destroy(data); umi_ai_runtime_destroy(&runtime);
    UmiAiWorkspaceCancellationDestroy(token); return result;
}
