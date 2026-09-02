/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/cross_navigation.c
 *
 * PURPOSE:
 *   Implement unified cross-domain IDE navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/cross_navigation.h"

#include <string.h>

/*
 * Initialise ide cross navigation from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_cross_navigation_init(
    UmiIdeCrossNavigation *navigation,
    UmiIdeIntegrationBindings *bindings)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(navigation, 0, sizeof(*navigation));
    navigation->bindings = bindings;
    navigation->revision = 1U;

    status = umi_ide_navigation_history_create(&navigation->history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(navigation, 0, sizeof(*navigation));
    }
    return status;
}

/*
 * Provide the ide cross navigation deinit operation used by this module and its client
 * applications.
 */
void umi_ide_cross_navigation_deinit(
    UmiIdeCrossNavigation *navigation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL) return;
    umi_ide_navigation_history_destroy(navigation->history);
    (void)memset(navigation, 0, sizeof(*navigation));
}

/*
 * Provide the ide cross navigation set debug resolver operation used by this module and
 * its client applications.
 */
UmiStatus umi_ide_cross_navigation_set_debug_resolver(
    UmiIdeCrossNavigation *navigation,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->bindings == NULL ||
        navigation->bindings->debug_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ide_debug_bridge_init(
        &navigation->debug_bridge,
        navigation->bindings->debug_runtime,
        resolver,
        user_data);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        navigation->debug_bridge_ready = 1;
        navigation->revision += 1U;
    }
    return status;
}

/*
 * Provide the ide cross navigation open operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_open(
    UmiIdeCrossNavigation *navigation,
    const UmiIdeNavigationTarget *target,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->bindings == NULL ||
        target == NULL || out_view_id == NULL ||
        out_offset == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation->bindings->documents == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ide_document_open_location(
        navigation->bindings->documents,
        &target->location,
        out_view_id,
        view_id_capacity,
        out_offset);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation->bindings->navigation != NULL) {
        status = umi_ide_productivity_record_visit(
            navigation->bindings->navigation,
            &target->location);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_ide_navigation_history_visit(
        navigation->history,
        target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) navigation->revision += 1U;
    return status;
}

#define OPEN_TARGET(expression) \
    do { \
        UmiIdeNavigationTarget target; \
        UmiStatus status = (expression); \
        if (status != UMI_STATUS_OK) return status; \
        return umi_ide_cross_navigation_open( \
            navigation, &target, out_view_id, view_id_capacity, out_offset); \
    } while (0)

/*
 * Provide the ide cross navigation problem operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_problem(
    UmiIdeCrossNavigation *navigation,
    size_t problem_index,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->bindings == NULL ||
        navigation->bindings->problems == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    OPEN_TARGET(umi_ide_problem_target(
        navigation->bindings->problems,
        problem_index,
        &target));
}

/*
 * Provide the ide cross navigation test operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_test(
    UmiIdeCrossNavigation *navigation,
    const char *test_item_id,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->bindings == NULL ||
        navigation->bindings->tests == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    OPEN_TARGET(umi_ide_test_item_target(
        navigation->bindings->tests,
        test_item_id,
        &target));
}

/*
 * Provide the ide cross navigation source control operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_cross_navigation_source_control(
    UmiIdeCrossNavigation *navigation,
    const char *path,
    uint32_t line,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    OPEN_TARGET(umi_ide_source_control_target(
        path,
        line,
        &target));
}

/*
 * Provide the ide cross navigation symbol operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_symbol(
    UmiIdeCrossNavigation *navigation,
    const char *symbol_id,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->bindings == NULL ||
        navigation->bindings->language == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    OPEN_TARGET(umi_ide_language_symbol_target(
        navigation->bindings->language,
        symbol_id,
        &target));
}

/*
 * Provide the ide cross navigation diagnostic operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_diagnostic(
    UmiIdeCrossNavigation *navigation,
    size_t diagnostic_index,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->bindings == NULL ||
        navigation->bindings->language == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    OPEN_TARGET(umi_ide_language_diagnostic_target(
        navigation->bindings->language,
        diagnostic_index,
        &target));
}

/*
 * Provide the ide cross navigation debug frame operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_cross_navigation_debug_frame(
    UmiIdeCrossNavigation *navigation,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || !navigation->debug_bridge_ready) {
        return UMI_STATUS_INVALID_STATE;
    }

    OPEN_TARGET(umi_ide_debug_active_frame_target(
        &navigation->debug_bridge,
        &target));
}

#undef OPEN_TARGET

/*
 * Provide the open history target operation used by this module and its client
 * applications.
 */
static UmiStatus open_history_target(
    UmiIdeCrossNavigation *navigation,
    UmiIdeNavigationTarget *target,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    UmiStatus status;

    status = umi_ide_document_open_location(
        navigation->bindings->documents,
        &target->location,
        out_view_id,
        view_id_capacity,
        out_offset);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        navigation->bindings->navigation != NULL) {
        status = umi_ide_productivity_record_visit(
            navigation->bindings->navigation,
            &target->location);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) navigation->revision += 1U;
    return status;
}

/*
 * Provide the ide cross navigation back operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_back(
    UmiIdeCrossNavigation *navigation,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    UmiIdeNavigationTarget target;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->history == NULL ||
        navigation->bindings == NULL ||
        navigation->bindings->documents == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ide_navigation_history_back(
        navigation->history,
        &target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return open_history_target(
        navigation,
        &target,
        out_view_id,
        view_id_capacity,
        out_offset);
}

/*
 * Provide the ide cross navigation forward operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_cross_navigation_forward(
    UmiIdeCrossNavigation *navigation,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    UmiIdeNavigationTarget target;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || navigation->history == NULL ||
        navigation->bindings == NULL ||
        navigation->bindings->documents == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ide_navigation_history_forward(
        navigation->history,
        &target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return open_history_target(
        navigation,
        &target,
        out_view_id,
        view_id_capacity,
        out_offset);
}
