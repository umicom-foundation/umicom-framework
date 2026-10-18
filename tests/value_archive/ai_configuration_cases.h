/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/ai_configuration_cases.h
 * PURPOSE: Verify AI metadata configuration preserves borrowed text and rejected values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_AI_CONFIGURATION_CASES_H
#define UMICOM_TEST_AI_CONFIGURATION_CASES_H
#include "construction_cases.h"
#include "umicom/base/status.h"

/* These owners contain inline metadata only. Exercise aliases through the
 * public entry point; a separate source buffer would miss early clearing of
 * the destination. Check rejected requests against the complete prior value. */
#define UMI_TEST_AI_CONFIGURATION(Function, Type, Configure, Validate) \
static int Function(void) \
{ \
    int result = 1; \
    Type *value = malloc(sizeof(*value)); \
    Type *before = malloc(sizeof(*before)); \
    char *long_id = malloc(sizeof(value->id) + 17U); \
    UMI_CONSTRUCTION_REQUIRE(value != NULL && before != NULL && long_id != NULL); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, "goal-original", "Original label", 12U, 3U) == UMI_STATUS_OK); \
    memcpy(before, value, sizeof(*before)); \
    UMI_CONSTRUCTION_REQUIRE(Configure(NULL, "goal", "Label", 1U, 0U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, NULL, "Label", 1U, 0U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*before)) == 0); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, "", "Label", 1U, 0U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*before)) == 0); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, value->id, value->label, 7U, 11U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "goal-original") == 0); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->label, "Original label") == 0); \
    UMI_CONSTRUCTION_REQUIRE(value->priority == 7U && value->flags == 11U && \
        value->revision == 1U && value->enabled == 1); \
    UMI_CONSTRUCTION_REQUIRE(Validate(value) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, "prefix-goal", "prefix-label", 2U, 1U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, value->id + 7U, value->label + 7U, 3U, 2U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "goal") == 0 && strcmp(value->label, "label") == 0); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, value->label, value->id, 4U, 3U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "label") == 0 && strcmp(value->label, "goal") == 0); \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, value->id, NULL, 5U, 4U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "label") == 0 && value->label[0] == '\0'); \
    memset(long_id, 'x', sizeof(value->id) + 16U); \
    long_id[sizeof(value->id) + 16U] = '\0'; \
    UMI_CONSTRUCTION_REQUIRE(Configure(value, long_id, "Bounded label", 6U, 5U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(value->id[sizeof(value->id) - 1U] == '\0'); \
    UMI_CONSTRUCTION_REQUIRE(strlen(value->id) == sizeof(value->id) - 1U); \
    UMI_CONSTRUCTION_REQUIRE(Validate(value) == UMI_STATUS_OK); \
    result = 0; \
construction_cleanup: \
    free(long_id); \
    free(before); \
    free(value); \
    return result; \
}
#endif
