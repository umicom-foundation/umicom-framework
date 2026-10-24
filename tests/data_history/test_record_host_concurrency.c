/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Start competing owners together; accept only outcomes allowed by one total order. */
#include "record_host_fixture.h"
#include <stdatomic.h>
typedef enum HostWorkKind { WRITE, RETIRE, DENY } HostWorkKind;
typedef struct HostWork {
  UmiRecordHost *host;
  UmiRecordAccessRequest request;
  atomic_bool *start;
  HostWorkKind kind;
  uint64_t expected;
  UmiStatus result;
} HostWork;
static int worker(void *data) {
  HostWork *work = data;
  while (!atomic_load_explicit(work->start, memory_order_acquire))
    umi_thread_sleep_ms(1);
  if (work->kind == RETIRE)
    work->result = umi_record_host_retire(work->host);
  else if (work->kind == DENY) {
    uint64_t revision;
    work->result = host_policy(work->host, UMI_POLICY_DENY, &revision);
  } else {
    UmiAuthorizedRecordRevision saved;
    work->result = umi_record_host_append(work->host, &work->request, "Concurrent fictional edit",
                                          100, "worker", 6, work->expected, &saved);
  }
  umi_record_host_release(work->host);
  return 0;
}
static int exercise(HostFixture *f, const char *name) {
  if (strcmp(name, "competing-create") && strcmp(name, "competing-amend") &&
      strcmp(name, "retire-race") && strcmp(name, "policy-race") &&
      strcmp(name, "retained-workers"))
    return 2;
  uint64_t expected = 0;
  if (!strcmp(name, "competing-amend")) {
    UmiAuthorizedRecordRevision seed;
    OK(host_save(f, "initial", 0, &seed));
    expected = 1;
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
  }
  /* Worker arguments must outlive an unsuccessful join too. Keep the former
   * stack setup for review; heap storage is freed only after both joins. */
#if 0
    atomic_bool start; atomic_init(&start, false);
    HostWork work[2] = {
        {.host = f->host, .request = f->request, .start = &start, .kind = WRITE, .expected = expected},
        {.host = f->host, .request = f->request, .start = &start, .kind = WRITE, .expected = expected}
    };
#endif
  typedef struct WorkGroup {
    atomic_bool start;
    HostWork work[2];
  } WorkGroup;
  WorkGroup *group = calloc(1, sizeof *group);
  CHECK(group != NULL);
  atomic_init(&group->start, false);
  HostWork *work = group->work;
  for (size_t i = 0; i < 2; ++i)
    work[i] = (HostWork){.host = f->host,
                         .request = f->request,
                         .start = &group->start,
                         .kind = WRITE,
                         .expected = expected};
  if (!strcmp(name, "retire-race"))
    work[1].kind = RETIRE;
  if (!strcmp(name, "policy-race"))
    work[1].kind = DENY;
  UmiThread *threads[2] = {NULL, NULL};
  UmiStatus creation = UMI_STATUS_OK;
  size_t started = 0;
  for (size_t i = 0; i < 2; ++i) {
    creation = umi_record_host_retain(f->host);
    if (creation != UMI_STATUS_OK)
      break;
    creation = umi_thread_start(worker, &work[i], &threads[i]);
    if (creation != UMI_STATUS_OK) {
      umi_record_host_release(f->host);
      break;
    }
    ++started;
  }
  if (!strcmp(name, "retained-workers") && started == 2) {
    umi_record_host_release(f->host);
    f->host = NULL;
  }
  atomic_store_explicit(&group->start, true, memory_order_release);
  bool joined = true;
  for (size_t i = 0; i < started; ++i) {
    int result = 99;
    UmiStatus s = umi_thread_join(threads[i], &result);
    if (s != UMI_STATUS_OK || result != 0)
      joined = false;
    /* A failed join does not prove the worker stopped. Leave its handle
     * and heap arguments alive until process exit on that failure path. */
    if (s == UMI_STATUS_OK)
      umi_thread_destroy(threads[i]);
  }
  CHECK(creation == UMI_STATUS_OK && started == 2 && joined);
  if (work[1].kind == WRITE) {
    CHECK((work[0].result == UMI_STATUS_OK && work[1].result == UMI_STATUS_INVALID_STATE) ||
          (work[1].result == UMI_STATUS_OK && work[0].result == UMI_STATUS_INVALID_STATE));
  } else {
    CHECK(work[1].result == UMI_STATUS_OK);
    CHECK(work[0].result == UMI_STATUS_OK ||
          work[0].result ==
              (work[1].kind == RETIRE ? UMI_STATUS_CANCELLED : UMI_STATUS_PERMISSION_DENIED));
    UmiAuthorizedRecordRevision saved;
    memset(&saved, 0xa5, sizeof saved);
    CHECK(host_save(f, "after change", 0, &saved) ==
          (work[1].kind == RETIRE ? UMI_STATUS_CANCELLED : UMI_STATUS_PERMISSION_DENIED));
    CHECK(host_filled(&saved, sizeof saved, 0xa5));
  }
  if (f->host && work[1].kind == WRITE) {
    UmiAuthorizedRecordDocument *document = malloc(sizeof *document);
    CHECK(document != NULL);
    f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
    OK(umi_record_host_latest(f->host, &f->request, document));
    CHECK(document->document.record.revision == expected + 1);
    free(document);
  }
  free(group);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  HostFixture f;
  UmiStatus s = host_new(&f, argv[1], true);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[2]) : 1;
  umi_record_host_release(f.host);
  return result;
}
