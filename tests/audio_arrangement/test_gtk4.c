/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/audio_arrangement/test_gtk4.c
 * PURPOSE: Exercise Media and Music arrangement controls using isolated local audio and saved library fixtures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
/* The audio fixture supplies the same explicit assertion policy. Avoid two
 * independently named macro parameters producing a redefinition diagnostic. */
#undef CHECK
#include "fixture.h"
#include "umicom/ui/gtk4/audio_arrangement.h"
#include "umicom/creative_workspace/asset_library_archive.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
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
static void Field(GtkWidget *root, const char *id, const char *text)
{
    GtkWidget *field = Find(root, id);
    CHECK(field != NULL);
    gtk_editable_set_text(GTK_EDITABLE(field), text);
}
static void Spin(GtkWidget *root, const char *id, double value)
{
    GtkWidget *field = Find(root, id);
    CHECK(field != NULL);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(field), value);
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(button != NULL);
    g_signal_emit_by_name(button, "clicked");
}
static UmiCreativeAudioArrangement Snapshot(GtkWidget *root, bool *rendered)
{
    UmiCreativeAudioArrangement plan;
    CHECK(UmiCreativeAudioArrangementGtkSnapshot(root, &plan, rendered) == UMI_STATUS_OK);
    return plan;
}
static void Wait(GtkWidget *root)
{
    UmiCreativeAudioArrangement plan;
    bool rendered = false;
    gint64 deadline = g_get_monotonic_time() + 10000000;
    while (UmiCreativeAudioArrangementGtkSnapshot(root, &plan, &rendered) == UMI_STATUS_BUSY)
    {
        CHECK(g_get_monotonic_time() < deadline);
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
int main(int argc, char **argv)
{
    const char *cases[] = {"add",
                           "duplicate",
                           "replace",
                           "remove",
                           "invalid-retains",
                           "render",
                           "export",
                           "existing-export",
                           "edit-invalidates",
                           "library-invalidates",
                           "cancelled-render",
                           "save-open",
                           "open-approval",
                           "path-approval",
                           "failed-open",
                           "retained-control",
                           "close-busy"};
    if (argc != 2 || !Known(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
        return 2;
    const char *mode = argv[1];
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    GtkWidget *root = UmiCreativeAudioArrangementGtkCreate();
    CHECK(root != NULL);
    g_object_ref_sink(root);
    char directory[UMI_PATH_CAPACITY], library_path[UMI_PATH_CAPACITY], plan_path[UMI_PATH_CAPACITY],
        output[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(library_path, directory, "sources.umilibrary");
    FixturePath(plan_path, directory, "arrangement.json");
    FixturePath(output, directory, "mix.wav");
    UmiCreativeAssetLibrary *library = Sources(8000U, 1U, 16000, 16000);
    UmiCreativeAssetWriteResult receipt;
    CHECK(UmiCreativeAssetLibrarySaveNew(library, library_path, NULL, &receipt) == UMI_STATUS_OK);
    UmiCreativeAssetLibraryDestroy(library);
    Field(root, "creative.arrangement.library-path", library_path);
    Click(root, "creative.arrangement.load-library");
    Wait(root);
    Spin(root, "creative.arrangement.rate", 8000);
    Click(root, "creative.arrangement.settings");
    Field(root, "creative.arrangement.id", "first");
    Field(root, "creative.arrangement.asset-id", "voice");
    Spin(root, "creative.arrangement.end", 16);
    Click(root, "creative.arrangement.add");
    bool rendered = false;
    CHECK(Snapshot(root, &rendered).clip_count == 1U && !rendered);
    Field(root, "creative.arrangement.plan-path", plan_path);
    Field(root, "creative.arrangement.output-path", output);
    if (strcmp(mode, "duplicate") == 0)
    {
        Click(root, "creative.arrangement.add");
        CHECK(Snapshot(root, &rendered).clip_count == 1U);
    }
    else if (strcmp(mode, "replace") == 0)
    {
        Spin(root, "creative.arrangement.gain", 500);
        Click(root, "creative.arrangement.replace");
        CHECK(Snapshot(root, &rendered).clips[0].gain_permille == 500U);
        Spin(root, "creative.arrangement.gain", 0);
        Click(root, "creative.arrangement.select");
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Find(root, "creative.arrangement.gain"))) ==
              500);
    }
    else if (strcmp(mode, "remove") == 0)
    {
        Click(root, "creative.arrangement.remove");
        CHECK(Snapshot(root, &rendered).clip_count == 0U);
    }
    else if (strcmp(mode, "save-open") == 0 || strcmp(mode, "failed-open") == 0)
    {
        Click(root, "creative.arrangement.save-plan");
        Wait(root);
        CHECK(umi_fs_exists(plan_path));
        Field(root, "creative.arrangement.id", "second");
        Click(root, "creative.arrangement.add");
        CHECK(Snapshot(root, &rendered).clip_count == 2U);
        if (strcmp(mode, "failed-open") == 0)
            CHECK(UmiRootedFileWrite(directory, "arrangement.json", "invalid", 7U) == UMI_STATUS_OK);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "creative.arrangement.approval")), TRUE);
        Click(root, "creative.arrangement.open-plan");
        Wait(root);
        CHECK(Snapshot(root, &rendered).clip_count == (strcmp(mode, "save-open") == 0 ? 1U : 2U));
    }
    else if (strcmp(mode, "open-approval") == 0 || strcmp(mode, "path-approval") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "creative.arrangement.open-plan")));
        Click(root, "creative.arrangement.open-plan");
        CHECK(Snapshot(root, &rendered).clip_count == 1U);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "creative.arrangement.approval")), TRUE);
        CHECK(gtk_widget_get_sensitive(Find(root, "creative.arrangement.open-plan")));
        if (strcmp(mode, "path-approval") == 0)
        {
            Field(root, "creative.arrangement.plan-path", library_path);
            CHECK(
                !gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "creative.arrangement.approval"))));
        }
    }
    else if (strcmp(mode, "retained-control") == 0)
    {
        GtkWidget *button = g_object_ref(Find(root, "creative.arrangement.save-plan"));
        GWeakRef weak;
        g_weak_ref_init(&weak, G_OBJECT(root));
        g_object_unref(root);
        root = NULL;
        GObject *remaining = g_weak_ref_get(&weak);
        CHECK(remaining == NULL);
        g_weak_ref_clear(&weak);
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
        CHECK(!umi_fs_exists(plan_path));
    }
    else if (strcmp(mode, "add") != 0)
    {
        Click(root, "creative.arrangement.render");
        if (strcmp(mode, "cancelled-render") == 0)
            Click(root, "creative.arrangement.cancel");
        if (strcmp(mode, "close-busy") == 0)
        {
            GWeakRef weak;
            g_weak_ref_init(&weak, G_OBJECT(root));
            g_object_unref(root);
            root = NULL;
            GObject *remaining = g_weak_ref_get(&weak);
            CHECK(remaining == NULL);
            g_weak_ref_clear(&weak);
            gint64 deadline = g_get_monotonic_time() + 250000;
            while (g_get_monotonic_time() < deadline)
            {
                while (g_main_context_iteration(NULL, FALSE))
                {
                }
                g_usleep(1000U);
            }
            CHECK(!umi_fs_exists(output));
        }
        else
        {
            Wait(root);
            (void)Snapshot(root, &rendered);
            CHECK(rendered == (strcmp(mode, "cancelled-render") != 0));
            if (strcmp(mode, "render") == 0)
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "creative.arrangement.status"))),
                             "Clipped samples: 0") != NULL);
            if (strcmp(mode, "export") == 0 || strcmp(mode, "existing-export") == 0)
            {
                if (strcmp(mode, "existing-export") == 0)
                    CHECK(UmiRootedFileWrite(directory, "mix.wav", "keep", 4U) == UMI_STATUS_OK);
                Click(root, "creative.arrangement.save-wave");
                Wait(root);
                size_t size = 0U;
                unsigned char *bytes = FixtureRead(output, &size);
                if (strcmp(mode, "existing-export") == 0)
                    CHECK(size == 4U && memcmp(bytes, "keep", 4U) == 0);
                else
                    CHECK(size == 44U + 128U * 4U && memcmp(bytes, "RIFF", 4U) == 0 && bytes[44] == 128U &&
                          bytes[45] == 62U);
                free(bytes);
                (void)Snapshot(root, &rendered);
                CHECK(rendered);
            }
            else if (strcmp(mode, "edit-invalidates") == 0)
            {
                Spin(root, "creative.arrangement.gain", 500);
                Click(root, "creative.arrangement.replace");
                (void)Snapshot(root, &rendered);
                CHECK(!rendered && !gtk_widget_get_sensitive(Find(root, "creative.arrangement.save-wave")));
            }
            else if (strcmp(mode, "library-invalidates") == 0)
            {
                Click(root, "creative.arrangement.load-library");
                Wait(root);
                (void)Snapshot(root, &rendered);
                CHECK(!rendered);
            }
            else if (strcmp(mode, "invalid-retains") == 0)
            {
                Spin(root, "creative.arrangement.begin", 16);
                Click(root, "creative.arrangement.replace");
                CHECK(Snapshot(root, &rendered).clips[0].source_begin_ms == 0U && rendered);
            }
        }
    }
    if (root != NULL)
        g_object_unref(root);
    return 0;
}
