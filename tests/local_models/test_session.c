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
#include "../model_formats/gguf_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "fixture.gguf");
    /* The former header-only fixture is retained for review. Session admission
     * now requires role metadata, so the replacement declares a candidate but
     * still supplies no real model weights; the finite child ignores the file. */
#if 0
    unsigned char bytes[128] = {0}; memcpy(bytes, "GGUF", 4U); bytes[4] = 3U; bytes[8] = 1U;
#endif
    GgufFixture metadata;fixture_model(&metadata);
    if (strcmp(argv[1], "auxiliary") == 0) {
        fixture_start(&metadata);fixture_text(&metadata,"general.architecture","controlvector");
    } else if (strcmp(argv[1], "adapter") == 0) fixture_text(&metadata,"general.type","adapter");
    else if (strcmp(argv[1], "unknown-role") == 0) fixture_start(&metadata);
    else if (strcmp(argv[1], "vocabulary") == 0) fixture_number_at(metadata.bytes+8U,0U,8U);
    unsigned char bytes[256] = {0};
    CHECK(metadata.size<=sizeof(bytes));memcpy(bytes,metadata.bytes,metadata.size);
    char digest[65]; CHECK(UmiSha256Buffer(bytes, sizeof(bytes), digest) == UMI_STATUS_OK);
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, sizeof(bytes)) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
    UmiModelRunPlan plan;
    CHECK(UmiModelRunPlanCreate(argv[2], path, digest, "fixture", 18080U, 512U, 1U, &plan) == UMI_STATUS_OK);
    UmiModelSession *session = NULL;
    if (strcmp(argv[1], "inspection-unavailable") == 0) {
        CHECK(!UmiModelRunInspectionAvailable());
        CHECK(UmiModelSessionStart(&plan,true,&session)==UMI_STATUS_NOT_IMPLEMENTED && session==NULL);
        CHECK(UmiModelSessionStart(&plan,true,&session)==UMI_STATUS_NOT_IMPLEMENTED && session==NULL);
        return 0;
    }
    if (strcmp(argv[1], "auxiliary") == 0 || strcmp(argv[1], "adapter") == 0 ||
        strcmp(argv[1], "unknown-role") == 0 || strcmp(argv[1], "vocabulary") == 0) {
        UmiStatus wanted=strcmp(argv[1],"vocabulary")==0?UMI_STATUS_PARSE_ERROR:UMI_STATUS_NOT_IMPLEMENTED;
        CHECK(UmiModelSessionStart(&plan,true,&session)==wanted && session==NULL);
        /* A second refusal, rather than BUSY, verifies lease cleanup. */
        CHECK(UmiModelSessionStart(&plan,true,&session)==wanted && session==NULL);
        return 0;
    }
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
