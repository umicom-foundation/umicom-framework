/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/studio_layouts.c
 *
 * PURPOSE:
 *   Implement Framework-owned Studio workbench presets derived from the supplied welcome, code, debug, designer, diff/merge, testing, database and AI mockups.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/studio_layouts.h"

static UmiStatus add(UmiWsLayoutTemplate *layout, const char *id, UmiWsDockRegion region, int32_t row, int32_t col, double weight) {
    return umi_ws_layout_template_add(layout, id, region, row, col, weight);
}


UmiStatus umi_ws_studio_layout_welcome(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.welcome", "Welcome", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.welcome.create", UMI_WS_DOCK_LEFT, 0, 0, 0.9)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.welcome.recent", UMI_WS_DOCK_CENTRE, 0, 1, 1.1)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.welcome.learn", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.welcome.framework", UMI_WS_DOCK_LEFT, 1, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.welcome.workspaces", UMI_WS_DOCK_CENTRE, 1, 1, 1.1)) != UMI_STATUS_OK) return s;
    return add(layout, "studio.welcome.status", UMI_WS_DOCK_RIGHT, 1, 2, 0.8);
}

UmiStatus umi_ws_studio_layout_code(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.code", "Code", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.project-explorer", UMI_WS_DOCK_LEFT, 0, 0, 0.9)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.outline", UMI_WS_DOCK_LEFT, 1, 0, 0.5)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 2.2)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.terminal-problems", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "studio.ai", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}

UmiStatus umi_ws_studio_layout_debug(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.debug", "Debug", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "debug.call-stack", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "debug.breakpoints", UMI_WS_DOCK_LEFT, 1, 0, 0.6)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 2.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "debug.variables", UMI_WS_DOCK_BOTTOM, 1, 1, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "debug.low-level", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}

UmiStatus umi_ws_studio_layout_designer(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.designer", "Visual Designer", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "designer.palette", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "designer.canvas", UMI_WS_DOCK_CENTRE, 0, 1, 2.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "designer.events", UMI_WS_DOCK_BOTTOM, 1, 1, 0.6)) != UMI_STATUS_OK) return s;
    return add(layout, "designer.properties", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}

UmiStatus umi_ws_studio_layout_diff_merge(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.diff-merge", "Diff & Merge", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "diff.sessions", UMI_WS_DOCK_LEFT, 0, 0, 0.7)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "diff.compare", UMI_WS_DOCK_CENTRE, 0, 1, 2.4)) != UMI_STATUS_OK) return s;
    return add(layout, "diff.merge-result", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7);
}

UmiStatus umi_ws_studio_layout_ai_teacher(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.ai-teacher", "AI Teacher", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "teacher.curriculum", UMI_WS_DOCK_LEFT, 0, 0, 0.9)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "teacher.lesson", UMI_WS_DOCK_LEFT, 1, 0, 0.5)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 2.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "teacher.results", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "teacher.chat", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}

UmiStatus umi_ws_studio_layout_testing(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.testing", "Testing", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "testing.explorer", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 1.9)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "testing.coverage", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "testing.results-output", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7);
}

UmiStatus umi_ws_studio_layout_database(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.database", "Database", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "database.explorer", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "database.sql-editor", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "database.results", UMI_WS_DOCK_BOTTOM, 1, 1, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "database.inspector", UMI_WS_DOCK_RIGHT, 0, 2, 0.8);
}

UmiStatus umi_ws_studio_layout_ai_development(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.ai-development", "AI Development", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.project-explorer", UMI_WS_DOCK_LEFT, 0, 0, 0.7)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "ai.agent-chat", UMI_WS_DOCK_RIGHT, 0, 2, 0.9)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "ai.context-plan", UMI_WS_DOCK_RIGHT, 1, 2, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "studio.terminal-problems", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7);
}

UmiStatus umi_ws_studio_layout_focus(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "studio.focus", "Minimal / Focus", UMI_WS_DOMAIN_STUDIO);
    if (s != UMI_STATUS_OK) return s;
    return add(layout, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 0, 1.0);
}
