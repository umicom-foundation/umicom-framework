/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/session.c
 *
 * PURPOSE:
 *   Implement persistent workbench session state, open panel instances, focus, checkpoints and clean-shutdown evidence separately from immutable layout templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/session.h"

#include <string.h>

#include "internal.h"

static UmiStatus validate_panel(
    const UmiWorkbenchOpenPanel *panel)
{
    if (panel == NULL ||
        panel->structure_size < sizeof(*panel) ||
        !umi_workbench_layout_text_present(panel->instance_id) ||
        !umi_workbench_layout_text_present(panel->panel_id) ||
        !umi_workbench_layout_text_present(
            panel->owner_application_id) ||
        !umi_workbench_layout_text_present(panel->node_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

void umi_workbench_layout_session_init(
    UmiWorkbenchLayoutSession *session,
    const char *session_id,
    const char *user_id,
    const char *workspace_id)
{
    if (session == NULL) {
        return;
    }

    (void)memset(session, 0, sizeof(*session));
    session->structure_size = sizeof(*session);
    session->revision = 1U;
    session->clean_shutdown = false;
    session->recovery_available = false;

    if (session_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            session->session_id,
            sizeof(session->session_id),
            session_id,
            true);
    }
    if (user_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            session->user_id,
            sizeof(session->user_id),
            user_id,
            true);
    }
    if (workspace_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            session->workspace_id,
            sizeof(session->workspace_id),
            workspace_id,
            true);
    }
}

UmiStatus umi_workbench_layout_session_validate(
    const UmiWorkbenchLayoutSession *session)
{
    size_t index;
    size_t other;
    size_t active_count = 0U;

    if (session == NULL ||
        session->structure_size < sizeof(*session) ||
        !umi_workbench_layout_text_present(session->session_id) ||
        !umi_workbench_layout_text_present(session->user_id) ||
        session->panel_count >
            UMI_WORKBENCH_LAYOUT_MAX_OPEN_PANELS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < session->panel_count; ++index) {
        UmiStatus status = validate_panel(
            &session->panels[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        if (session->panels[index].active) {
            active_count += 1U;
        }
        for (other = index + 1U;
             other < session->panel_count;
             ++other) {
            if (strcmp(
                    session->panels[index].instance_id,
                    session->panels[other].instance_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    if (active_count > 1U) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (session->active_panel_instance_id[0] != '\0' &&
        umi_workbench_layout_session_find_panel(
            session,
            session->active_panel_instance_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_session_set_active_layout(
    UmiWorkbenchLayoutSession *session,
    const char *layout_id)
{
    UmiStatus status;

    if (session == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_copy_text(
        session->active_layout_id,
        sizeof(session->active_layout_id),
        layout_id,
        false);
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
        session->clean_shutdown = false;
        session->recovery_available = true;
    }
    return status;
}

UmiStatus umi_workbench_layout_session_set_active_application(
    UmiWorkbenchLayoutSession *session,
    const char *application_id)
{
    UmiStatus status;

    if (session == NULL ||
        !umi_workbench_layout_text_present(application_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_copy_text(
        session->active_application_id,
        sizeof(session->active_application_id),
        application_id,
        false);
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
        session->clean_shutdown = false;
        session->recovery_available = true;
    }
    return status;
}

UmiStatus umi_workbench_layout_session_set_focus(
    UmiWorkbenchLayoutSession *session,
    const char *node_id,
    const char *panel_instance_id)
{
    size_t index;
    UmiWorkbenchOpenPanel *active_panel = NULL;
    UmiStatus status;

    if (session == NULL ||
        !umi_workbench_layout_text_present(node_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (panel_instance_id != NULL &&
        panel_instance_id[0] != '\0') {
        active_panel =
            umi_workbench_layout_session_find_panel_mutable(
                session, panel_instance_id);
        if (active_panel == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
    }

    status = umi_workbench_layout_copy_text(
        session->active_node_id,
        sizeof(session->active_node_id),
        node_id,
        false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            session->active_panel_instance_id,
            sizeof(session->active_panel_instance_id),
            panel_instance_id != NULL ? panel_instance_id : "",
            true);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    for (index = 0U; index < session->panel_count; ++index) {
        session->panels[index].active =
            &session->panels[index] == active_panel;
    }
    session->revision += 1U;
    session->clean_shutdown = false;
    session->recovery_available = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_session_open_panel(
    UmiWorkbenchLayoutSession *session,
    const UmiWorkbenchOpenPanel *panel)
{
    UmiStatus status;

    if (session == NULL || panel == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = validate_panel(panel);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (session->panel_count >=
        UMI_WORKBENCH_LAYOUT_MAX_OPEN_PANELS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_workbench_layout_session_find_panel(
            session, panel->instance_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    session->panels[session->panel_count] = *panel;
    session->panels[session->panel_count].structure_size =
        sizeof(session->panels[session->panel_count]);
    session->panel_count += 1U;
    session->revision += 1U;
    session->clean_shutdown = false;
    session->recovery_available = true;

    if (panel->active) {
        return umi_workbench_layout_session_set_focus(
            session, panel->node_id, panel->instance_id);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_session_close_panel(
    UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id)
{
    size_t index;

    if (session == NULL ||
        !umi_workbench_layout_text_present(panel_instance_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < session->panel_count; ++index) {
        bool active = strcmp(
            session->panels[index].instance_id,
            session->active_panel_instance_id) == 0;
        if (strcmp(
                session->panels[index].instance_id,
                panel_instance_id) != 0) {
            continue;
        }

        for (; index + 1U < session->panel_count; ++index) {
            session->panels[index] =
                session->panels[index + 1U];
        }
        session->panel_count -= 1U;
        (void)memset(
            &session->panels[session->panel_count],
            0,
            sizeof(session->panels[session->panel_count]));
        if (active) {
            session->active_panel_instance_id[0] = '\0';
            session->active_node_id[0] = '\0';
        }
        session->revision += 1U;
        session->clean_shutdown = false;
        session->recovery_available = true;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_workbench_layout_session_update_panel(
    UmiWorkbenchLayoutSession *session,
    const UmiWorkbenchOpenPanel *panel)
{
    UmiWorkbenchOpenPanel *existing;
    UmiStatus status;

    if (session == NULL || panel == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = validate_panel(panel);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    existing = umi_workbench_layout_session_find_panel_mutable(
        session, panel->instance_id);
    if (existing == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    *existing = *panel;
    existing->structure_size = sizeof(*existing);
    existing->revision += 1U;
    session->revision += 1U;
    session->clean_shutdown = false;
    session->recovery_available = true;

    if (existing->active) {
        return umi_workbench_layout_session_set_focus(
            session, existing->node_id, existing->instance_id);
    }
    return UMI_STATUS_OK;
}

const UmiWorkbenchOpenPanel *umi_workbench_layout_session_find_panel(
    const UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id)
{
    size_t index;

    if (session == NULL ||
        !umi_workbench_layout_text_present(panel_instance_id)) {
        return NULL;
    }
    for (index = 0U; index < session->panel_count; ++index) {
        if (strcmp(
                session->panels[index].instance_id,
                panel_instance_id) == 0) {
            return &session->panels[index];
        }
    }
    return NULL;
}

UmiWorkbenchOpenPanel *umi_workbench_layout_session_find_panel_mutable(
    UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id)
{
    return (UmiWorkbenchOpenPanel *)
        umi_workbench_layout_session_find_panel(
            session, panel_instance_id);
}

size_t umi_workbench_layout_session_visible_panel_count(
    const UmiWorkbenchLayoutSession *session)
{
    size_t index;
    size_t count = 0U;

    if (session == NULL) {
        return 0U;
    }
    for (index = 0U; index < session->panel_count; ++index) {
        if (session->panels[index].visible) {
            count += 1U;
        }
    }
    return count;
}

size_t umi_workbench_layout_session_dirty_panel_count(
    const UmiWorkbenchLayoutSession *session)
{
    size_t index;
    size_t count = 0U;

    if (session == NULL) {
        return 0U;
    }
    for (index = 0U; index < session->panel_count; ++index) {
        if (session->panels[index].dirty) {
            count += 1U;
        }
    }
    return count;
}

UmiStatus umi_workbench_layout_session_mark_checkpoint(
    UmiWorkbenchLayoutSession *session,
    uint64_t checkpoint_at_ms)
{
    if (session == NULL || checkpoint_at_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->last_checkpoint_at_ms = checkpoint_at_ms;
    session->recovery_available = true;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_session_mark_clean_shutdown(
    UmiWorkbenchLayoutSession *session,
    uint64_t shutdown_at_ms)
{
    size_t index;

    if (session == NULL || shutdown_at_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->last_checkpoint_at_ms = shutdown_at_ms;
    session->clean_shutdown = true;
    session->recovery_available = false;
    for (index = 0U; index < session->panel_count; ++index) {
        session->panels[index].dirty = false;
    }
    session->revision += 1U;
    return UMI_STATUS_OK;
}
