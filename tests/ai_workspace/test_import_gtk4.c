/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_import_gtk4.c
 * PURPOSE: Exercise the actual import controls, reapproval, stale input and retained-widget teardown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <stdio.h>
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
typedef struct PreviewChange
{
    UmiAiWorkspaceGtkPanel **panel;
    GtkTextBuffer *input;
    bool edit, called;
} PreviewChange;
static void ChangeDuringPreview(GtkTextBuffer *buffer, gpointer data)
{
    PreviewChange *change = data;
    if (change->called || gtk_text_buffer_get_char_count(buffer) == 0)
        return;
    change->called = true;
    if (change->edit)
        gtk_text_buffer_set_text(change->input, "New unreviewed input", -1);
    else if (*change->panel != NULL)
    {
        UmiAiWorkspaceGtkPanel *panel = *change->panel;
        *change->panel = NULL;
        UmiAiWorkspaceGtkPanelDestroy(panel);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"apply", "no-preview", "no-approval",  "text-change",     "destination-change",
                           "stale", "retained",   "preview-edit", "preview-destroy", "private",
                           "search"};
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
    UmiAiWorkspace *workspace = NULL;
    UmiAiWorkspaceCancellation *cancellation = NULL;
    UmiAiWorkspaceGtkPanel *panel = NULL;
    UmiAiRuntime runtime;
    umi_ai_runtime_init(&runtime);
    GtkWidget *root = NULL, *apply = NULL, *input = NULL;
    gulong hook = 0UL;
    GtkTextBuffer *previewBuffer = NULL;
    CHECK(umi_data_server_create_memory(&data) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceCancellationCreate(&cancellation) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceCreate(data, &runtime, "native-import", &workspace) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceGtkPanelCreate(workspace, &runtime, cancellation, "Memory-only fixture",
                                       "org.umicom.rag", &panel) == UMI_STATUS_OK);
    root = g_object_ref(UmiAiWorkspaceGtkPanelWidget(panel));
    GtkWidget *foundInput = Find(root, "ai.import.text"), *foundApply = Find(root, "ai.import.apply");
    CHECK(foundInput != NULL && foundApply != NULL);
    input = g_object_ref(foundInput);
    apply = g_object_ref(foundApply);
    GtkWidget *preview = Find(root, "ai.import.prepare"), *approve = Find(root, "ai.import.approve"),
              *destination = Find(root, "ai.import.collection");
    GtkWidget *previewView = Find(root, "ai.import.preview");
    CHECK(preview != NULL && approve != NULL && destination != NULL && previewView != NULL);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input));
    previewBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(previewView));
    gtk_text_buffer_set_text(buffer, "The workshop opens at ten.\nBring a notebook.", -1);
    PreviewChange change = {&panel, buffer, strcmp(name, "preview-edit") == 0, false};
    if (strcmp(name, "preview-edit") == 0 || strcmp(name, "preview-destroy") == 0)
        hook = g_signal_connect(previewBuffer, "changed", G_CALLBACK(ChangeDuringPreview), &change);
    if (strcmp(name, "no-preview") != 0)
        g_signal_emit_by_name(preview, "clicked");
    if (hook != 0UL)
    {
        g_signal_handler_disconnect(previewBuffer, hook);
        hook = 0UL;
        CHECK(change.called);
    }
    if (strcmp(name, "no-approval") != 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
    if (strcmp(name, "text-change") == 0)
        gtk_text_buffer_set_text(buffer, "Different text", -1);
    if (strcmp(name, "destination-change") == 0)
        gtk_editable_set_text(GTK_EDITABLE(destination), "different");
    if (strcmp(name, "stale") == 0)
        CHECK(UmiAiWorkspacePutCollection(workspace, "other", "Other") == UMI_STATUS_OK);
    if (strcmp(name, "retained") == 0)
    {
        UmiAiWorkspaceGtkPanelDestroy(panel);
        panel = NULL;
        CHECK(gtk_text_buffer_get_char_count(buffer) == 0);
    }
    if (strcmp(name, "private") == 0)
    {
        CHECK(UmiGtk4RecordingIsPrivate(input) && UmiGtk4RecordingIsPrivate(previewView));
        goto unchanged;
    }
    g_signal_emit_by_name(apply, "clicked");
    UmiAiWorkspaceSnapshot snapshot;
    CHECK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK);
    if (strcmp(name, "apply") == 0 || strcmp(name, "search") == 0)
    {
        CHECK(snapshot.sourceCount == 1U && snapshot.corpusRevision == 1U);
        g_signal_emit_by_name(apply, "clicked");
        CHECK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK &&
              snapshot.sourceCount == 1U);
        if (strcmp(name, "search") == 0)
        {
            GtkWidget *search = Find(root, "ai.search.run"), *results = Find(root, "ai.search.results");
            CHECK(search != NULL && results != NULL);
            g_signal_emit_by_name(search, "clicked");
            GtkTextBuffer *resultBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(results));
            GtkTextIter begin, end;
            gtk_text_buffer_get_bounds(resultBuffer, &begin, &end);
            char *text = gtk_text_buffer_get_text(resultBuffer, &begin, &end, TRUE);
            bool found = strstr(text, "notice.part.1") != NULL && strstr(text, "opens at ten") != NULL;
            g_free(text);
            CHECK(found);
        }
        goto cleanup;
    }
unchanged:
    CHECK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK && snapshot.sourceCount == 0U);
cleanup:
    if (hook != 0UL)
        g_signal_handler_disconnect(previewBuffer, hook);
    UmiAiWorkspaceGtkPanelDestroy(panel);
    g_clear_object(&root);
    g_clear_object(&input);
    g_clear_object(&apply);
    UmiAiWorkspaceDestroy(workspace);
    UmiAiWorkspaceCancellationDestroy(cancellation);
    umi_data_server_destroy(data);
    umi_ai_runtime_destroy(&runtime);
    return failed;
}
