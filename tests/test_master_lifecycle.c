/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_master_lifecycle.c
 *
 * PURPOSE:
 *   Verify dependency-preserving shutdown, rollback retries and callback reentry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/runtime/master_controller.h"
#include <assert.h>
#include <string.h>

enum Phase { CONFIGURE, INITIALISE, START, QUIESCE, STOP, DESTROY, PHASE_COUNT };
typedef struct Fixture Fixture;
typedef struct ModuleState {
    Fixture *fixture;
    unsigned index;
    unsigned calls[PHASE_COUNT];
} ModuleState;
struct Fixture {
    UmiMasterController *master;
    UmiModuleDescriptor modules[3];
    ModuleState state[3];
    int fault_module;
    int fault_phase;
    unsigned failures;
    int rollback_stop_failure;
    int check_reentry;
    unsigned stop_order[16];
    unsigned stops;
    unsigned destroy_order[8];
    unsigned destroys;
    unsigned authorities_destroyed;
};
static const char *ids[] = {
    "org.umicom.test.provider", "org.umicom.test.consumer", "org.umicom.test.leaf"
};
static const char *provider_caps[] = {"test.lifecycle.provider", NULL};
static const char *consumer_caps[] = {"test.lifecycle.consumer", NULL};
static const char *consumer_needs[] = {"test.lifecycle.provider", NULL};
static const char *leaf_needs[] = {"test.lifecycle.consumer", NULL};

static void UnusedAuthorityDestroy(void *state)
{
    (void)state;
    assert(!"Reentrant authority registration must be refused");
}

static void CheckReentry(Fixture *fixture)
{
    UmiMasterController *borrowed = fixture->master;
    assert(UmiMasterControllerRelease(&borrowed) == UMI_STATUS_INVALID_STATE);
    assert(borrowed == fixture->master);
    assert(umi_master_controller_start(borrowed) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_stop(borrowed) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_validate(borrowed, NULL) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_register(borrowed, &fixture->modules[0])
           == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_register_authority(borrowed,
        "org.umicom.test.reentry", fixture, UnusedAuthorityDestroy)
        == UMI_STATUS_INVALID_STATE);
    umi_master_controller_destroy(borrowed);
    assert(umi_master_controller_module_count(borrowed) == 3U);
}

static void DestroyAuthority(void *data)
{
    Fixture *fixture = data;
    assert(fixture->destroys == 3U);
    ++fixture->authorities_destroyed;
    if (fixture->check_reentry) CheckReentry(fixture);
}

static UmiStatus PhaseCall(UmiModuleContext *context, enum Phase phase)
{
    ModuleState *state = context->module_state;
    Fixture *fixture = state->fixture;
    ++state->calls[phase];
    if (fixture->check_reentry) CheckReentry(fixture);
    if (phase == STOP) {
        assert(fixture->stops < 16U);
        fixture->stop_order[fixture->stops++] = state->index;
        if (fixture->rollback_stop_failure && state->index == 1U) {
            fixture->rollback_stop_failure = 0;
            return UMI_STATUS_IO_ERROR;
        }
    }
    if (fixture->failures != 0U && fixture->fault_module == (int)state->index &&
        fixture->fault_phase == (int)phase) {
        --fixture->failures;
        return phase == STOP || phase == QUIESCE
            ? UMI_STATUS_IO_ERROR : UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
static UmiStatus Configure(UmiModuleContext *c) { return PhaseCall(c, CONFIGURE); }
static UmiStatus Initialise(UmiModuleContext *c) { return PhaseCall(c, INITIALISE); }
static UmiStatus Start(UmiModuleContext *c) { return PhaseCall(c, START); }
static UmiStatus Quiesce(UmiModuleContext *c) { return PhaseCall(c, QUIESCE); }
static UmiStatus Stop(UmiModuleContext *c) { return PhaseCall(c, STOP); }
static void Destroy(UmiModuleContext *context)
{
    ModuleState *state = context->module_state;
    Fixture *fixture = state->fixture;
    ++state->calls[DESTROY];
    if (fixture->check_reentry) CheckReentry(fixture);
    assert(fixture->destroys < 8U);
    fixture->destroy_order[fixture->destroys++] = state->index;
}

static void Create(Fixture *fixture)
{
    memset(fixture, 0, sizeof(*fixture));
    fixture->fault_module = -1;
    fixture->fault_phase = -1;
    UmiMasterControllerConfig config = {"Lifecycle fixture", NULL, NULL};
    assert(umi_master_controller_create(&config, &fixture->master) == UMI_STATUS_OK);
    for (unsigned index = 0U; index < 3U; ++index) {
        ModuleState *state = &fixture->state[index];
        state->fixture = fixture;
        state->index = index;
        UmiModuleDescriptor *module = &fixture->modules[index];
        module->structure_size = (uint32_t)sizeof(*module);
        module->abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
        module->module_id = ids[index];
        module->display_name = ids[index];
        module->module_version = (UmiVersion){1U, 0U, 0U};
        module->kind = UMI_MODULE_SERVICE;
        module->module_state = state;
        module->lifecycle = (UmiModuleLifecycle){
            Configure, Initialise, Start, Quiesce, Stop, Destroy
        };
    }
    fixture->modules[0].provided_capabilities = provider_caps;
    fixture->modules[1].provided_capabilities = consumer_caps;
    fixture->modules[1].required_capabilities = consumer_needs;
    fixture->modules[2].required_capabilities = leaf_needs;
    assert(umi_master_controller_register_authority(fixture->master,
        "org.umicom.test.owned", fixture, DestroyAuthority) == UMI_STATUS_OK);
    /* Deliberately register opposite to dependency order. */
    for (unsigned index = 3U; index > 0U; --index)
        assert(umi_master_controller_register(fixture->master,
            &fixture->modules[index - 1U]) == UMI_STATUS_OK);
}

static void Release(Fixture *fixture)
{
    assert(UmiMasterControllerRelease(&fixture->master) == UMI_STATUS_OK);
    assert(fixture->master == NULL);
    assert(fixture->destroys == 3U);
    assert(fixture->authorities_destroyed == 1U);
    assert(fixture->destroy_order[0] == 2U);
    assert(fixture->destroy_order[1] == 1U);
    assert(fixture->destroy_order[2] == 0U);
    for (unsigned index = 0U; index < 3U; ++index)
        assert(fixture->state[index].calls[DESTROY] == 1U);
}

static void CheckSuccessfulCycles(int reentry)
{
    Fixture fixture;
    Create(&fixture);
    fixture.check_reentry = reentry;
    for (unsigned cycle = 0U; cycle < 2U; ++cycle) {
        assert(umi_master_controller_start(fixture.master) == UMI_STATUS_OK);
        assert(UmiMasterControllerNeedsStop(fixture.master));
        assert(umi_master_controller_stop(fixture.master) == UMI_STATUS_OK);
        assert(!UmiMasterControllerNeedsStop(fixture.master));
        assert(umi_master_controller_stop(fixture.master) == UMI_STATUS_INVALID_STATE);
    }
    assert(fixture.stops == 6U);
    for (unsigned index = 0U; index < fixture.stops; ++index)
        assert(fixture.stop_order[index] == 2U - index % 3U);
    Release(&fixture);
}

static void CheckStopRetry(enum Phase phase, int legacy)
{
    Fixture fixture;
    Create(&fixture);
    assert(umi_master_controller_start(fixture.master) == UMI_STATUS_OK);
    fixture.fault_module = 1;
    fixture.fault_phase = (int)phase;
    fixture.failures = 1U;
    if (legacy == 1) umi_master_controller_destroy(fixture.master);
    else if (legacy == 2) {
        UmiMasterController *original = fixture.master;
        assert(UmiMasterControllerRelease(&fixture.master) == UMI_STATUS_IO_ERROR);
        assert(fixture.master == original);
    } else assert(umi_master_controller_stop(fixture.master) == UMI_STATUS_IO_ERROR);
    assert(UmiMasterControllerNeedsStop(fixture.master));
    assert(fixture.destroys == 0U);
    assert(fixture.authorities_destroyed == 0U);
    assert(fixture.state[2].calls[STOP] == 1U);
    assert(fixture.state[0].calls[STOP] == 0U);
    assert(fixture.state[0].calls[QUIESCE] == 0U);
    assert(umi_master_controller_module_state(fixture.master, ids[0])
           == UMI_MODULE_STARTED);
    assert(umi_master_controller_module_state(fixture.master, ids[1])
           == UMI_MODULE_FAILED);
    assert(umi_master_controller_start(fixture.master) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_validate(fixture.master, NULL) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_register(fixture.master, &fixture.modules[0])
           == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_stop(fixture.master) == UMI_STATUS_OK);
    assert(fixture.state[2].calls[STOP] == 1U);
    assert(fixture.state[1].calls[STOP] == (phase == STOP ? 2U : 1U));
    assert(fixture.state[1].calls[QUIESCE] == (phase == QUIESCE ? 2U : 1U));
    assert(fixture.state[0].calls[STOP] == 1U);
    Release(&fixture);
}

static void CheckRollback(enum Phase phase, int cleanup_failure)
{
    Fixture fixture;
    Create(&fixture);
    fixture.fault_module = 1;
    fixture.fault_phase = (int)phase;
    fixture.failures = 1U;
    fixture.rollback_stop_failure = cleanup_failure;
    assert(umi_master_controller_start(fixture.master) == UMI_STATUS_PARSE_ERROR);
    assert(fixture.state[2].calls[CONFIGURE] == 0U);
    if (cleanup_failure) {
        assert(UmiMasterControllerNeedsStop(fixture.master));
        assert(fixture.state[0].calls[STOP] == 0U);
        assert(fixture.state[1].calls[STOP] == 1U);
        assert(umi_master_controller_start(fixture.master) == UMI_STATUS_INVALID_STATE);
        assert(umi_master_controller_stop(fixture.master) == UMI_STATUS_OK);
        assert(fixture.state[1].calls[STOP] == 2U);
    } else {
        assert(!UmiMasterControllerNeedsStop(fixture.master));
        assert(fixture.state[1].calls[STOP] == (phase == CONFIGURE ? 0U : 1U));
    }
    assert(fixture.state[0].calls[STOP] == 1U);
    assert(fixture.state[1].calls[QUIESCE] == 0U);
    Release(&fixture);
}


#ifdef UMICOM_MASTER_RELEASE_FAULTS
static int fail_service_release;
UmiStatus __real_UmiServiceRegistryRelease(UmiServiceRegistry **);
UmiStatus __wrap_UmiServiceRegistryRelease(UmiServiceRegistry **registry)
{
    if (fail_service_release) {
        fail_service_release = 0;
        return UMI_STATUS_IO_ERROR;
    }
    return __real_UmiServiceRegistryRelease(registry);
}

static void CheckChildReleaseRetry(void)
{
    Fixture fixture;
    Create(&fixture);
    assert(umi_master_controller_start(fixture.master) == UMI_STATUS_OK);
    UmiMasterController *original = fixture.master;
    fail_service_release = 1;
    assert(UmiMasterControllerRelease(&fixture.master) == UMI_STATUS_IO_ERROR);
    assert(fixture.master == original && fail_service_release == 0);
    assert(fixture.destroys == 3U && fixture.authorities_destroyed == 1U);
    /* Modules are already destroyed. Only completing release is now valid. */
    assert(umi_master_controller_start(fixture.master) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_stop(fixture.master) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_validate(fixture.master, NULL) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_register(fixture.master, &fixture.modules[0])
           == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_register_authority(fixture.master,
        "example.after-release", &fixture, UnusedAuthorityDestroy) == UMI_STATUS_INVALID_STATE);
    assert(!UmiMasterControllerNeedsStop(fixture.master));
    Release(&fixture);
}
#endif

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "empty") == 0) {
        UmiMasterController *master = NULL;
        assert(UmiMasterControllerRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiMasterControllerRelease(&master) == UMI_STATUS_OK);
        assert(!UmiMasterControllerNeedsStop(NULL));
        UmiMasterControllerConfig config = {"Empty lifecycle", NULL, NULL};
        assert(umi_master_controller_create(&config, &master) == UMI_STATUS_OK);
        assert(umi_master_controller_start(master) == UMI_STATUS_OK);
        assert(UmiMasterControllerRelease(&master) == UMI_STATUS_OK);
        assert(master == NULL);
    } else if (strcmp(argv[1], "cycles") == 0) CheckSuccessfulCycles(0);
    else if (strcmp(argv[1], "callback-reentry") == 0) CheckSuccessfulCycles(1);
    else if (strcmp(argv[1], "stop-retry") == 0) CheckStopRetry(STOP, 0);
    else if (strcmp(argv[1], "quiesce-retry") == 0) CheckStopRetry(QUIESCE, 0);
    else if (strcmp(argv[1], "legacy-retain") == 0) CheckStopRetry(STOP, 1);
    else if (strcmp(argv[1], "release-retry") == 0) CheckStopRetry(STOP, 2);
    else if (strcmp(argv[1], "rollback-configure") == 0) CheckRollback(CONFIGURE, 0);
    else if (strcmp(argv[1], "rollback-initialise") == 0) CheckRollback(INITIALISE, 0);
    else if (strcmp(argv[1], "rollback-start") == 0) CheckRollback(START, 0);
    else if (strcmp(argv[1], "rollback-stop-retry") == 0) CheckRollback(START, 1);

#ifdef UMICOM_MASTER_RELEASE_FAULTS
    else if (strcmp(argv[1], "release-child-retry") == 0) CheckChildReleaseRetry();
#endif
    else assert(!"Unknown master lifecycle scenario");
    return 0;
}
