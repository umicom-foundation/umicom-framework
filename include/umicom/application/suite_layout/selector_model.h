/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/selector_model.h
 *
 * PURPOSE:
 *   Expose a bounded toolkit-neutral layout selector model derived directly
 *   from one canonical application experience definition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_SELECTOR_MODEL_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_SELECTOR_MODEL_H

#include "umicom/application/suite_layout/layout_summary.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX 32U

/**
 * A small, owned description that a menu or welcome screen can display.
 *
 * Text and flags are copied from the canonical catalogue. A frontend may keep
 * this value after the catalogue query has finished and must not free any
 * field inside it.
 */
typedef struct UmiApplicationSuiteLayoutChoice {
    char layout_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    size_t panel_count;
    int default_layout;
    int lockable;
    int multi_monitor;
    int responsive;
    int context_linked;
    int selected;
} UmiApplicationSuiteLayoutChoice;

/**
 * Holds every selectable layout for one application and exactly one current
 * selection. Frontends may copy this value without retaining catalogue data.
 */
typedef struct UmiApplicationSuiteLayoutSelectorModel {
    UmiApplicationSuiteLayoutChoice
        choices[UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX];
    size_t count;
    size_t selected_index;
    uint64_t revision;
} UmiApplicationSuiteLayoutSelectorModel;

/**
 * Builds a selector and chooses the requested or canonical default layout.
 *
 * @param experience Borrowed application metadata from Framework.
 * @param active_layout_id Layout to select, or `NULL` to use the default.
 * @param out_model Receives an owned, frontend-neutral selector model.
 * @return `UMI_STATUS_OK` when all choices are valid and one is selected.
 *
 * @see umi_application_suite_layout_summary_build
 */
UmiStatus umi_application_suite_layout_selector_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *active_layout_id,
    UmiApplicationSuiteLayoutSelectorModel *out_model);
/**
 * Selects one known layout without applying UI or changing saved state.
 *
 * @param model Selector whose current choice will change.
 * @param layout_id Stable identifier of a choice already in `model`.
 * @return `UMI_STATUS_OK` when the choice exists, otherwise not-found or
 *         invalid-argument status.
 */
UmiStatus umi_application_suite_layout_selector_select(
    UmiApplicationSuiteLayoutSelectorModel *model,
    const char *layout_id);
/**
 * Returns the choice currently selected by the model.
 *
 * @param model Previously built selector model.
 * @return A pointer owned by `model`, or `NULL` when no choice is available.
 */
const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_current(
    const UmiApplicationSuiteLayoutSelectorModel *model);
/**
 * Returns one choice by index without changing the current selection.
 *
 * @param model Previously built selector model.
 * @param index Zero-based position in the selector.
 * @return A pointer owned by `model`, or `NULL` when the index is outside it.
 */
const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_at(
    const UmiApplicationSuiteLayoutSelectorModel *model,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
