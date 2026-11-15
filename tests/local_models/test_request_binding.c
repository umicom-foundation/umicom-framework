/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_request_binding.c
 *
 * PURPOSE:
 *   Verify host correlation rejects stale sessions and requests without process or network activity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/local_models/request_binding.h"
#include "umicom/platform/threading.h"
/* Private owner and exhaustion fixtures run the production implementation.
 * The fake supervisor is metadata only and is never dereferenced. */
#include "../../src/local_models/session.c"
#ifdef _WIN32
#define SERVER_PATH "C:/umicom-test/llama-server.exe"
#define MODEL_PATH "C:/umicom-test/local.gguf"
#else
#define SERVER_PATH "/umicom-test/llama-server"
#define MODEL_PATH "/umicom-test/local.gguf"
#endif

static void MakeOwner(UmiModelSession *owner)
{
    memset(owner, 0, sizeof(*owner));
    assert(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH,
        "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        "captured", 18080U, 2048U, 4U, &owner->plan) == UMI_STATUS_OK);
    owner->supervisor = (UmiProcessSupervisor *)owner;
    owner->job = 1U;
    assert(ModelSessionIdentityReserve(&owner->identity) == UMI_STATUS_OK);
}

static void CheckBind(const char *name)
{
    UmiModelSession owner;
    MakeOwner(&owner);
    UmiModelRequestBinding output, before;
    memset(&output, 0x5a, sizeof(output));
    char request[128] = "request.1";
    const char *id = request;
    const UmiModelSession *input = &owner;
    UmiModelRequestBinding *destination = &output;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "null-session") == 0) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-output") == 0) { destination = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-request") == 0) { id = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "empty-request") == 0) { request[0] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "invalid-request") == 0) { strcpy(request, "has space"); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "unterminated-request") == 0) { memset(request, 'x', sizeof(request)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "maximum-request") == 0) { memset(request, 'x', sizeof(request)-1U); request[127] = '\0'; }
    else if (strcmp(name, "partial-supervisor") == 0) { owner.supervisor = NULL; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "partial-job") == 0) { owner.job = 0U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "zero-identity") == 0) { owner.identity = 0U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "invalid-plan") == 0) { memset(owner.plan.port, '9', sizeof(owner.plan.port)); expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "alias-input") == 0) { strcpy(output.request_id, request); id = output.request_id; }
    else assert(strcmp(name, "valid") == 0 || strcmp(name, "copy") == 0);
    memcpy(&before, &output, sizeof(before));
    UmiModelSession ownerBefore;
    memcpy(&ownerBefore, &owner, sizeof(owner));
    assert(UmiModelSessionBindRequest(input, id, destination) == expected);
    assert(memcmp(&ownerBefore, &owner, sizeof(owner)) == 0);
    if (expected != UMI_STATUS_OK) { assert(memcmp(&before, &output, sizeof(output)) == 0); return; }
    assert(output.session_identity == owner.identity);
    assert(strcmp(output.request_id, request) == 0);
    assert(strcmp(output.model_sha256, owner.plan.sha256) == 0);
    assert(strcmp(output.model_alias, "captured") == 0);
    assert(output.port == 18080U && output.context_tokens == 2048U && output.threads == 4U);
    if (strcmp(name, "copy") == 0) {
        strcpy(output.model_alias, "caller-edit"); strcpy(request, "changed");
        assert(strcmp(owner.plan.alias, "captured") == 0);
        assert(strcmp(output.request_id, "request.1") == 0);
    }
}

static void CheckComparison(const char *group, const char *name)
{
    UmiModelSession owner;
    MakeOwner(&owner);
    UmiModelRequestBinding binding, before;
    assert(UmiModelSessionBindRequest(&owner, "request.1", &binding) == UMI_STATUS_OK);
    const UmiModelSession *input = &owner;
    const UmiModelRequestBinding *record = &binding;
    const char *request = "request.1";
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(group, "malformed") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(name, "request") == 0) memset(binding.request_id, 'x', sizeof(binding.request_id));
        else if (strcmp(name, "alias") == 0) memset(binding.model_alias, 'x', sizeof(binding.model_alias));
        else if (strcmp(name, "alias-option") == 0) strcpy(binding.model_alias, "--other");
        else if (strcmp(name, "digest") == 0) memset(binding.model_sha256, 'x', sizeof(binding.model_sha256));
        else if (strcmp(name, "identity") == 0) binding.session_identity = 0U;
        else if (strcmp(name, "port") == 0) binding.port = 1023U;
        else if (strcmp(name, "context-low") == 0) binding.context_tokens = 511U;
        else if (strcmp(name, "context-high") == 0) binding.context_tokens = 32769U;
        else if (strcmp(name, "threads-low") == 0) binding.threads = 0U;
        else { assert(strcmp(name, "threads-high") == 0); binding.threads = 257U; }
    } else if (strcmp(name, "match") == 0) expected = UMI_STATUS_OK;
    else if (strcmp(name, "identity") == 0) ++binding.session_identity;
    else if (strcmp(name, "request") == 0) request = "request.2";
    else if (strcmp(name, "digest") == 0) binding.model_sha256[0] = 'a';
    else if (strcmp(name, "alias") == 0) strcpy(binding.model_alias, "other");
    else if (strcmp(name, "port") == 0) binding.port = 19090U;
    else if (strcmp(name, "context") == 0) binding.context_tokens = 4096U;
    else if (strcmp(name, "threads") == 0) binding.threads = 8U;
    else if (strcmp(name, "null-session") == 0) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-binding") == 0) { record = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-request") == 0) { request = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "empty-request") == 0) { request = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else { assert(strcmp(name, "invalid-request") == 0); request = "has space"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    memcpy(&before, &binding, sizeof(before));
    UmiModelSession ownerBefore;
    memcpy(&ownerBefore, &owner, sizeof(owner));
    assert(UmiModelSessionCheckBinding(input, record, request) == expected);
    assert(memcmp(&before, &binding, sizeof(binding)) == 0);
    assert(memcmp(&ownerBefore, &owner, sizeof(owner)) == 0);
}

typedef struct IdentityWorker { uint64_t values[64]; } IdentityWorker;
static int ReserveIdentities(void *data)
{
    IdentityWorker *worker = data;
    for (size_t i = 0U; i < 64U; ++i)
        if (ModelSessionIdentityReserve(&worker->values[i]) != UMI_STATUS_OK) return 1;
    return 0;
}
static void CheckIdentity(const char *name)
{
    if (strcmp(name, "concurrent") == 0) {
        IdentityWorker workers[4] = {0};
        UmiThread *threads[4] = {0};
        for (size_t i = 0U; i < 4U; ++i) assert(umi_thread_start(ReserveIdentities, &workers[i], &threads[i]) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 4U; ++i) {
            int result = -1;
            assert(umi_thread_join(threads[i], &result) == UMI_STATUS_OK && result == 0);
            assert(UmiThreadRelease(&threads[i]) == UMI_STATUS_OK);
        }
        bool seen[257] = {false};
        for (size_t i = 0U; i < 4U; ++i) for (size_t j = 0U; j < 64U; ++j) {
            uint64_t id = workers[i].values[j];
            assert(id >= 1U && id <= 256U && !seen[(size_t)id]); seen[(size_t)id] = true;
        }
    } else if (strcmp(name, "exhaustion") == 0) {
        atomic_store_explicit(&modelSessionIdentity, UINT64_MAX-1U, memory_order_relaxed);
        uint64_t id = 99U;
        assert(ModelSessionIdentityReserve(&id) == UMI_STATUS_OK && id == UINT64_MAX);
        id = 99U;
        assert(ModelSessionIdentityReserve(&id) == UMI_STATUS_CAPACITY_EXCEEDED && id == 99U);
        assert(ModelSessionIdentityReserve(&id) == UMI_STATUS_CAPACITY_EXCEEDED && id == 99U);
        assert(atomic_load_explicit(&modelSessionIdentity, memory_order_relaxed) == UINT64_MAX);
    } else {
        assert(strcmp(name, "reserve") == 0);
        uint64_t first = 0U, second = 0U;
        assert(ModelSessionIdentityReserve(&first) == UMI_STATUS_OK && first == 1U);
        assert(ModelSessionIdentityReserve(&second) == UMI_STATUS_OK && second == 2U);
    }
}
int main(int argc, char **argv)
{
    assert(argc == 3);
    if (strcmp(argv[1], "bind") == 0) CheckBind(argv[2]);
    else if (strcmp(argv[1], "identity") == 0) CheckIdentity(argv[2]);
    else { assert(strcmp(argv[1], "check") == 0 || strcmp(argv[1], "malformed") == 0); CheckComparison(argv[1], argv[2]); }
    return 0;
}
