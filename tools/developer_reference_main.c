/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/developer_reference_main.c
 *
 * PURPOSE:
 *   Generate a Markdown reference from the live Framework catalogues so new
 *   developers can discover reusable engines, components, panels and layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/layout_summary.h"
#include "umicom/engine/catalogue.h"

/* Keep Boolean catalogue facts short and consistent in generated tables. */
static const char *yes_no(int value)
{
    return value ? "yes" : "no";
}

/* Print engines first because they provide the domain logic used by components. */
static void print_engines(void)
{
    size_t index;
    (void)puts("## Reusable engines\n");
    (void)puts("| Engine | Maturity | Purpose | Primary header |");
    (void)puts("|---|---|---|---|");
    /* Catalogue access returns borrowed immutable descriptors owned by Framework. */
    for (index = 0U; index < umi_engine_catalogue_count(); ++index) {
        const UmiEngineDescriptor *engine = umi_engine_catalogue_at(index);
        /* A missing descriptor indicates catalogue corruption and is omitted safely. */
        if (engine == NULL) {
            continue;
        }
        (void)printf("| %s | %s | %s | `%s` |\n",
                     engine->display_name,
                     umi_engine_maturity_text(engine->maturity),
                     engine->summary,
                     engine->primary_header);
    }
    (void)putchar('\n');
}

/* Print shared components separately from product panels to show reuse boundaries. */
static void print_components(void)
{
    size_t index;
    (void)puts("## Reusable application components\n");
    (void)puts("| Component ID | Domain | Role | Maturity | Description |");
    (void)puts("|---|---|---|---|---|");
    /* Every row comes from the canonical catalogue rather than a duplicated tool list. */
    for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
        const UmiApplicationComponentDefinition *component =
            umi_application_component_catalogue_at(index);
        /* Defensive omission keeps reference generation useful if one slot is unavailable. */
        if (component == NULL) {
            continue;
        }
        (void)printf("| `%s` | `%s` | %s | %s | %s |\n",
                     component->component_id,
                     component->domain_id,
                     umi_application_component_role_text(component->role),
                     umi_capability_maturity_text(component->maturity),
                     component->description);
    }
    (void)putchar('\n');
}

/* Explain one application's panels, layouts and features as a cohesive workspace. */
static void print_experience(const UmiApplicationExperienceDefinition *experience)
{
    size_t index;
    /* Callers may reuse this helper while filtering catalogues, so NULL is harmless. */
    if (experience == NULL) {
        return;
    }
    (void)printf("## %s\n\n", experience->display_name);
    (void)printf("Application ID: `%s`  \nDefault layout: `%s`  \n",
                 experience->application_id, experience->default_layout_id);
    (void)printf("Panels: %zu · Layouts: %zu · Features: %zu · Readiness: %u%%\n\n",
                 experience->panel_count, experience->layout_count,
                 experience->feature_count,
                 umi_application_experience_readiness_percent(experience));

    (void)puts("### Panels\n");
    (void)puts(
        "| Panel ID | Title | Region | Capability | Dock | Float | "
        "Multiple monitors | Linked context | Multiple instances | "
        "Critical | Description |");
    (void)puts("|---|---|---|---|---|---|---|---|---|---|---|");
    /* Panels are recipes: applications render them through a selected UI adapter. */
    for (index = 0U; index < experience->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel = &experience->panels[index];
        (void)printf(
                     "| `%s` | %s | `%s` | `%s` | %s | %s | %s | %s | "
                     "%s | %s | %s |\n",
                     panel->panel_id, panel->title, panel->default_region,
                     panel->required_capability,
                     yes_no((panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) != 0U),
                     yes_no((panel->flags & UMI_EXPERIENCE_PANEL_FLOATABLE) != 0U),
                     yes_no((panel->flags & UMI_EXPERIENCE_PANEL_MULTI_MONITOR) != 0U),
                     yes_no((panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) != 0U),
                     yes_no((panel->flags & UMI_EXPERIENCE_PANEL_MULTI_INSTANCE) != 0U),
                     yes_no((panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U),
                     panel->summary);
    }

    (void)puts("\n### Layouts\n");
    (void)puts(
        "| Layout ID | Title | Panels | Lockable | Multiple monitors | "
        "Responsive | Linked context | Description |");
    (void)puts("|---|---|---:|---|---|---|---|---|");
    /* Layout summaries keep this generated guide aligned with the same
     * validation and customisation facts consumed by application frontends. */
    for (index = 0U; index < experience->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout = &experience->layouts[index];
        UmiApplicationSuiteLayoutSummary summary;
        UmiStatus status = umi_application_suite_layout_summary_build(
            experience, layout, &summary);

        /* A failed summary means the generated reference would hide invalid
         * product metadata, so print an explicit invalid row instead. */
        if (status != UMI_STATUS_OK) {
            (void)printf(
                "| `%s` | %s | %zu | invalid | invalid | invalid | "
                "invalid | %s |\n",
                layout->layout_id, layout->title,
                layout->panel_count, layout->description);
            continue;
        }
        (void)printf("| `%s` | %s | %zu | %s | %s | %s | %s | %s |\n",
                     layout->layout_id, layout->title,
                     summary.panel_count,
                     summary.lockable ? "yes" : "no",
                     summary.multi_monitor ? "yes" : "no",
                     summary.responsive ? "yes" : "no",
                     summary.context_linked ? "yes" : "no",
                     layout->description);
    }

    (void)puts("\n### Layout panel order\n");
    /* Printing the stable panel sequence makes each layout-to-panel
     * relationship visible without asking a reader to inspect C arrays. */
    for (index = 0U; index < experience->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout = &experience->layouts[index];
        size_t panel_index;

        (void)printf("- `%s`: ", layout->layout_id);
        /* Visit each bounded item once so every record receives the same rule. */
        for (panel_index = 0U; panel_index < layout->panel_count; ++panel_index) {
            /* Separate identifiers after the first while keeping one compact
             * line that remains easy to copy into a design discussion. */
            if (panel_index > 0U) {
                (void)fputs(", ", stdout);
            }
            (void)printf("`%s`", layout->panel_ids[panel_index]);
        }
        (void)putchar('\n');
    }

    (void)puts("\n### Feature status\n");
    (void)puts("| Feature ID | Title | State | Owner | Purpose |");
    (void)puts("|---|---|---|---|---|");
    /* Feature state is printed truthfully so foundations are not advertised as complete. */
    for (index = 0U; index < experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature = &experience->features[index];
        (void)printf("| `%s` | %s | %s | %s | %s |\n",
                     feature->feature_id, feature->title,
                     umi_experience_feature_state_text(feature->state),
                     umi_experience_ownership_text(feature->owner),
                     feature->summary);
    }
    (void)putchar('\n');
}

/* Generate one deterministic reference document from validated Framework truth. */
int main(void)
{
    size_t index;
    /* Invalid catalogues stop generation so a polished document cannot hide bad contracts. */
    if (umi_application_component_catalogue_validate() != UMI_STATUS_OK ||
        umi_application_experience_catalogue_validate() != UMI_STATUS_OK) {
        (void)fputs("Developer reference generation failed: catalogue validation failed.\n",
                    stderr);
        return 2;
    }
    (void)puts("# Umicom Framework Generated Developer Reference\n");
    (void)puts("This reference is generated from Framework-owned catalogues. "
               "Do not edit generated output by hand.\n");
    print_engines();
    print_components();
    (void)puts("# Application experiences\n");
    /* Each canonical experience contributes its own panels, layouts and feature states. */
    for (index = 0U;
         index < umi_application_experience_catalogue_count(); ++index) {
        print_experience(umi_application_experience_catalogue_at(index));
    }
    return 0;
}
