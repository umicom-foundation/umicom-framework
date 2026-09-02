/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/explorer_session.c
 *
 * PURPOSE:
 *   Implement the Framework-owned Test Explorer composition root. Applications
 *   bind the service to menus and views; they do not recreate provider,
 *   analytics, quality-gate or rerun orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/explorer_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiTestExplorerSession {
    UmiTestPlatformService *service;
    UmiTestWorkspace *workspace;
    UmiTestPlatformProviderRegistry *providers;
    UmiTestExplorerModel *model;
    UmiTestRerunPlan *rerun;
    UmiTestCoverageAnalysis *coverage;
    UmiTestBenchmarkAnalysis *benchmarks;
    UmiTestHistoryAnalysis *history;
    UmiTestCoveragePolicy coverage_policy;
    UmiTestCoverageBaseline coverage_baseline;
    UmiTestBenchmarkPolicy benchmark_policy;
    UmiTestHistoryPolicy history_policy;
    UmiTestRerunPolicy rerun_policy;
    UmiTestExplorerSessionState state;
    uint64_t refresh_generation;
    uint64_t revision;
    int owns_service;
    int owns_workspace;
};

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the register builtin provider operation used by this module and its client
 * applications.
 */
static UmiStatus register_builtin_provider(
    UmiTestPlatformProviderRegistry *registry,
    const char *id,
    const char *label,
    const char *frameworks,
    UmiTestPlatformProviderKind kind,
    uint32_t capabilities,
    int32_t priority)
{
    UmiTestPlatformProviderDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_TEST_PLATFORM_PROVIDER_API_VERSION;
    copy_text(descriptor.id, sizeof(descriptor.id), id);
    copy_text(descriptor.label, sizeof(descriptor.label), label);
    copy_text(descriptor.frameworks, sizeof(descriptor.frameworks), frameworks);
    descriptor.kind = kind;
    descriptor.capabilities = capabilities;
    descriptor.priority = priority;
    descriptor.enabled = 1;
    descriptor.trusted = 1;
    descriptor.available = 1;
    descriptor.built_in = 1;
    return umi_test_platform_provider_registry_upsert(registry, &descriptor);
}

/*
 * Provide the register builtin providers operation used by this module and its client
 * applications.
 */
static UmiStatus register_builtin_providers(UmiTestExplorerSession *session)
{
    UmiStatus status;
    const uint32_t common =
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DISCOVERY |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_EXECUTION |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_STRUCTURED_REPORTS |
        (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_RETRY;

    status = register_builtin_provider(
        session->providers, "umicom.test.ctest", "CTest",
        "ctest;cmake;c;c++;assembly", UMI_TEST_PLATFORM_PROVIDER_CTEST,
        common |
            (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_COVERAGE |
            (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_BENCHMARK |
            (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_SHARDING,
        100);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = register_builtin_provider(
        session->providers, "umicom.test.native", "Native Test Provider",
        "native;c;c++;assembly;*", UMI_TEST_PLATFORM_PROVIDER_NATIVE,
        common |
            (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DEBUG |
            (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_ATTACHMENTS |
            (uint32_t)UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_CONTINUOUS_TESTING,
        90);
    return status;
}

/*
 * Provide the allocate components operation used by this module and its client
 * applications.
 */
static UmiStatus allocate_components(UmiTestExplorerSession *session)
{
    UmiStatus status;
    status = umi_test_platform_provider_registry_create(&session->providers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_explorer_model_create(&session->model);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_test_rerun_plan_create(&session->rerun);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_coverage_analysis_create(&session->coverage);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_benchmark_analysis_create(&session->benchmarks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_history_analysis_create(&session->history);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = register_builtin_providers(session);
    return status;
}

/*
 * Provide the initialise policies operation used by this module and its client
 * applications.
 */
static void initialise_policies(UmiTestExplorerSession *session)
{
    umi_test_coverage_policy_init(&session->coverage_policy);
    umi_test_coverage_baseline_init(&session->coverage_baseline);
    umi_test_benchmark_policy_init(&session->benchmark_policy);
    umi_test_history_policy_init(&session->history_policy);
    umi_test_rerun_policy_init(&session->rerun_policy);
}

/*
 * Provide the test explorer session create bound operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_create_bound(
    UmiTestPlatformService *service,
    UmiTestWorkspace *workspace,
    UmiTestExplorerSession **out_session)
{
    UmiTestExplorerSession *session;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || workspace == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiTestExplorerSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->service = service;
    session->workspace = workspace;
    initialise_policies(session);
    status = allocate_components(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_test_explorer_session_destroy(session);
        return status;
    }
    session->state = UMI_TEST_EXPLORER_SESSION_READY;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Initialise test explorer session from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_explorer_session_create(
    UmiTestExplorerSession **out_session)
{
    UmiTestExplorerSession *session;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiTestExplorerSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    initialise_policies(session);
    status = umi_test_platform_service_create(&session->service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->owns_service = 1;
        status = umi_test_workspace_create(session->service, &session->workspace);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) session->owns_workspace = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = allocate_components(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_test_explorer_session_destroy(session);
        return status;
    }
    session->state = UMI_TEST_EXPLORER_SESSION_READY;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by test explorer session so the same storage can be reused
 * safely.
 */
void umi_test_explorer_session_destroy(UmiTestExplorerSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_test_history_analysis_destroy(session->history);
    umi_test_benchmark_analysis_destroy(session->benchmarks);
    umi_test_coverage_analysis_destroy(session->coverage);
    umi_test_rerun_plan_destroy(session->rerun);
    umi_test_explorer_model_destroy(session->model);
    umi_test_platform_provider_registry_destroy(session->providers);
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->owns_workspace) umi_test_workspace_destroy(session->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->owns_service) umi_test_platform_service_destroy(session->service);
    free(session);
}

/*
 * Provide the test explorer session refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_refresh(
    UmiTestExplorerSession *session,
    const char *coverage_session_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->service == NULL || session->workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state == UMI_TEST_EXPLORER_SESSION_RUNNING ||
        session->state == UMI_TEST_EXPLORER_SESSION_STOPPING) {
        return UMI_STATUS_BUSY;
    }
    status = umi_test_workspace_refresh(session->workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_explorer_model_rebuild(
            session->model,
            umi_test_platform_service_item(session->service),
            umi_test_platform_service_result(session->service));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_history_analysis_build(
            session->history,
            umi_test_platform_service_item(session->service),
            umi_test_platform_service_result(session->service),
            &session->history_policy);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_coverage_analysis_build(
            session->coverage,
            umi_test_platform_service_coverage(session->service),
            coverage_session_id, &session->coverage_policy,
            &session->coverage_baseline);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_benchmark_analysis_build(
            session->benchmarks,
            umi_test_platform_service_benchmark(session->service),
            &session->benchmark_policy);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_rerun_plan_build(
            session->rerun,
            umi_test_platform_service_item(session->service),
            umi_test_platform_service_result(session->service),
            &session->rerun_policy);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->state = UMI_TEST_EXPLORER_SESSION_READY;
        ++session->refresh_generation;
        ++session->revision;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->state = UMI_TEST_EXPLORER_SESSION_FAILED;
        ++session->revision;
    }
    return status;
}

/*
 * Provide the test explorer session set filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_set_filter(
    UmiTestExplorerSession *session,
    const UmiTestPlatformFilter *filter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_explorer_model_set_filter(session->model, filter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_explorer_model_rebuild(
            session->model,
            umi_test_platform_service_item(session->service),
            umi_test_platform_service_result(session->service));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++session->revision;
    return status;
}

/*
 * Provide the test explorer session set sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_set_sort(
    UmiTestExplorerSession *session,
    UmiTestExplorerSortMode sort_mode)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_explorer_model_set_sort(session->model, sort_mode);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_explorer_model_rebuild(
            session->model,
            umi_test_platform_service_item(session->service),
            umi_test_platform_service_result(session->service));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++session->revision;
    return status;
}

/*
 * Provide the test explorer session set group operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_set_group(
    UmiTestExplorerSession *session,
    UmiTestExplorerGroupMode group_mode)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_explorer_model_set_group(session->model, group_mode);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_explorer_model_rebuild(
            session->model,
            umi_test_platform_service_item(session->service),
            umi_test_platform_service_result(session->service));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++session->revision;
    return status;
}

/*
 * Provide the test explorer session select operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_select(
    UmiTestExplorerSession *session,
    const char *item_id,
    int selected,
    int exclusive)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_explorer_model_select(session->model, item_id, selected,
                                            exclusive);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++session->revision;
    return status;
}

/*
 * Provide the test explorer session build rerun operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_build_rerun(
    UmiTestExplorerSession *session,
    const UmiTestRerunPolicy *policy)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session->rerun_policy = *policy;
    status = umi_test_rerun_plan_build(
        session->rerun, umi_test_platform_service_item(session->service),
        umi_test_platform_service_result(session->service), policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++session->revision;
    return status;
}

/*
 * Provide the test explorer session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_begin(
    UmiTestExplorerSession *session,
    const UmiTestPlatformOperationPlan *plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state == UMI_TEST_EXPLORER_SESSION_RUNNING ||
        session->state == UMI_TEST_EXPLORER_SESSION_STOPPING) {
        return UMI_STATUS_BUSY;
    }
    status = umi_test_workspace_begin(session->workspace, plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->state = UMI_TEST_EXPLORER_SESSION_RUNNING;
        ++session->revision;
    }
    return status;
}

/*
 * Provide the test explorer session request stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_explorer_session_request_stop(
    UmiTestExplorerSession *session)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_TEST_EXPLORER_SESSION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_test_workspace_request_stop(session->workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->state = UMI_TEST_EXPLORER_SESSION_STOPPING;
        ++session->revision;
    }
    return status;
}

/*
 * Provide the test explorer session finish operation used by this module and its client
 * applications.
 */
void umi_test_explorer_session_finish(
    UmiTestExplorerSession *session,
    int cancelled,
    int failed)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_test_workspace_finish(session->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (cancelled) session->state = UMI_TEST_EXPLORER_SESSION_CANCELLED;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (failed) session->state = UMI_TEST_EXPLORER_SESSION_FAILED;
    /* Use this fallback path when the earlier condition does not apply. */
    else session->state = UMI_TEST_EXPLORER_SESSION_COMPLETED;
    ++session->revision;
}

/*
 * Provide the test explorer session snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_explorer_session_snapshot(
    UmiTestExplorerSession *session,
    UmiTestExplorerSessionSnapshot *out_snapshot)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TEST_EXPLORER_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    status = umi_test_platform_service_snapshot(session->service,
                                                 &out_snapshot->service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_workspace_snapshot(session->workspace,
                                             &out_snapshot->workspace);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_explorer_model_snapshot(session->model,
                                                  &out_snapshot->explorer);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_rerun_plan_snapshot(session->rerun,
                                              &out_snapshot->rerun);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_coverage_analysis_snapshot(session->coverage,
                                                     &out_snapshot->coverage);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_benchmark_analysis_snapshot(session->benchmarks,
                                                      &out_snapshot->benchmarks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_test_history_analysis_snapshot(session->history,
                                                    &out_snapshot->history);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->provider_count =
        umi_test_platform_provider_registry_count(session->providers);
    out_snapshot->refresh_generation = session->refresh_generation;
    out_snapshot->revision = session->revision;
    out_snapshot->owns_service = session->owns_service;
    out_snapshot->owns_workspace = session->owns_workspace;
    out_snapshot->quality_gate_passed =
        out_snapshot->coverage.gate_state != UMI_TEST_COVERAGE_GATE_FAILED &&
        out_snapshot->benchmarks.regressed_metric_count == 0U;
    out_snapshot->can_refresh =
        session->state != UMI_TEST_EXPLORER_SESSION_RUNNING &&
        session->state != UMI_TEST_EXPLORER_SESSION_STOPPING;
    out_snapshot->can_run = out_snapshot->can_refresh &&
                            out_snapshot->service.item_count > 0U;
    out_snapshot->can_stop =
        session->state == UMI_TEST_EXPLORER_SESSION_RUNNING;
    return UMI_STATUS_OK;
}

/*
 * Provide the test explorer session providers operation used by this module and its client
 * applications.
 */
UmiTestPlatformProviderRegistry *umi_test_explorer_session_providers(
    UmiTestExplorerSession *session)
{
    return session != NULL ? session->providers : NULL;
}

/*
 * Provide the test explorer session model operation used by this module and its client
 * applications.
 */
UmiTestExplorerModel *umi_test_explorer_session_model(
    UmiTestExplorerSession *session)
{
    return session != NULL ? session->model : NULL;
}

/*
 * Provide the test explorer session rerun operation used by this module and its client
 * applications.
 */
UmiTestRerunPlan *umi_test_explorer_session_rerun(
    UmiTestExplorerSession *session)
{
    return session != NULL ? session->rerun : NULL;
}

/*
 * Provide the test explorer session coverage operation used by this module and its client
 * applications.
 */
UmiTestCoverageAnalysis *umi_test_explorer_session_coverage(
    UmiTestExplorerSession *session)
{
    return session != NULL ? session->coverage : NULL;
}

/*
 * Provide the test explorer session benchmarks operation used by this module and its
 * client applications.
 */
UmiTestBenchmarkAnalysis *umi_test_explorer_session_benchmarks(
    UmiTestExplorerSession *session)
{
    return session != NULL ? session->benchmarks : NULL;
}

/*
 * Provide the test explorer session history operation used by this module and its client
 * applications.
 */
UmiTestHistoryAnalysis *umi_test_explorer_session_history(
    UmiTestExplorerSession *session)
{
    return session != NULL ? session->history : NULL;
}

/*
 * Provide the test explorer session state text operation used by this module and its
 * client applications.
 */
const char *umi_test_explorer_session_state_text(
    UmiTestExplorerSessionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_TEST_EXPLORER_SESSION_READY: return "Ready";
        case UMI_TEST_EXPLORER_SESSION_DISCOVERING: return "Discovering";
        case UMI_TEST_EXPLORER_SESSION_RUNNING: return "Running";
        case UMI_TEST_EXPLORER_SESSION_STOPPING: return "Stopping";
        case UMI_TEST_EXPLORER_SESSION_COMPLETED: return "Completed";
        case UMI_TEST_EXPLORER_SESSION_CANCELLED: return "Cancelled";
        case UMI_TEST_EXPLORER_SESSION_FAILED: return "Failed";
        case UMI_TEST_EXPLORER_SESSION_EMPTY:
        default: return "Empty";
    }
}
