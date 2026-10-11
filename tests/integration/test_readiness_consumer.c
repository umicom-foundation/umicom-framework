/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration/test_readiness_consumer.c
 * PURPOSE: Verify readiness lifecycle symbols through the public integration library alone.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/integration/readiness_gate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A single allocation keeps the suite and registry off the small default
 * Windows stack. The test owns these values until it destroys the borrowing
 * gate; no process, authenticated peer or secret store is opened here. */
typedef struct ReadinessFixture
{
    UmiIntegrationRegistry registry;
    UmiIntegrationApplication application;
    UmiIntegrationSuiteDefinition suite;
    UmiIntegrationSuiteRuntime runtime;
} ReadinessFixture;

#define REQUIRE(expression)                                                                        \
    do                                                                                             \
    {                                                                                              \
        if (!(expression))                                                                         \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression);                       \
            goto finish;                                                                           \
        }                                                                                          \
    } while (0)

int main(void)
{
    ReadinessFixture *fixture = calloc(1U, sizeof(*fixture));
    UmiIntegrationReadinessGate *gate = NULL;
    UmiIntegrationReadinessSnapshot snapshot = {0};
    uint8_t challenge[UMI_INTEGRATION_READINESS_CHALLENGE_BYTES] = {0};
    int result = EXIT_FAILURE;

    REQUIRE(fixture != NULL);
    umi_integration_registry_init(&fixture->registry);
    umi_integration_application_init(&fixture->application);
    REQUIRE(umi_integration_application_set_identity(&fixture->application, "studio", "Studio") ==
            UMI_STATUS_OK);
    fixture->application.enabled = true;
    REQUIRE(umi_integration_registry_register(&fixture->registry, &fixture->application) ==
            UMI_STATUS_OK);
    umi_integration_suite_init(&fixture->suite, "development", "Development");
    REQUIRE(umi_integration_suite_add_member(&fixture->suite, "studio",
                                             UMI_INTEGRATION_DEPENDENCY_REQUIRED,
                                             0U) == UMI_STATUS_OK);
    REQUIRE(umi_integration_suite_runtime_prepare(&fixture->runtime, &fixture->suite,
                                                  &fixture->registry) == UMI_STATUS_OK);
    REQUIRE(UmiIntegrationReadinessGateCreate(&fixture->runtime, &gate) == UMI_STATUS_OK);
    REQUIRE(!UmiIntegrationReadinessGateIsReady(gate));

    /* Deterministic bytes are suitable only for this isolated test. A real
     * supervisor supplies fresh operating-system randomness and verifies the
     * transport peer before allowing a confirmation into the gate. */
    for (size_t index = 0U; index < sizeof(challenge); ++index)
        challenge[index] = (uint8_t)(index + 1U);
    REQUIRE(UmiIntegrationReadinessGateArm(gate, "studio", 41U, 1U, challenge, 100U, 500U) ==
            UMI_STATUS_OK);
    REQUIRE(UmiIntegrationReadinessGateSnapshot(gate, "studio", &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.pending && !snapshot.confirmed && snapshot.expiresAtMs == 600U);
    REQUIRE(UmiIntegrationReadinessGateConfirm(gate, "studio", 41U, 1U, challenge, 101U, false) ==
            UMI_STATUS_PERMISSION_DENIED);
    REQUIRE(!UmiIntegrationReadinessGateIsReady(gate));
    REQUIRE(UmiIntegrationReadinessGateConfirm(gate, "studio", 41U, 1U, challenge, 101U, true) ==
            UMI_STATUS_OK);
    REQUIRE(UmiIntegrationReadinessGateIsReady(gate));
    REQUIRE(UmiIntegrationReadinessGateSnapshot(gate, "studio", &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.structureSize == sizeof(snapshot) && snapshot.confirmed && !snapshot.pending);
    REQUIRE(strcmp(snapshot.applicationId, "studio") == 0 && snapshot.processToken == 41U &&
            snapshot.processGeneration == 1U);
    REQUIRE(UmiIntegrationReadinessGateExit(gate, "studio", 41U, 1U, 0) == UMI_STATUS_OK);
    REQUIRE(!UmiIntegrationReadinessGateIsReady(gate));
    result = EXIT_SUCCESS;

finish:
    UmiIntegrationReadinessGateDestroy(gate);
    free(fixture);
    return result;
}
