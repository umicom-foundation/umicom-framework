/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/catalogue.c
 *
 * PURPOSE:
 *   Compose executable Studio and Trader user journeys from small auditable
 *   scenario records while resolving panels and layouts from the canonical
 *   Framework experience catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/journey/catalogue.h"
#include <string.h>

#define STEP(step_id_, title_, panel_id_, command_id_, evidence_, critical_, confirmation_) \
    { sizeof(UmiApplicationJourneyStep), step_id_, title_, panel_id_, command_id_, \
      evidence_, critical_, confirmation_ },

static const UmiApplicationJourneyStep STUDIO_FIRST_RUN_WORKSPACE_STEPS[] = {
#include "journeys/studio/first_run_workspace/01.inc"
#include "journeys/studio/first_run_workspace/02.inc"
#include "journeys/studio/first_run_workspace/03.inc"
#include "journeys/studio/first_run_workspace/04.inc"
#include "journeys/studio/first_run_workspace/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_EDIT_BUILD_TEST_STEPS[] = {
#include "journeys/studio/edit_build_test/01.inc"
#include "journeys/studio/edit_build_test/02.inc"
#include "journeys/studio/edit_build_test/03.inc"
#include "journeys/studio/edit_build_test/04.inc"
#include "journeys/studio/edit_build_test/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_DEBUG_SESSION_STEPS[] = {
#include "journeys/studio/debug_session/01.inc"
#include "journeys/studio/debug_session/02.inc"
#include "journeys/studio/debug_session/03.inc"
#include "journeys/studio/debug_session/04.inc"
#include "journeys/studio/debug_session/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_SOURCE_CONTROL_REVIEW_STEPS[] = {
#include "journeys/studio/source_control_review/01.inc"
#include "journeys/studio/source_control_review/02.inc"
#include "journeys/studio/source_control_review/03.inc"
#include "journeys/studio/source_control_review/04.inc"
#include "journeys/studio/source_control_review/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_VISUAL_DESIGNER_ROUND_TRIP_STEPS[] = {
#include "journeys/studio/visual_designer_round_trip/01.inc"
#include "journeys/studio/visual_designer_round_trip/02.inc"
#include "journeys/studio/visual_designer_round_trip/03.inc"
#include "journeys/studio/visual_designer_round_trip/04.inc"
#include "journeys/studio/visual_designer_round_trip/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_AI_ASSISTED_CHANGE_STEPS[] = {
#include "journeys/studio/ai_assisted_change/01.inc"
#include "journeys/studio/ai_assisted_change/02.inc"
#include "journeys/studio/ai_assisted_change/03.inc"
#include "journeys/studio/ai_assisted_change/04.inc"
#include "journeys/studio/ai_assisted_change/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_LAYOUT_RECOVERY_STEPS[] = {
#include "journeys/studio/layout_recovery/01.inc"
#include "journeys/studio/layout_recovery/02.inc"
#include "journeys/studio/layout_recovery/03.inc"
#include "journeys/studio/layout_recovery/04.inc"
#include "journeys/studio/layout_recovery/05.inc"
};
static const UmiApplicationJourneyStep STUDIO_PACKAGE_RELEASE_STEPS[] = {
#include "journeys/studio/package_release/01.inc"
#include "journeys/studio/package_release/02.inc"
#include "journeys/studio/package_release/03.inc"
#include "journeys/studio/package_release/04.inc"
#include "journeys/studio/package_release/05.inc"
};
static const UmiApplicationJourneyStep TRADER_SIMULATION_STARTUP_STEPS[] = {
#include "journeys/trader/simulation_startup/01.inc"
#include "journeys/trader/simulation_startup/02.inc"
#include "journeys/trader/simulation_startup/03.inc"
#include "journeys/trader/simulation_startup/04.inc"
#include "journeys/trader/simulation_startup/05.inc"
};
static const UmiApplicationJourneyStep TRADER_MARKET_DATA_NAVIGATION_STEPS[] = {
#include "journeys/trader/market_data_navigation/01.inc"
#include "journeys/trader/market_data_navigation/02.inc"
#include "journeys/trader/market_data_navigation/03.inc"
#include "journeys/trader/market_data_navigation/04.inc"
#include "journeys/trader/market_data_navigation/05.inc"
};
static const UmiApplicationJourneyStep TRADER_SIMULATION_ORDER_STEPS[] = {
#include "journeys/trader/simulation_order/01.inc"
#include "journeys/trader/simulation_order/02.inc"
#include "journeys/trader/simulation_order/03.inc"
#include "journeys/trader/simulation_order/04.inc"
#include "journeys/trader/simulation_order/05.inc"
};
static const UmiApplicationJourneyStep TRADER_CANCEL_RECONCILE_STEPS[] = {
#include "journeys/trader/cancel_reconcile/01.inc"
#include "journeys/trader/cancel_reconcile/02.inc"
#include "journeys/trader/cancel_reconcile/03.inc"
#include "journeys/trader/cancel_reconcile/04.inc"
#include "journeys/trader/cancel_reconcile/05.inc"
};
static const UmiApplicationJourneyStep TRADER_KILL_SWITCH_RECOVERY_STEPS[] = {
#include "journeys/trader/kill_switch_recovery/01.inc"
#include "journeys/trader/kill_switch_recovery/02.inc"
#include "journeys/trader/kill_switch_recovery/03.inc"
#include "journeys/trader/kill_switch_recovery/04.inc"
#include "journeys/trader/kill_switch_recovery/05.inc"
};
static const UmiApplicationJourneyStep TRADER_REPLAY_STRATEGY_STEPS[] = {
#include "journeys/trader/replay_strategy/01.inc"
#include "journeys/trader/replay_strategy/02.inc"
#include "journeys/trader/replay_strategy/03.inc"
#include "journeys/trader/replay_strategy/04.inc"
#include "journeys/trader/replay_strategy/05.inc"
};
static const UmiApplicationJourneyStep TRADER_PAPER_SESSION_RECOVERY_STEPS[] = {
#include "journeys/trader/paper_session_recovery/01.inc"
#include "journeys/trader/paper_session_recovery/02.inc"
#include "journeys/trader/paper_session_recovery/03.inc"
#include "journeys/trader/paper_session_recovery/04.inc"
#include "journeys/trader/paper_session_recovery/05.inc"
};
static const UmiApplicationJourneyStep TRADER_LIVE_RELEASE_GATE_STEPS[] = {
#include "journeys/trader/live_release_gate/01.inc"
#include "journeys/trader/live_release_gate/02.inc"
#include "journeys/trader/live_release_gate/03.inc"
#include "journeys/trader/live_release_gate/04.inc"
#include "journeys/trader/live_release_gate/05.inc"
};

#undef STEP
#define JOURNEY(symbol_, application_id_, journey_id_, title_, layout_id_, steps_, blocks_) \
    static const UmiApplicationJourney symbol_ = { \
        sizeof(UmiApplicationJourney), UMI_APPLICATION_JOURNEY_API_VERSION, \
        application_id_, journey_id_, title_, layout_id_, steps_, \
        sizeof(steps_) / sizeof((steps_)[0]), blocks_ \
    };

#include "journeys/studio/first_run_workspace/journey.inc"
#include "journeys/studio/edit_build_test/journey.inc"
#include "journeys/studio/debug_session/journey.inc"
#include "journeys/studio/source_control_review/journey.inc"
#include "journeys/studio/visual_designer_round_trip/journey.inc"
#include "journeys/studio/ai_assisted_change/journey.inc"
#include "journeys/studio/layout_recovery/journey.inc"
#include "journeys/studio/package_release/journey.inc"
#include "journeys/trader/simulation_startup/journey.inc"
#include "journeys/trader/market_data_navigation/journey.inc"
#include "journeys/trader/simulation_order/journey.inc"
#include "journeys/trader/cancel_reconcile/journey.inc"
#include "journeys/trader/kill_switch_recovery/journey.inc"
#include "journeys/trader/replay_strategy/journey.inc"
#include "journeys/trader/paper_session_recovery/journey.inc"
#include "journeys/trader/live_release_gate/journey.inc"

#undef JOURNEY

static const UmiApplicationJourney *const JOURNEYS[] = {
    &STUDIO_FIRST_RUN_WORKSPACE,
    &STUDIO_EDIT_BUILD_TEST,
    &STUDIO_DEBUG_SESSION,
    &STUDIO_SOURCE_CONTROL_REVIEW,
    &STUDIO_VISUAL_DESIGNER_ROUND_TRIP,
    &STUDIO_AI_ASSISTED_CHANGE,
    &STUDIO_LAYOUT_RECOVERY,
    &STUDIO_PACKAGE_RELEASE,
    &TRADER_SIMULATION_STARTUP,
    &TRADER_MARKET_DATA_NAVIGATION,
    &TRADER_SIMULATION_ORDER,
    &TRADER_CANCEL_RECONCILE,
    &TRADER_KILL_SWITCH_RECOVERY,
    &TRADER_REPLAY_STRATEGY,
    &TRADER_PAPER_SESSION_RECOVERY,
    &TRADER_LIVE_RELEASE_GATE
};

UmiStatus umi_application_journey_catalogue_build(
    UmiApplicationJourneyCatalogue *out_catalogue)
{
    size_t index;
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_catalogue, 0, sizeof(*out_catalogue));
    if (sizeof(JOURNEYS) / sizeof(JOURNEYS[0]) >
        UMI_APPLICATION_JOURNEY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < sizeof(JOURNEYS) / sizeof(JOURNEYS[0]); ++index) {
        size_t other;
        UmiStatus status = umi_application_journey_validate(JOURNEYS[index]);
        if (status != UMI_STATUS_OK) return status;
        for (other = 0U; other < index; ++other)
            if (strcmp(JOURNEYS[index]->journey_id,
                       JOURNEYS[other]->journey_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        out_catalogue->entries[out_catalogue->count++] = JOURNEYS[index];
    }
    return UMI_STATUS_OK;
}

const UmiApplicationJourney *umi_application_journey_catalogue_find(
    const UmiApplicationJourneyCatalogue *catalogue,
    const char *journey_id)
{
    size_t index;
    if (catalogue == NULL || journey_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->entries[index]->journey_id, journey_id) == 0)
            return catalogue->entries[index];
    return NULL;
}

size_t umi_application_journey_catalogue_count_for(
    const UmiApplicationJourneyCatalogue *catalogue,
    const char *application_id)
{
    size_t index;
    size_t count = 0U;
    if (catalogue == NULL || application_id == NULL) return 0U;
    for (index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->entries[index]->application_id,
                   application_id) == 0)
            count++;
    return count;
}

