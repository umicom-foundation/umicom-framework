/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/service_composition/main.c
 *
 * PURPOSE:
 *   Verify installed module admission, dependency order and retryable cleanup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "composition.h"
#include <stdio.h>
#include <string.h>

static UmiStatus Expect(int condition, const char *description)
{
    if (condition) return UMI_STATUS_OK;
    fprintf(stderr, "Composition check failed: %s\n", description);
    return UMI_STATUS_INTERNAL_ERROR;
}

/* These checks remain active in release builds. They do not depend on assert
 * or on private Framework structures, so an installed SDK is the only input. */
#define REQUIRE(condition, description) do { \
    UmiStatus check_status = Expect((condition), (description)); \
    if (check_status != UMI_STATUS_OK) return check_status; \
} while (0)

static UmiStatus Run(ExampleComposition *composition, const char *scenario)
{
    UmiStatus status = ExampleCompositionCreate(composition,
        strcmp(scenario, "corrected-registration") == 0);
    if (status != UMI_STATUS_OK) return status;
    composition->fail_start_once = strcmp(scenario, "startup-failure") == 0;
    composition->fail_stop_once = strcmp(scenario, "stop-retry") == 0;
    status = umi_master_controller_start(composition->master);
    if (strcmp(scenario, "startup-failure") == 0) {
        REQUIRE(status == UMI_STATUS_IO_ERROR, "startup preserves the failing phase status");
        REQUIRE(!UmiMasterControllerNeedsStop(composition->master),
            "successful rollback leaves no stop obligation");
        REQUIRE(composition->borrowed_counter == NULL && !composition->service_registered,
            "rollback detaches the dependent before destroying its provider");
        REQUIRE(composition->counters_created == 1U && composition->counters_destroyed == 1U,
            "rollback destroys the owned counter exactly once");
        /* A completely rolled-back composition can be started again. */
        status = umi_master_controller_start(composition->master);
    }
    if (status != UMI_STATUS_OK) return status;
    REQUIRE(composition->borrowed_counter != NULL &&
            composition->borrowed_counter->value == 1U && composition->consumer_running,
        "consumer receives a started provider despite reverse registration");
    UmiServiceSnapshot metadata;
    UmiServiceRegistry *registry = umi_master_controller_services(composition->master);
    const char *service_id = composition->provider.provided_capabilities[0];
    status = UmiServiceRegistryCopySnapshot(registry, service_id,
        composition->provider.module_id, &metadata);
    if (status != UMI_STATUS_OK) return status;
    REQUIRE(strcmp(metadata.service_id, service_id) == 0 &&
            strcmp(metadata.provider_module_id, composition->provider.module_id) == 0,
        "diagnostic metadata identifies the registered provider");
    status = umi_master_controller_stop(composition->master);
    if (strcmp(scenario, "stop-retry") == 0) {
        REQUIRE(status == UMI_STATUS_IO_ERROR, "first stop reports the injected error");
        REQUIRE(UmiMasterControllerNeedsStop(composition->master),
            "failed stop retains its obligation");
        REQUIRE(composition->consumer_running && composition->service_registered &&
                composition->borrowed_counter != NULL && composition->counters_destroyed == 0U,
            "provider survives while a dependent still uses it");
        REQUIRE(umi_master_controller_start(composition->master) == UMI_STATUS_INVALID_STATE,
            "restart cannot conceal an unfinished stop");
        status = umi_master_controller_stop(composition->master);
    }
    if (status != UMI_STATUS_OK) return status;
    REQUIRE(!composition->consumer_running && !composition->service_registered &&
            composition->borrowed_counter == NULL,
        "successful stop clears all application references");
    /* The copied strings remain valid after unregister destroys the counter.
     * They describe the earlier observation, not a currently live service. */
    REQUIRE(strcmp(metadata.service_id, service_id) == 0 &&
            metadata.service_version.major == 1U,
        "service metadata survives the object lifetime");
    REQUIRE(UmiServiceRegistryCopySnapshot(registry, service_id,
            composition->provider.module_id, &metadata) == UMI_STATUS_NOT_FOUND,
        "fresh discovery reports that the stopped provider was removed");
    if (strcmp(scenario, "restart") == 0) {
        status = umi_master_controller_start(composition->master);
        if (status != UMI_STATUS_OK) return status;
        REQUIRE(composition->borrowed_counter != NULL &&
                composition->borrowed_counter->value == 1U,
            "restart creates a fresh service instead of reusing released storage");
        status = umi_master_controller_stop(composition->master);
        if (status != UMI_STATUS_OK) return status;
    }
    REQUIRE(composition->counters_created == composition->counters_destroyed,
        "all successfully admitted counters have been destroyed");
    REQUIRE(composition->modules_destroyed == 0U,
        "ordinary stop does not destroy the reusable module descriptors");
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    const char *scenario = argc == 2 ? argv[1] : "normal";
    if (argc > 2 || (strcmp(scenario, "normal") != 0 &&
        strcmp(scenario, "corrected-registration") != 0 &&
        strcmp(scenario, "startup-failure") != 0 &&
        strcmp(scenario, "stop-retry") != 0 && strcmp(scenario, "restart") != 0)) {
        fprintf(stderr, "Usage: %s [normal|corrected-registration|startup-failure|stop-retry|restart]\n",
            argv[0]);
        return 2;
    }
    ExampleComposition composition = {0};
    UmiStatus status = Run(&composition, scenario);
    /* Always attempt checked release, including partial admission and startup
     * errors. The descriptors and callback payload remain alive until here. */
    UmiStatus release_status = UmiMasterControllerRelease(&composition.master);
    if (release_status != UMI_STATUS_OK || composition.master != NULL) {
        fprintf(stderr, "Controller retained after release error: %s\n",
            umi_status_text(release_status));
        return 1;
    }
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Service composition failed: %s\n", umi_status_text(status));
        return 1;
    }
    if (Expect(composition.modules_destroyed == 2U,
            "checked release destroys both modules exactly once") != UMI_STATUS_OK ||
        Expect(composition.counters_created == composition.counters_destroyed,
            "final release has balanced service ownership") != UMI_STATUS_OK) return 1;
    printf("Service composition passed: %s\n", scenario);
    return 0;
}
