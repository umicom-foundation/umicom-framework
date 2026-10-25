/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_document_concurrency.c
 *
 * PURPOSE:
 *   A worker must not read or append through another thread's transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A worker must not read or append through another thread's transaction. */
#include "umicom/data/record_document.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

typedef struct DocumentWorker {
    UmiDataServer *server;
    UmiStatus append_status;
    UmiStatus read_status;
    UmiStatus verify_status;
    int outputs_preserved;
} DocumentWorker;
static void perform(DocumentWorker *worker)
{
    UmiRecordAmendment metadata = {.entity_id = "worker", .source = "synthetic",
        .actor = "fictional-worker", .reason = "Exercise ownership", .recorded_at_nanoseconds = 100U};
    UmiRecordRevision revision;
    UmiRecordDocument document;
    memset(&revision, 0xa5, sizeof revision); memset(&document, 0xa5, sizeof document);
    unsigned char before_revision[sizeof revision], before_document[sizeof document];
    memcpy(before_revision, &revision, sizeof revision); memcpy(before_document, &document, sizeof document);
    worker->append_status = umi_record_document_append(worker->server, &metadata, "abc", 3U, 0U, &revision);
    worker->read_status = umi_record_document_latest(worker->server, "worker", &document);
    uint64_t count = 77U;
    worker->verify_status = umi_record_document_verify(worker->server, "worker", &count);
    worker->outputs_preserved = memcmp(before_revision, &revision, sizeof revision) == 0 &&
        memcmp(before_document, &document, sizeof document) == 0 && count == 77U;
}
#ifdef _WIN32
static DWORD WINAPI entry(LPVOID context) { perform(context); return 0U; }
static int join(DocumentWorker *worker)
{
    HANDLE thread = CreateThread(NULL, 0, entry, worker, 0, NULL);
    if (thread == NULL) return 1;
    DWORD result = WaitForSingleObject(thread, INFINITE);
    (void)CloseHandle(thread);
    return result == WAIT_OBJECT_0 ? 0 : 1;
}
#else
static void *entry(void *context) { perform(context); return NULL; }
static int join(DocumentWorker *worker)
{
    pthread_t thread;
    if (pthread_create(&thread, NULL, entry, worker) != 0) return 1;
    return pthread_join(thread, NULL);
}
#endif
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiDataServer *server = NULL;
    if (strcmp(argv[1], "memory") == 0) { OK(umi_data_server_create_memory(&server)); }
    else if (strcmp(argv[1], "sqlite") == 0) { OK(umi_data_server_create_sqlite(":memory:", &server)); }
    else return 2;
    DocumentWorker worker = {.server = server};
    OK(umi_data_server_begin(server));
    OK(umi_data_server_set(server, "owner-draft", "retained"));
    CHECK(join(&worker) == 0);
    CHECK(worker.append_status == UMI_STATUS_BUSY && worker.read_status == UMI_STATUS_BUSY &&
        worker.verify_status == UMI_STATUS_BUSY && worker.outputs_preserved);
    CHECK(umi_data_server_in_transaction(server));
    OK(umi_data_server_commit(server));
    CHECK(join(&worker) == 0);
    CHECK(worker.append_status == UMI_STATUS_OK && worker.read_status == UMI_STATUS_OK && worker.verify_status == UMI_STATUS_OK);
    char text[16]; OK(umi_data_server_get(server, "owner-draft", text, sizeof text)); CHECK(strcmp(text, "retained") == 0);
    umi_data_server_destroy(server);
    return 0;
}
