/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_session_metadata.c
 *
 * PURPOSE:
 *   Verify retained failed-start owners cannot publish submitted-session metadata.
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
#include "umicom/local_models/runtime.h"
/* Private state injection exercises cleanup-owner admission without launching
 * a process. The fake supervisor is never dereferenced by metadata reads. */
#include "../../src/local_models/session.c"
#ifdef _WIN32
#define SERVER_PATH "C:/umicom-test/llama-server.exe"
#define MODEL_PATH "C:/umicom-test/local.gguf"
#else
#define SERVER_PATH "/umicom-test/llama-server"
#define MODEL_PATH "/umicom-test/local.gguf"
#endif
int main(int argc, char **argv)
{
    UmiModelSession session = {0};
    UmiModelRunPlan output, sentinel;
    UmiAiRuntimeDescriptor descriptor, descriptorSentinel;
    assert(argc == 2);
    memset(&sentinel, 0x5a, sizeof(sentinel)); memcpy(&output, &sentinel, sizeof(output));
    memset(&descriptorSentinel, 0x5a, sizeof(descriptorSentinel));
    memcpy(&descriptor, &descriptorSentinel, sizeof(descriptor));
    assert(UmiModelRunPlanCreate(SERVER_PATH, MODEL_PATH,
        "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        "captured", 18080U, 2048U, 4U, &session.plan) == UMI_STATUS_OK);
    if (strcmp(argv[1], "partial") == 0) {
        assert(UmiModelSessionReadPlan(&session, &output) == UMI_STATUS_INVALID_STATE);
        session.supervisor = (UmiProcessSupervisor *)&session;
        assert(UmiModelSessionReadPlan(&session, &output) == UMI_STATUS_INVALID_STATE);
        assert(UmiModelSessionRuntimeDescribe(&session, "runtime", "Label", 256U, &descriptor) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
        assert(memcmp(&descriptor, &descriptorSentinel, sizeof(descriptor)) == 0);
    } else {
        assert(strcmp(argv[1], "copy") == 0);
        session.supervisor = (UmiProcessSupervisor *)&session; session.job = 1U;
        assert(UmiModelSessionReadPlan(&session, &output) == UMI_STATUS_OK);
        assert(strcmp(output.alias, "captured") == 0);
        strcpy(output.alias, "caller-edit");
        assert(strcmp(session.plan.alias, "captured") == 0);
        memset(session.plan.port, '9', sizeof(session.plan.port));
        memcpy(&output, &sentinel, sizeof(output));
        assert(UmiModelSessionReadPlan(&session, &output) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
    }
    return 0;
}
