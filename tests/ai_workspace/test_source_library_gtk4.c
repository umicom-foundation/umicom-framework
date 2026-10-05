/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_source_library_gtk4.c
 * PURPOSE: Exercise the production source-review page, stale selection gates and retained-control lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include "workspace_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static bool TextContains(GtkTextBuffer *buffer, const char *needle)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    char *text = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    bool found = strstr(text, needle) != NULL;
    g_free(text);
    return found;
}
typedef struct Mutation
{
    UmiAiWorkspaceGtkPanel **panel;
    GtkTextBuffer *input;
    bool edit, empty, called;
} Mutation;
static void Mutate(GtkTextBuffer *buffer, gpointer data)
{
    Mutation *mutation = data;
    if (mutation->called || (gtk_text_buffer_get_char_count(buffer) == 0) != mutation->empty)
        return;
    mutation->called = true;
    if (mutation->edit)
        gtk_text_buffer_set_text(mutation->input, "Unreviewed change", -1);
    else if (*mutation->panel != NULL)
    {
        UmiAiWorkspaceGtkPanel *panel = *mutation->panel;
        *mutation->panel = NULL;
        UmiAiWorkspaceGtkPanelDestroy(panel);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"replace",
                           "remove",
                           "copy",
                           "no-preview",
                           "no-approval",
                           "empty",
                           "selection-change",
                           "mode-change",
                           "text-change",
                           "destination-change",
                           "stale",
                           "external-writer",
                           "stale-list",
                           "refresh",
                           "collision",
                           "retained",
                           "preview-edit",
                           "preview-destroy",
                           "apply-destroy",
                           "private",
                           "busy"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    UmiDataServer *data = NULL;
    UmiAiWorkspace *workspace = NULL, *other = NULL;
    UmiAiWorkspaceGtkPanel *panel = NULL;
    UmiAiWorkspaceCancellation *cancellation = NULL;
    UmiAiRuntime *runtime = calloc(1U, sizeof(*runtime));
    GtkWidget *root = NULL, *retainedApply = NULL, *retainedInput = NULL;
    GtkTextBuffer *preview = NULL;
    gulong hook = 0UL;
    CHECK(runtime != NULL);
    umi_ai_runtime_init(runtime);
    OK(umi_data_server_create_memory(&data));
    OK(UmiAiWorkspaceCreate(data, runtime, "native-library", &workspace));
    OK(UmiAiWorkspaceCancellationCreate(&cancellation));
    OK(UmiAiWorkspacePutCollection(workspace, "workshop", "Community workshop"));
    OK(UmiAiWorkspacePutSource(workspace, "notice.part.1", "workshop", "Notice", "Workshop opens at ten.\n",
                               1U));
    OK(UmiAiWorkspacePutSource(workspace, "notice.part.2", "workshop", "Notice", "Bring a notebook.", 2U));
    OK(UmiAiWorkspaceGtkPanelCreate(workspace, runtime, cancellation, "Memory-only fixture", "org.umicom.rag",
                                    &panel));
    root = g_object_ref(UmiAiWorkspaceGtkPanelWidget(panel));
    GtkWidget *list = Find(root, "ai.library.sources"), *refresh = Find(root, "ai.library.refresh"),
              *copy = Find(root, "ai.library.copy"), *prepare = Find(root, "ai.library.prepare"),
              *apply = Find(root, "ai.library.apply"), *approve = Find(root, "ai.library.approve"),
              *input = Find(root, "ai.library.text"), *previewView = Find(root, "ai.library.preview"),
              *operation = Find(root, "ai.library.operation"), *document = Find(root, "ai.library.document"),
              *title = Find(root, "ai.library.title");
    CHECK(list != NULL && refresh != NULL && copy != NULL && prepare != NULL && apply != NULL &&
          approve != NULL && input != NULL && previewView != NULL && operation != NULL && document != NULL &&
          title != NULL);
    retainedApply = g_object_ref(apply);
    retainedInput = g_object_ref(input);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input));
    preview = gtk_text_view_get_buffer(GTK_TEXT_VIEW(previewView));
    g_signal_emit_by_name(refresh, "clicked");
    GtkListBoxRow *first = gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 0),
                  *second = gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 1);
    CHECK(first != NULL && second != NULL);
    if (strcmp(name, "empty") != 0)
    {
        gtk_list_box_select_row(GTK_LIST_BOX(list), first);
        if (strcmp(name, "collision") != 0)
            gtk_list_box_select_row(GTK_LIST_BOX(list), second);
    }
    if (strcmp(name, "copy") == 0)
    {
        g_signal_emit_by_name(copy, "clicked");
        CHECK(TextContains(buffer, "Workshop opens at ten.\nBring a notebook."));
        goto unchanged;
    }
    gtk_editable_set_text(GTK_EDITABLE(document), "notice");
    gtk_editable_set_text(GTK_EDITABLE(title), "Notice");
    gtk_text_buffer_set_text(buffer, "Workshop now opens at noon.", -1);
    if (strcmp(name, "collision") == 0)
    {
        char *large = g_malloc0(1601U);
        memset(large, 'x', 1600U);
        gtk_text_buffer_set_text(buffer, large, -1);
        g_free(large);
    }
    bool removal = strcmp(name, "remove") == 0;
    if (removal)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(operation), 1U);
    if (strcmp(name, "stale-list") == 0)
        OK(UmiAiWorkspacePutCollection(workspace, "new", "New"));
    Mutation mutation = {&panel, buffer, strcmp(name, "preview-edit") == 0,
                         strcmp(name, "apply-destroy") == 0, false};
    if (strcmp(name, "preview-edit") == 0 || strcmp(name, "preview-destroy") == 0)
        hook = g_signal_connect(preview, "changed", G_CALLBACK(Mutate), &mutation);
    if (strcmp(name, "no-preview") != 0)
        g_signal_emit_by_name(prepare, "clicked");
    if (hook != 0UL)
    {
        g_signal_handler_disconnect(preview, hook);
        hook = 0UL;
        CHECK(mutation.called);
    }
    if (strcmp(name, "replace") == 0 || removal)
    {
        CHECK(TextContains(preview, "BEFORE: workshop / notice.part.1") &&
              TextContains(preview, "BEFORE: workshop / notice.part.2"));
        CHECK(removal ? !TextContains(preview, "AFTER:")
                      : TextContains(preview, "AFTER: workshop / notice.part.1") &&
                            TextContains(preview, "noon"));
    }
    if (strcmp(name, "no-approval") != 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
    if (strcmp(name, "selection-change") == 0)
        gtk_list_box_unselect_row(GTK_LIST_BOX(list), second);
    if (strcmp(name, "mode-change") == 0)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(operation), 1U);
    if (strcmp(name, "text-change") == 0)
        gtk_text_buffer_set_text(buffer, "Unreviewed text", -1);
    if (strcmp(name, "destination-change") == 0)
        gtk_editable_set_text(GTK_EDITABLE(document), "different");
    if (strcmp(name, "refresh") == 0)
        g_signal_emit_by_name(refresh, "clicked");
    if (strcmp(name, "stale") == 0)
        OK(UmiAiWorkspacePutCollection(workspace, "changed", "Changed"));
    if (strcmp(name, "external-writer") == 0)
    {
        OK(UmiAiWorkspaceCreate(data, runtime, "native-library", &other));
        OK(UmiAiWorkspacePutCollection(other, "changed", "Changed"));
    }
    if (strcmp(name, "private") == 0)
    {
        CHECK(UmiGtk4RecordingIsPrivate(input) && UmiGtk4RecordingIsPrivate(list) &&
              UmiGtk4RecordingIsPrivate(previewView));
        goto unchanged;
    }
    if (strcmp(name, "retained") == 0)
    {
        UmiAiWorkspaceGtkPanelDestroy(panel);
        panel = NULL;
        CHECK(gtk_text_buffer_get_char_count(buffer) == 0 && gtk_text_buffer_get_char_count(preview) == 0);
    }
    if (strcmp(name, "apply-destroy") == 0)
        hook = g_signal_connect(preview, "changed", G_CALLBACK(Mutate), &mutation);
    if (strcmp(name, "busy") == 0)
        workspace->busy = true;
    g_signal_emit_by_name(apply, "clicked");
    workspace->busy = false;
    if (hook != 0UL)
    {
        g_signal_handler_disconnect(preview, hook);
        hook = 0UL;
        CHECK(mutation.called);
    }
    UmiAiWorkspaceSnapshot snapshot;
    OK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot));
    if (strcmp(name, "replace") == 0 || removal || strcmp(name, "apply-destroy") == 0)
    {
        CHECK(snapshot.sourceCount == (removal ? 0U : 1U) && snapshot.corpusRevision == 3U);
        if (!removal)
        {
            UmiAiWorkspaceSource source;
            OK(UmiAiWorkspaceSourceAt(workspace, 0U, &source));
            CHECK(strcmp(source.text, "Workshop now opens at noon.") == 0);
        }
        g_signal_emit_by_name(retainedApply, "clicked");
        OK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot));
        CHECK(snapshot.corpusRevision == 3U);
        goto cleanup;
    }
unchanged:
    OK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot));
    CHECK(snapshot.sourceCount == 2U && snapshot.corpusRevision == 2U);
cleanup:
    if (hook != 0UL)
        g_signal_handler_disconnect(preview, hook);
    if (workspace != NULL)
        workspace->busy = false;
    UmiAiWorkspaceGtkPanelDestroy(panel);
    g_clear_object(&root);
    g_clear_object(&retainedApply);
    g_clear_object(&retainedInput);
    UmiAiWorkspaceDestroy(other);
    UmiAiWorkspaceDestroy(workspace);
    UmiAiWorkspaceCancellationDestroy(cancellation);
    umi_data_server_destroy(data);
    if (runtime != NULL)
    {
        umi_ai_runtime_destroy(runtime);
        free(runtime);
    }
    return failed;
}
