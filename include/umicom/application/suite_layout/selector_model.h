/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/selector_model.h
 *
 * PURPOSE:
 *   Expose a bounded toolkit-neutral layout selector model derived directly
 *   from one canonical application experience definition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_SELECTOR_MODEL_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_SELECTOR_MODEL_H

#include "umicom/application/experience.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX 32U

typedef struct UmiApplicationSuiteLayoutChoice {
    char layout_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    int selected;
} UmiApplicationSuiteLayoutChoice;

typedef struct UmiApplicationSuiteLayoutSelectorModel {
    UmiApplicationSuiteLayoutChoice
        choices[UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX];
    size_t count;
    size_t selected_index;
    uint64_t revision;
} UmiApplicationSuiteLayoutSelectorModel;

UmiStatus umi_application_suite_layout_selector_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *active_layout_id,
    UmiApplicationSuiteLayoutSelectorModel *out_model);
UmiStatus umi_application_suite_layout_selector_select(
    UmiApplicationSuiteLayoutSelectorModel *model,
    const char *layout_id);
const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_current(
    const UmiApplicationSuiteLayoutSelectorModel *model);
const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_at(
    const UmiApplicationSuiteLayoutSelectorModel *model,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
