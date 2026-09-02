/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/product_panel_projection.c
 *
 * PURPOSE:
 *   Resolve application layout panels to shared presentation components and
 *   copy their live state into frontend-neutral product projections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/product_panel_projection.h"

#include <stdio.h>
#include <string.h>

/* Friendly layout names sometimes describe a broader task than the reusable
 * component which supplies its data. These aliases record only that semantic
 * join; business behavior remains inside the component controller. */
static const UmiApplicationProductPanelBinding BINDINGS[] = {
    {"org.umicom.desktop", "application-centre", "umicom.shell.activity-bar", "open-application", "Open Application"},
    {"org.umicom.desktop", "taskbar", "umicom.shell.activity-bar", "refresh", "Refresh Applications"},
    {"org.umicom.desktop", "workspace", "umicom.shell.dock-canvas", "refresh", "Refresh Workspace"},
    {"org.umicom.desktop", "notifications", "umicom.shell.notifications", "refresh", "Refresh Notifications"},
    {"org.umicom.desktop", "quick-settings", "umicom.shell.workspace-profiles", "open-settings", "Open Settings"},
    {"org.umicom.desktop", "global-search", "umicom.shell.command-palette", "search", "Search Applications"},
    {"org.umicom.desktop", "operations", "umicom.shell.multi-monitor", "refresh", "Refresh Operations"},
    {"org.umicom.desktop", "context", "umicom.shell.context-links", "refresh", "Refresh Context"},

    {"org.umicom.os", "system-info", "umicom.os.control-centre", "refresh", "Refresh System Information"},
    {"org.umicom.os", "devices", "umicom.os.control-centre", "refresh-devices", "Refresh Devices"},
    {"org.umicom.os", "storage", "umicom.os.control-centre", "refresh-storage", "Refresh Storage"},
    {"org.umicom.os", "network", "umicom.os.control-centre", "refresh-network", "Refresh Network"},
    {"org.umicom.os", "services", "umicom.shared.operations", "refresh-services", "Refresh Services"},
    {"org.umicom.os", "processes", "umicom.os.resource-monitor", "refresh", "Refresh Processes"},
    {"org.umicom.os", "security", "umicom.shared.security", "refresh", "Refresh Security"},
    {"org.umicom.os", "updates", "umicom.shared.operations", "check-updates", "Check for Updates"},
    {"org.umicom.os", "logs", "umicom.os.resource-monitor", "refresh-logs", "Refresh Logs"},
    {"org.umicom.os", "developer-options", "umicom.os.control-centre",
     "open-developer-options", "Open Developer Options"},

    {"org.umicom.bank", "overview", "umicom.shared.banking", "refresh", "Refresh Overview"},
    {"org.umicom.bank", "accounts", "umicom.shared.banking", "connect", "Connect Account"},
    {"org.umicom.bank", "payments", "umicom.shared.payments", "prepare-payment", "Prepare Payment"},
    {"org.umicom.bank", "cards", "umicom.shared.banking", "manage-cards", "Manage Cards"},
    {"org.umicom.bank", "fx", "umicom.shared.payments", "request-quote", "Request Quote"},
    {"org.umicom.bank", "beneficiaries", "umicom.shared.payments", "manage-beneficiaries", "Manage Beneficiaries"},
    {"org.umicom.bank", "external-accounts", "umicom.shared.banking", "connect-account", "Connect Bank"},
    {"org.umicom.bank", "crypto", "umicom.shared.digital-assets", "connect-vault", "Connect Vault"},
    {"org.umicom.bank", "vaulted-assets", "umicom.shared.digital-assets", "review-assets", "Review Assets"},
    {"org.umicom.bank", "umicoin", "umicom.shared.digital-assets", "review-reserves", "Review Reserves"},
    {"org.umicom.bank", "insights", "umicom.shared.positions", "refresh", "Refresh Insights"},
    {"org.umicom.bank", "security", "umicom.shared.audit", "review-security", "Review Security"},
    {"org.umicom.bank", "audit", "umicom.shared.audit", "refresh", "Refresh Audit"},

    {"org.umicom.tms", "trade-capture", "umicom.treasury.trade-capture", "create-draft", "Create Draft"},
    {"org.umicom.tms", "market-data", "umicom.treasury.market-data", "connect", "Connect Market Data"},
    {"org.umicom.tms", "positions", "umicom.treasury.trade-blotter", "refresh", "Refresh Positions"},
    {"org.umicom.tms", "pricing", "umicom.treasury.pricing", "price", "Run Pricing"},
    {"org.umicom.tms", "risk", "umicom.treasury.risk", "calculate", "Calculate Risk"},
    {"org.umicom.tms", "pnl", "umicom.treasury.risk", "explain-pnl", "Explain P&L"},
    {"org.umicom.tms", "limits", "umicom.treasury.risk", "review-limits", "Review Limits"},
    {"org.umicom.tms", "collateral", "umicom.treasury.workflow", "review-collateral", "Review Collateral"},
    {"org.umicom.tms", "task-station", "umicom.treasury.workflow", "refresh", "Refresh Tasks"},
    {"org.umicom.tms", "settlement", "umicom.treasury.settlement", "prepare-settlement", "Prepare Settlement"},
    {"org.umicom.tms", "messaging", "umicom.treasury.settlement", "review-messages", "Review Messages"},
    {"org.umicom.tms", "accounting", "umicom.treasury.accounting", "refresh", "Refresh Accounting"},
    {"org.umicom.tms", "reconciliation", "umicom.treasury.accounting", "reconcile", "Run Reconciliation"},
    {"org.umicom.tms", "engine-ops", "umicom.treasury.workflow", "refresh-operations", "Refresh Operations"},
    {"org.umicom.tms", "context-inspector", "umicom.treasury.trade-blotter", "refresh-context", "Refresh Context"},

    {"org.umicom.music-studio", "ai-create", "umicom.music.prompt", "create", "Create Idea"},
    {"org.umicom.music-studio", "timeline", "umicom.music.arranger", "play", "Play Arrangement"},
    {"org.umicom.music-studio", "ideas", "umicom.music.generations", "generate", "Generate Idea"},
    {"org.umicom.music-studio", "lyrics", "umicom.music.prompt", "edit-lyrics", "Edit Lyrics"},
    {"org.umicom.music-studio", "piano-roll", "umicom.music.piano-roll", "create-clip", "Create MIDI Clip"},
    {"org.umicom.music-studio", "stem-lab", "umicom.music.stems", "separate-stems", "Separate Stems"},
    {"org.umicom.music-studio", "mixer", "umicom.music.stems", "open-mixer", "Open Mixer"},
    {"org.umicom.music-studio", "mastering", "umicom.music.stems", "prepare-master", "Prepare Master"},
    {"org.umicom.music-studio", "automation", "umicom.music.arranger", "add-automation", "Add Automation"},
    {"org.umicom.music-studio", "browser", "umicom.music.generations", "import", "Import Media"},
    {"org.umicom.music-studio", "inspector", "umicom.ai.tools", "refresh", "Refresh Inspector"},
    {"org.umicom.music-studio", "export", "umicom.music.generations", "export", "Export Project"}
};

/* Copying text here gives every frontend an owned snapshot and prevents it
 * from depending on the lifetime of a controller's internal buffers. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

/* Component identifiers use dotted namespaces. Comparing the final segment
 * lets a panel named `payments` automatically reuse `umicom.shared.payments`.
 */
static int component_suffix_matches(const char *component_id, const char *panel_id)
{
    const char *suffix;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component_id == NULL || panel_id == NULL) return 0;
    suffix = strrchr(component_id, '.');
    suffix = suffix != NULL ? suffix + 1 : component_id;
    return strcmp(suffix, panel_id) == 0;
}

/* Search the active recipe rather than the whole catalogue so a projection
 * never routes a command to a component which this surface does not own. */
static const UmiApplicationPresentationSurfaceItem *find_suffix_item(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || panel_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->session.item_count; ++index) {
        const UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (item->placement != NULL && item->placement->panel != NULL &&
            component_suffix_matches(
                item->placement->panel->component_id, panel_id)) {
            return item;
        }
    }
    return NULL;
}

/* Count immutable aliases for diagnostics and catalogue tests. */
size_t umi_application_product_panel_binding_count(void)
{
    return sizeof(BINDINGS) / sizeof(BINDINGS[0]);
}

/* Return a borrowed alias while protecting callers from an invalid index. */
const UmiApplicationProductPanelBinding *
umi_application_product_panel_binding_at(size_t index)
{
    return index < umi_application_product_panel_binding_count()
        ? &BINDINGS[index]
        : NULL;
}

/* Find the semantic join for one application panel without allocating memory. */
const UmiApplicationProductPanelBinding *
umi_application_product_panel_binding_find(
    const char *application_id,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL || panel_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_product_panel_binding_count();
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(BINDINGS[index].application_id, application_id) == 0 &&
            strcmp(BINDINGS[index].panel_id, panel_id) == 0) {
            return &BINDINGS[index];
        }
    }
    return NULL;
}

/* Join immutable panel metadata to its current controller-owned runtime item. */
UmiStatus umi_application_product_panel_project(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *panel_id,
    UmiApplicationProductPanelProjection *out_projection)
{
    const UmiExperiencePanelDefinition *panel;
    const UmiApplicationProductPanelBinding *binding;
    const UmiApplicationPresentationSurfaceItem *item = NULL;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || runtime == NULL || panel_id == NULL ||
        out_projection == NULL || experience->application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = umi_application_experience_panel_find(experience, panel_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel == NULL) return UMI_STATUS_NOT_FOUND;

    (void)memset(out_projection, 0, sizeof(*out_projection));
    /* Size and version let future releases append fields while older clients
     * continue to recognise the part of the value they understand. */
    out_projection->structure_size =
        (uint32_t)sizeof(*out_projection);
    out_projection->api_version =
        UMI_APPLICATION_PRODUCT_PANEL_API_VERSION;
    copy_text(out_projection->application_id,
              sizeof(out_projection->application_id),
              experience->application_id);
    copy_text(out_projection->panel_id, sizeof(out_projection->panel_id),
              panel->panel_id);
    copy_text(out_projection->title, sizeof(out_projection->title),
              panel->title);
    copy_text(out_projection->summary, sizeof(out_projection->summary),
              panel->summary);
    copy_text(out_projection->required_capability,
              sizeof(out_projection->required_capability),
              panel->required_capability);
    out_projection->panel_flags = panel->flags;

    /* Explicit aliases take priority because several task panels may safely
     * share one broader component, such as Bank accounts and card controls. */
    binding = umi_application_product_panel_binding_find(
        experience->application_id, panel_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding != NULL) {
        item = umi_application_presentation_surface_session_find_const(
            &runtime->session, binding->component_id);
        copy_text(out_projection->command_id,
                  sizeof(out_projection->command_id), binding->command_id);
        copy_text(out_projection->command_label,
                  sizeof(out_projection->command_label),
                  binding->command_label);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        item = find_suffix_item(runtime, panel_id);
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || item->placement == NULL ||
        item->placement->panel == NULL) {
        /* An honest unavailable projection is more useful than a blank panel:
         * it preserves the planned feature without claiming it is connected. */
        out_projection->state = UMI_APPLICATION_PRESENTATION_STATE_OFFLINE;
        copy_text(out_projection->message, sizeof(out_projection->message),
                  "This panel is defined, but its runtime component is not connected yet.");
        return UMI_STATUS_OK;
    }

    copy_text(out_projection->component_id,
              sizeof(out_projection->component_id),
              item->placement->panel->component_id);
    copy_text(out_projection->message, sizeof(out_projection->message),
              item->message);
    copy_text(out_projection->badge, sizeof(out_projection->badge),
              item->badge);
    out_projection->state = item->state;
    out_projection->progress_percent = item->progress_percent;
    out_projection->revision = item->revision;
    out_projection->component_bound = 1;
    out_projection->visible = item->visible;
    out_projection->focused = item->focused;
    out_projection->dirty = item->dirty;
    out_projection->has_progress = item->has_progress;

    /* Busy and loading panels reject another action until their controller
     * returns to a stable state. Guarded financial commands remain enabled;
     * their controllers stage approval instead of executing immediately. */
    out_projection->command_enabled =
        out_projection->command_id[0] != '\0' && item->visible &&
        item->state != UMI_APPLICATION_PRESENTATION_STATE_BUSY &&
        item->state != UMI_APPLICATION_PRESENTATION_STATE_LOADING;
    return UMI_STATUS_OK;
}

/* Summarise live coverage so roadmaps never confuse a panel name with a
 * connected implementation. */
UmiStatus umi_application_product_panel_coverage(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationProductPanelCoverage *out_coverage)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || runtime == NULL || out_coverage == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_coverage, 0, sizeof(*out_coverage));
    out_coverage->structure_size = (uint32_t)sizeof(*out_coverage);
    out_coverage->panel_count = experience->panel_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->panel_count; ++index) {
        UmiApplicationProductPanelProjection projection;
        UmiStatus status = umi_application_product_panel_project(
            experience, runtime, experience->panels[index].panel_id,
            &projection);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (projection.component_bound) out_coverage->bound_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else out_coverage->unbound_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (projection.command_enabled) out_coverage->actionable_count += 1U;
    }
    return UMI_STATUS_OK;
}
