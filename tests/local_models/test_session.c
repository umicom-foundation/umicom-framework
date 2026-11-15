/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_session.c
 *
 * PURPOSE:
 *   Exercise model-session ownership, failed replacement and cooperative lease cleanup with a finite child fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include "umicom/local_models/runtime.h"
#include "umicom/local_models/request_binding.h"
#include "umicom/base/sha256.h"
#include "umicom/platform/threading.h"
#include "../build_log/fixture.h"
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "fixture.gguf");
    unsigned char bytes[128] = {0}; memcpy(bytes, "GGUF", 4U); bytes[4] = 3U; bytes[8] = 1U;
    char digest[65]; CHECK(UmiSha256Buffer(bytes, sizeof(bytes), digest) == UMI_STATUS_OK);
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, sizeof(bytes)) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
    UmiModelRunPlan plan;
    CHECK(UmiModelRunPlanCreate(argv[2], path, digest, "fixture", 18080U, 512U, 1U, &plan) == UMI_STATUS_OK);
    UmiModelSession *session = NULL;
    if (strcmp(argv[1], "wrong-hash") == 0) {
        plan.sha256[0] = plan.sha256[0] == 'a' ? 'b' : 'a';
        CHECK(UmiModelSessionStart(&plan, true, &session) == UMI_STATUS_INVALID_STATE && session == NULL);
        memcpy(plan.sha256, digest, 65U);
        CHECK(UmiModelSessionStart(&plan, true, &session) == UMI_STATUS_OK);
        CHECK(UmiModelSessionStop(&session) == UMI_STATUS_OK && session == NULL);
        return 0;
    }
    if (strcmp(argv[1], "lifetime") != 0) return 2;
    CHECK(UmiModelSessionStart(&plan, true, &session) == UMI_STATUS_OK && session != NULL);
    /* The real submitted owner must keep the original settings even when the
     * next form/edit buffer is changed. Reading metadata never marks it ready. */
    UmiModelRunPlan captured, original = plan;
    strcpy(plan.alias, "changed-after-start");
    strcpy(plan.port, "19090");
    CHECK(UmiModelSessionReadPlan(session, &captured) == UMI_STATUS_OK);
    CHECK(strcmp(captured.alias, "fixture") == 0 && strcmp(captured.port, "18080") == 0);
    UmiAiRuntimeDescriptor descriptor;
    CHECK(UmiModelSessionRuntimeDescribe(session, "runtime.fixture", "Fixture", 256U, &descriptor) == UMI_STATUS_OK);
    CHECK(strcmp(descriptor.model_id, "fixture") == 0 && !descriptor.health.available && descriptor.revision == 0U);
    plan = original;
    /* Native submission must receive the identity used by host reply checks.
     * A later owner of the same plan must reject this captured request. */
    UmiModelRequestBinding originalBinding;
    CHECK(UmiModelSessionBindRequest(session, "fixture.request", &originalBinding) == UMI_STATUS_OK);
    CHECK(UmiModelSessionCheckBinding(session, &originalBinding, "fixture.request") == UMI_STATUS_OK);
    UmiModelSession *retained = session;
    CHECK(UmiModelSessionStart(NULL, true, &session) == UMI_STATUS_BUSY && session == retained);
    UmiModelSession *second = NULL;
    CHECK(UmiModelSessionStart(&plan, true, &second) == UMI_STATUS_BUSY && second == NULL);
    UmiProcessJobSnapshot snapshot;
    bool observed = false;
    for (unsigned i = 0U; i < 200U; ++i) {
        CHECK(UmiModelSessionRead(session, &snapshot) == UMI_STATUS_OK);
        if (strstr(snapshot.output, "Owned fixture started") != NULL) { observed = true; break; }
        CHECK(snapshot.state != UMI_PROCESS_JOB_FAILED);
        umi_thread_sleep_ms(10U);
    }
    CHECK(observed);
    CHECK(UmiModelSessionStop(&session) == UMI_STATUS_OK && session == NULL);
    CHECK(UmiModelSessionStop(&session) == UMI_STATUS_OK);
    CHECK(UmiModelSessionStart(&plan, true, &second) == UMI_STATUS_OK);
    UmiModelRequestBinding replacementBinding;
    CHECK(UmiModelSessionBindRequest(second, "fixture.request", &replacementBinding) == UMI_STATUS_OK);
    CHECK(replacementBinding.session_identity != originalBinding.session_identity);
    CHECK(UmiModelSessionCheckBinding(second, &originalBinding, "fixture.request") == UMI_STATUS_INVALID_STATE);
    CHECK(UmiModelSessionCheckBinding(second, &replacementBinding, "fixture.request") == UMI_STATUS_OK);
    CHECK(UmiModelSessionStop(&second) == UMI_STATUS_OK && second == NULL);
    return 0;
}
