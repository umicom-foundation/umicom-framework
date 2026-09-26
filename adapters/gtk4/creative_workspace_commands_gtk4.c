/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_commands_gtk4.c
 * PURPOSE: Translate explicit GTK actions into the single Framework creative workspace owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "creative_workspace_private.h"
#include <glib/gstdio.h>
#include <inttypes.h>
#include <string.h>
static const char * TextValue(GtkWidget * widget){return gtk_editable_get_text(GTK_EDITABLE(widget));
}

static int32_t NumberValue(GtkWidget * widget){return (int32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

static UmiStatus Copy(char * out, size_t capacity, const char * text)
{
    if (text == NULL || strlen(text) >= capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, strlen(text) + 1U);
    return UMI_STATUS_OK;
}

void UmiCreativeGtkStatus(UmiCreativeGtkPanel * p, UmiStatus status, const char * operation)
{
    const char * reason = "completed";
    switch (status){
        case UMI_STATUS_OK:break;
        case UMI_STATUS_BUSY:reason = "the project changed elsewhere; reload and review your edit";
        break;
        case UMI_STATUS_ALREADY_EXISTS:reason = "that ID or file already exists; use Update or choose a new name";
        break;
        case UMI_STATUS_NOT_FOUND:reason = "the requested project, object, note or history step was not found";
        break;
        case UMI_STATUS_INVALID_ARGUMENT:reason = "check IDs, units, timing, bounds and the chosen file path";
        break;
        case UMI_STATUS_CAPACITY_EXCEEDED:reason = "the configured capacity has been reached";
        break;
        case UMI_STATUS_INVALID_STATE:reason = "not available in this state; after a storage recovery error, close and reopen the application";
        break;
        case UMI_STATUS_NOT_IMPLEMENTED:reason = "this operation is not supported here; the collision playground requires static objects";
        break;
        case UMI_STATUS_PARSE_ERROR:reason = "saved records are incomplete or invalid; they have not been replaced";
        break;
        case UMI_STATUS_IO_ERROR:reason = "storage or export failed; inspect the path and any newly-created partial file";
        break;
        case UMI_STATUS_UNAVAILABLE:reason = "the required storage backend is unavailable";
        break;
        default : reason = "the operation could not complete";
        break;
    }
    char * message = g_strdup_printf("%s: %s (status %d).", operation, reason, (int)status);
    gtk_label_set_text(GTK_LABEL(p -> status), message);
    g_free(message);
}

static UmiStatus EnsureServer(UmiCreativeGtkPanel * p)
{
    if (p -> server!= NULL)return UMI_STATUS_OK;
    const char * base = g_get_user_data_dir();
    if (base == NULL || !g_path_is_absolute(base))return UMI_STATUS_INVALID_ARGUMENT;
    char * directory = g_build_filename(base, "umicom", "creative-workspace", NULL);
    char * path = g_build_filename(directory, "projects.sqlite3", NULL);
    UmiStatus status = g_mkdir_with_parents(directory, 0700) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)status = umi_data_server_create_sqlite(path, & p -> server);
    if (status == UMI_STATUS_OK){p -> ownsServer = true;
        gtk_label_set_text(GTK_LABEL(p -> storage), path);
    }
    g_free(path);
    g_free(directory);
    return status;
}

static UmiStatus Settings(UmiCreativeGtkPanel * p, UmiCreativeSettings * settings)
{
    memset(settings, 0, sizeof( * settings));
    UmiStatus status = Copy(settings -> title, sizeof(settings -> title), TextValue(p -> title));
    guint unit = gtk_drop_down_get_selected(GTK_DROP_DOWN(p -> unit));
    if (unit > 1U)return UMI_STATUS_INVALID_ARGUMENT;
    settings -> unit = (UmiCreativeUnit)unit;
    settings -> width = NumberValue(p -> width);
    settings -> height = NumberValue(p -> height);
    settings -> durationMs = (uint32_t)NumberValue(p -> duration);
    settings -> tempoBpm = (uint32_t)NumberValue(p -> tempo);
    return status;
}

static void FillSettings(UmiCreativeGtkPanel * p)
{
    const UmiCreativeProject * value = p -> project;
    gtk_editable_set_text(GTK_EDITABLE(p -> projectId), value -> id);
    gtk_editable_set_text(GTK_EDITABLE(p -> title), value -> settings.title);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(p -> unit), (guint)value -> settings.unit);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> width), value -> settings.width);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> height), value -> settings.height);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> duration), (double)value -> settings.durationMs);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> tempo), (double)value -> settings.tempoBpm);
}

static UmiStatus OpenProject(UmiCreativeGtkPanel * p, bool create)
{
    UmiCreativeProject * draft = g_new0(UmiCreativeProject, 1);
    UmiStatus status = UMI_STATUS_OK;
    if (create){UmiCreativeSettings settings;
        status = Settings(p, & settings);
        if (status == UMI_STATUS_OK)status = UmiCreativeProjectInit(draft, TextValue(p -> projectId), & settings);
    }
    if (status == UMI_STATUS_OK)status = EnsureServer(p);
    UmiCreativeWorkspace * next = NULL;
    if (status == UMI_STATUS_OK)status = create ? UmiCreativeWorkspaceCreate(p -> server, draft, & next) : UmiCreativeWorkspaceOpen(p -> server, TextValue(p -> projectId), & next);
    if (status == UMI_STATUS_OK){UmiCreativeWorkspaceDestroy(p -> workspace);
        p -> workspace = next;
        UmiCreativeGtkRefresh(p);
        FillSettings(p);
    }
    g_free(draft);
    return status;
}

void UmiCreativeGtkLoadObject(UmiCreativeGtkPanel * p)
{
    if (p -> workspace == NULL)return;
    UmiCreativeElement element;
    UmiStatus status = UmiCreativeProjectFindElement(p -> project, TextValue(p -> elementId), & element);
    if (status == UMI_STATUS_OK){
        gtk_editable_set_text(GTK_EDITABLE(p -> label), element.label);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(p -> kind), (guint)element.kind-1U);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> x), element.x);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> y), element.y);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> objectWidth), element.width);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(p -> objectHeight), element.height);
        char colour[8];
        (void)g_snprintf(colour, sizeof(colour), "#%06X", (unsigned)element.colourRgb);
        gtk_editable_set_text(GTK_EDITABLE(p -> colour), colour);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(p -> visible), element.visible);
    }
    UmiCreativeGtkStatus(p, status, "Load object");
}

static UmiStatus ObjectEdit(UmiCreativeGtkPanel * p, UmiCreativeProject * draft, bool update)
{
    const char * text = TextValue(p -> colour);
    uint32_t rgb = 0U;
    if (strlen(text)!= 7U || text[0]!= '#')return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 1U;i < 7U; ++ i){int digit = g_ascii_xdigit_value(text[i]);
        if (digit < 0)return UMI_STATUS_INVALID_ARGUMENT;
        rgb = rgb * 16U + (uint32_t)digit;
    }
    guint kind = gtk_drop_down_get_selected(GTK_DROP_DOWN(p -> kind));
    if (kind > 5U)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeElement element;
    UmiStatus status = UmiCreativeElementInit( & element, TextValue(p -> elementId), TextValue(p -> label), (UmiCreativeElementKind)(kind + 1U),
    NumberValue(p -> x), NumberValue(p -> y), NumberValue(p -> objectWidth), NumberValue(p -> objectHeight), rgb);
    if (status!= UMI_STATUS_OK)return status;
    element.visible = gtk_check_button_get_active(GTK_CHECK_BUTTON(p -> visible));
    if (update){UmiCreativeElement previous;
        status = UmiCreativeProjectFindElement(draft, element.id, & previous);
        if (status!= UMI_STATUS_OK)return status;
        memcpy(element.keys, previous.keys, sizeof(element.keys));
        element.keyCount = previous.keyCount;
    }
    return UmiCreativeProjectPutElement(draft, & element, update);
}

static UmiStatus Edit(UmiCreativeGtkPanel * p, const char * action)
{
    UmiCreativeProject * draft = g_new(UmiCreativeProject, 1);
    * draft = * p -> project;
    const char * id = TextValue(p -> elementId);
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(action, "project.starter") == 0)status = UmiCreativeProjectStarter(draft, p -> profile);
    else if (strcmp(action, "settings.save") == 0){UmiCreativeSettings settings;
        status = Settings(p, & settings);
        if (status == UMI_STATUS_OK)status = UmiCreativeProjectConfigure(draft, & settings);
    }
    else if (strcmp(action, "object.add") == 0 || strcmp(action, "object.update") == 0)status = ObjectEdit(p, draft, strcmp(action, "object.update") == 0);
    else if (strcmp(action, "object.remove") == 0)status = UmiCreativeProjectRemoveElement(draft, id);
    else if (strcmp(action, "object.front") == 0 || strcmp(action, "object.back") == 0)
    status = draft -> elementCount == 0U ? UMI_STATUS_NOT_FOUND : UmiCreativeProjectMoveLayer(draft, id, strcmp(action, "object.front") == 0 ? draft -> elementCount-1U : 0U);
    else if (strcmp(action, "key.save") == 0){guint interpolation = gtk_drop_down_get_selected(GTK_DROP_DOWN(p -> interpolation));
        UmiCreativeKey key = {(uint32_t)NumberValue(p -> keyTime), NumberValue(p -> offsetX), NumberValue(p -> offsetY), (UmiMediaAnimationInterpolation)interpolation};
        status = interpolation > 1U ? UMI_STATUS_INVALID_ARGUMENT : UmiCreativeProjectPutKey(draft, id, & key);
    }
    else if (strcmp(action, "key.remove") == 0)status = UmiCreativeProjectRemoveKey(draft, id, (uint32_t)NumberValue(p -> keyTime));
    else if (strcmp(action, "note.remove") == 0)status = UmiCreativeProjectRemoveNote(draft, TextValue(p -> noteId));
    else if (strcmp(action, "note.add") == 0 || strcmp(action, "note.update") == 0){UmiCreativeNote note = {0};
        status = Copy(note.id, sizeof(note.id), TextValue(p -> noteId));
        note.startTick = (uint32_t)NumberValue(p -> startTick);
        note.durationTicks = (uint32_t)NumberValue(p -> durationTicks);
        note.pitch = (uint8_t)NumberValue(p -> pitch);
        note.velocity = (uint8_t)NumberValue(p -> velocity);
        if (status == UMI_STATUS_OK)status = UmiCreativeProjectPutNote(draft, & note, strcmp(action, "note.update") == 0);
    }
    if (status == UMI_STATUS_OK)status = UmiCreativeWorkspaceReplace(p -> workspace, draft, p -> project -> revision);
    if (status == UMI_STATUS_OK){UmiCreativeGtkRefresh(p);
        if (strcmp(action, "project.starter") == 0)FillSettings(p);
    }
    g_free(draft);
    return status;
}

static UmiStatus Export(UmiCreativeGtkPanel * p, bool suggest)
{
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(p -> exportKind));
    if (selected > 3U)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeExportKind kind = (UmiCreativeExportKind)selected;
    if (suggest){const char * directory = g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS);
        if (directory == NULL)directory = g_get_home_dir();
        if (directory == NULL || !g_path_is_absolute(directory))return UMI_STATUS_INVALID_ARGUMENT;
        char * name = g_strdup_printf("%s-r%" PRIu64 "%s", p -> project -> id, p -> project -> revision, UmiCreativeExportExtension(kind));
        char * path = g_build_filename(directory, name, NULL);
        gtk_editable_set_text(GTK_EDITABLE(p -> exportPath), path);
        g_free(path);
        g_free(name);
        return UMI_STATUS_OK;
    }
    UmiCreativeExport exported = {0};
    UmiStatus status = UmiCreativeExportBuild(p -> project, kind, (uint32_t)gtk_range_get_value(GTK_RANGE(p -> timeline)), & exported);
    if (status == UMI_STATUS_OK)status = UmiCreativeExportWriteNew( & exported, TextValue(p -> exportPath));
    UmiCreativeExportFree( & exported);
    return status;
}

static UmiStatus Play(UmiCreativeGtkPanel * p, const char * action)
{
    const char * id = TextValue(p -> elementId);
    if (!p -> playActive || strcmp(id, p -> play.actorId)!= 0 || strcmp(action, "play.reset") == 0){
        UmiStatus status = UmiCreativePlayBegin(p -> project, id, & p -> play);
        if (status!= UMI_STATUS_OK)return status;
        p -> playActive = true;
    }
    int32_t dx = strcmp(action, "play.left") == 0 ? -20 : strcmp(action, "play.right") == 0 ? 20 : 0;
    int32_t dy = strcmp(action, "play.up") == 0 ? -20 : strcmp(action, "play.down") == 0 ? 20 : 0;
    UmiCreativeMotion motion;
    UmiStatus status = UmiCreativePlayMove(p -> project, & p -> play, dx, dy, & motion);
    if (status == UMI_STATUS_OK)UmiCreativeGtkFrameRefresh(p);
    return status;
}

void UmiCreativeGtkAction(GtkButton * button, gpointer data)
{
    UmiCreativeGtkPanel * p = data;
    const char * action = g_object_get_data(G_OBJECT(button), "creative-action");
    if (p -> refreshing || action == NULL)return;
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    if (strcmp(action, "project.create") == 0 || strcmp(action, "project.open") == 0)status = OpenProject(p, strcmp(action, "project.create") == 0);
    else if (p -> workspace!= NULL){
        UmiCreativeHistory history = {0};
        (void)UmiCreativeWorkspaceHistory(p -> workspace, & history);
        if (history.recoveryRequired){
            p -> frameValid = false;
            p -> playActive = false;
            gtk_widget_queue_draw(p -> canvas);
            UmiCreativeGtkStatus(p, UMI_STATUS_INVALID_STATE, "Workspace recovery required");
            return;
        }
        if (strcmp(action, "object.load") == 0){UmiCreativeGtkLoadObject(p);
            return;
        }
        if (strcmp(action, "history.undo") == 0 || strcmp(action, "history.redo") == 0){
            status = strcmp(action, "history.undo") == 0 ? UmiCreativeWorkspaceUndo(p -> workspace, p -> project -> revision) : UmiCreativeWorkspaceRedo(p -> workspace, p -> project -> revision);
            if (status == UMI_STATUS_OK){UmiCreativeGtkRefresh(p);
                FillSettings(p);
            }}
        else if (strcmp(action, "project.reload") == 0){status = UmiCreativeWorkspaceReload(p -> workspace);
            if (status == UMI_STATUS_OK){UmiCreativeGtkRefresh(p);
                FillSettings(p);
            }}
        else if (g_str_has_prefix(action, "export."))status = Export(p, strcmp(action, "export.suggest") == 0);
        else if (g_str_has_prefix(action, "play."))status = Play(p, action);
        else status = Edit(p, action);
    }
    UmiCreativeGtkStatus(p, status, action);
}

