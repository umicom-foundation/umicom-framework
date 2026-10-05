/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_song_video_composition_gtk4.c
 * PURPOSE: Check Media and Music page composition from the same Framework owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_workspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag && !strcmp(tag, id))
        return root;
    for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c))
    {
        GtkWidget *found = Find(c, id);
        if (found)
            return found;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2 && (!strcmp(argv[1], "media") || !strcmp(argv[1], "music")));
    if (!gtk_init_check())
        return 77;
    UmiDataServer *server = NULL;
    UmiCreativeGtkPanel *panel = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiCreativeGtkPanelCreate(server, argv[1], &panel) == UMI_STATUS_OK);
    GtkWidget *root = UmiCreativeGtkPanelWidget(panel);
    CHECK(Find(root, "song-plan.panel") && Find(root, "pixverse.panel"));
    CHECK(Find(root, "creative.canvas") && umi_data_server_count(server) == 0U);
    UmiCreativeGtkPanelDestroy(panel);
    umi_data_server_destroy(server);
    return 0;
}
