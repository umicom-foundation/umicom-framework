/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/responsive.c
 *
 * PURPOSE:
 *   Implement deterministic responsive rules for smaller windows, remote sessions and changed display classes without toolkit-specific conditionals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/responsive.h"

#include <string.h>

#include "internal.h"

/* Provide the rule compare operation used by this module and its client applications. */
static int rule_compare(
    const UmiWorkbenchResponsiveRule *left,
    const UmiWorkbenchResponsiveRule *right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->priority < right->priority) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->priority > right->priority) return 1;
    return strcmp(left->rule_id, right->rule_id);
}

/* Provide the sort rule indices operation used by this module and its client applications. */
static void sort_rule_indices(
    const UmiWorkbenchResponsiveProfile *profile,
    size_t *indices)
{
    size_t index;
    size_t other;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->rule_count; ++index) {
        indices[index] = index;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < profile->rule_count; ++index) {
        size_t selected = indices[index];
        other = index;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (other > 0U &&
               rule_compare(
                   &profile->rules[selected],
                   &profile->rules[indices[other - 1U]]) < 0) {
            indices[other] = indices[other - 1U];
            other -= 1U;
        }
        indices[other] = selected;
    }
}

/* Provide the apply action operation used by this module and its client applications. */
static UmiStatus apply_action(
    const UmiWorkbenchResponsiveRule *rule,
    UmiWorkbenchLayoutNode *node,
    bool *out_changed)
{
    UmiWorkbenchLayoutVisibility visibility;
    UmiWorkbenchLayoutDockRegion region;
    UmiWorkbenchLayoutNodeKind kind;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL || node == NULL || out_changed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_changed = false;

    /* Select the behaviour associated with the requested command or state value. */
    switch (rule->action) {
    case UMI_WORKBENCH_RESPONSIVE_NO_CHANGE:
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_RESPONSIVE_HIDE_NODE:
        visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->visibility != visibility) {
            node->visibility = visibility;
            *out_changed = true;
        }
        break;

    case UMI_WORKBENCH_RESPONSIVE_SHOW_NODE:
        visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->visibility != visibility) {
            node->visibility = visibility;
            *out_changed = true;
        }
        break;

    case UMI_WORKBENCH_RESPONSIVE_MOVE_TO_BOTTOM:
        region = UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->dock_region != region) {
            node->dock_region = region;
            *out_changed = true;
        }
        break;

    case UMI_WORKBENCH_RESPONSIVE_MOVE_TO_RIGHT:
        region = UMI_WORKBENCH_LAYOUT_DOCK_RIGHT;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->dock_region != region) {
            node->dock_region = region;
            *out_changed = true;
        }
        break;

    case UMI_WORKBENCH_RESPONSIVE_COLLAPSE_TO_TABS:
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_workbench_layout_node_is_container(node)) {
            return UMI_STATUS_INVALID_STATE;
        }
        kind = UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind != kind) {
            node->kind = kind;
            node->orientation =
                UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE;
            node->split_ratio = 0.5;
            *out_changed = true;
        }
        break;

    case UMI_WORKBENCH_RESPONSIVE_SET_SPLIT_RATIO:
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_workbench_layout_node_is_container(node) ||
            rule->split_ratio <= 0.05 ||
            rule->split_ratio >= 0.95) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->split_ratio != rule->split_ratio) {
            node->split_ratio = rule->split_ratio;
            *out_changed = true;
        }
        break;

    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (*out_changed) {
        node->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench responsive profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_responsive_profile_init(
    UmiWorkbenchResponsiveProfile *profile,
    const char *profile_id,
    const char *display_name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) {
        return;
    }

    (void)memset(profile, 0, sizeof(*profile));
    profile->structure_size = sizeof(*profile);
    profile->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            profile->profile_id,
            sizeof(profile->profile_id),
            profile_id,
            true);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (display_name != NULL) {
        (void)umi_workbench_layout_copy_text(
            profile->display_name,
            sizeof(profile->display_name),
            display_name,
            true);
    }
}

/*
 * Provide the workbench responsive profile add rule operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_responsive_profile_add_rule(
    UmiWorkbenchResponsiveProfile *profile,
    const UmiWorkbenchResponsiveRule *rule)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || rule == NULL ||
        rule->structure_size < sizeof(*rule) ||
        !umi_workbench_layout_text_present(rule->rule_id) ||
        !umi_workbench_layout_text_present(rule->node_id) ||
        rule->action < UMI_WORKBENCH_RESPONSIVE_NO_CHANGE ||
        rule->action >
            UMI_WORKBENCH_RESPONSIVE_SET_SPLIT_RATIO) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (profile->rule_count >=
        UMI_WORKBENCH_LAYOUT_MAX_BREAKPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->rule_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                profile->rules[index].rule_id,
                rule->rule_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    profile->rules[profile->rule_count] = *rule;
    profile->rules[profile->rule_count].structure_size =
        sizeof(profile->rules[profile->rule_count]);
    profile->rule_count += 1U;
    profile->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench responsive rule matches operation used by this module and its
 * client applications.
 */
bool umi_workbench_responsive_rule_matches(
    const UmiWorkbenchResponsiveRule *rule,
    int32_t available_width,
    int32_t available_height)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL || !rule->enabled ||
        available_width <= 0 || available_height <= 0) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->minimum_width > 0 &&
        available_width < rule->minimum_width) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->maximum_width > 0 &&
        available_width > rule->maximum_width) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->minimum_height > 0 &&
        available_height < rule->minimum_height) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->maximum_height > 0 &&
        available_height > rule->maximum_height) {
        return false;
    }
    return true;
}

/*
 * Perform workbench responsive through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_workbench_responsive_apply(
    const UmiWorkbenchResponsiveProfile *profile,
    int32_t available_width,
    int32_t available_height,
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchResponsiveReport *out_report)
{
    UmiWorkbenchResponsiveReport report;
    size_t indices[UMI_WORKBENCH_LAYOUT_MAX_BREAKPOINTS];
    size_t order;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || document == NULL ||
        available_width <= 0 || available_height <= 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&report, 0, sizeof(report));
    report.structure_size = sizeof(report);
    sort_rule_indices(profile, indices);

    /* Visit each bounded item once so every record receives the same rule. */
    for (order = 0U;
         order < profile->rule_count &&
         status == UMI_STATUS_OK;
         ++order) {
        const UmiWorkbenchResponsiveRule *rule =
            &profile->rules[indices[order]];
        UmiWorkbenchLayoutNode *node;
        bool changed = false;

        report.evaluated_rule_count += 1U;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_responsive_rule_matches(
                rule, available_width, available_height)) {
            continue;
        }
        node = umi_workbench_layout_document_find_node_mutable(
            document, rule->node_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (node == NULL) {
            report.missing_node_count += 1U;
            continue;
        }

        status = apply_action(rule, node, &changed);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && changed) {
            report.applied_rule_count += 1U;
            report.changed = true;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && report.changed) {
        umi_workbench_layout_document_increment_revision(document);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_report != NULL) {
        *out_report = report;
    }
    return status;
}

/*
 * Provide the workbench responsive action text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_responsive_action_text(
    UmiWorkbenchResponsiveAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
    case UMI_WORKBENCH_RESPONSIVE_NO_CHANGE:
        return "no-change";
    case UMI_WORKBENCH_RESPONSIVE_HIDE_NODE:
        return "hide-node";
    case UMI_WORKBENCH_RESPONSIVE_SHOW_NODE:
        return "show-node";
    case UMI_WORKBENCH_RESPONSIVE_MOVE_TO_BOTTOM:
        return "move-to-bottom";
    case UMI_WORKBENCH_RESPONSIVE_MOVE_TO_RIGHT:
        return "move-to-right";
    case UMI_WORKBENCH_RESPONSIVE_COLLAPSE_TO_TABS:
        return "collapse-to-tabs";
    case UMI_WORKBENCH_RESPONSIVE_SET_SPLIT_RATIO:
        return "set-split-ratio";
    default:
        return "invalid";
    }
}
