/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/responsive.h
 *
 * PURPOSE:
 *   Apply deterministic responsive layout rules for smaller windows, remote sessions and different monitor classes without embedding toolkit logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_RESPONSIVE_H
#define UMICOM_WORKBENCH_LAYOUT_RESPONSIVE_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchResponsiveAction {
    UMI_WORKBENCH_RESPONSIVE_NO_CHANGE = 0,
    UMI_WORKBENCH_RESPONSIVE_HIDE_NODE = 1,
    UMI_WORKBENCH_RESPONSIVE_SHOW_NODE = 2,
    UMI_WORKBENCH_RESPONSIVE_MOVE_TO_BOTTOM = 3,
    UMI_WORKBENCH_RESPONSIVE_MOVE_TO_RIGHT = 4,
    UMI_WORKBENCH_RESPONSIVE_COLLAPSE_TO_TABS = 5,
    UMI_WORKBENCH_RESPONSIVE_SET_SPLIT_RATIO = 6
} UmiWorkbenchResponsiveAction;

typedef struct UmiWorkbenchResponsiveRule {
    uint32_t structure_size;
    char rule_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    int32_t minimum_width;
    int32_t maximum_width;
    int32_t minimum_height;
    int32_t maximum_height;
    UmiWorkbenchResponsiveAction action;
    double split_ratio;
    int32_t priority;
    bool enabled;
} UmiWorkbenchResponsiveRule;

typedef struct UmiWorkbenchResponsiveProfile {
    uint32_t structure_size;
    char profile_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    UmiWorkbenchResponsiveRule rules[UMI_WORKBENCH_LAYOUT_MAX_BREAKPOINTS];
    size_t rule_count;
    uint64_t revision;
} UmiWorkbenchResponsiveProfile;

typedef struct UmiWorkbenchResponsiveReport {
    uint32_t structure_size;
    size_t evaluated_rule_count;
    size_t applied_rule_count;
    size_t missing_node_count;
    bool changed;
} UmiWorkbenchResponsiveReport;

void umi_workbench_responsive_profile_init(
    UmiWorkbenchResponsiveProfile *profile,
    const char *profile_id,
    const char *display_name);

UmiStatus umi_workbench_responsive_profile_add_rule(
    UmiWorkbenchResponsiveProfile *profile,
    const UmiWorkbenchResponsiveRule *rule);

UmiStatus umi_workbench_responsive_apply(
    const UmiWorkbenchResponsiveProfile *profile,
    int32_t available_width,
    int32_t available_height,
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchResponsiveReport *out_report);

bool umi_workbench_responsive_rule_matches(
    const UmiWorkbenchResponsiveRule *rule,
    int32_t available_width,
    int32_t available_height);

const char *umi_workbench_responsive_action_text(
    UmiWorkbenchResponsiveAction action);

#ifdef __cplusplus
}
#endif

#endif
