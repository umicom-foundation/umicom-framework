/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_action_group_gtk4.c
 *
 * PURPOSE:
 *   Verify wrapping, command identity, activation and ownership admission in native GTK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/action_group.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed=1; goto done; } } while (0)
static void Pump(void)
{
    gint64 until = g_get_monotonic_time() + G_TIME_SPAN_SECOND / 2;
    while (g_get_monotonic_time() < until) {
        for (unsigned i=0U; i<32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(10000U);
    }
}
static void Count(GtkButton *button, gpointer context)
{
    (void)button;
    ++*(unsigned *)context;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode=argv[1];
    if (strcmp(mode,"layout") && strcmp(mode,"activation") &&
        strcmp(mode,"admission") && strcmp(mode,"capacity")) return 2;
    if (!gtk_init_check()) return 77;
    int failed=0;
    unsigned calls=0U;
    GtkWidget *window=g_object_ref_sink(gtk_window_new());
    GtkWidget *group=g_object_ref_sink(UmiGtk4ActionGroupCreate());
    GtkWidget *buttons[64]={0};
    GtkWidget *extra=g_object_ref_sink(gtk_button_new_with_label("Extra"));
    GtkWidget *other=g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0));
    const char *labels[]={"Project Settings","Configure","Build","Test","Build and Run",
        "Run current executable","Install","Cancel","Clean","Package"};
    size_t total=!strcmp(mode,"capacity") ? 64U : G_N_ELEMENTS(labels);
    GtkWidget *scroll=gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),group);
    gtk_window_set_child(GTK_WINDOW(window),scroll);
    gtk_window_set_default_size(GTK_WINDOW(window),420,700);
    for (size_t i=0U;i<total;++i) {
        buttons[i]=g_object_ref_sink(gtk_button_new_with_label(labels[i%G_N_ELEMENTS(labels)]));
        g_signal_connect(buttons[i],"clicked",G_CALLBACK(Count),&calls);
        CHECK(UmiGtk4ActionGroupAppend(group,buttons[i])==UMI_STATUS_OK);
        GtkFlowBoxChild *row=gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(group),(int)i);
        CHECK(row!=NULL && gtk_flow_box_child_get_child(row)==buttons[i]);
    }
    CHECK(calls==0U);
    if (!strcmp(mode,"capacity")) {
        CHECK(UmiGtk4ActionGroupAppend(group,extra)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(gtk_widget_get_parent(extra)==NULL);
    } else if (!strcmp(mode,"admission")) {
        CHECK(UmiGtk4ActionGroupAppend(other,extra)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_widget_get_parent(extra)==NULL);
        CHECK(UmiGtk4ActionGroupAppend(group,buttons[0])==UMI_STATUS_INVALID_ARGUMENT);
        gtk_box_append(GTK_BOX(other),extra);
        CHECK(UmiGtk4ActionGroupAppend(group,extra)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_widget_get_parent(extra)==other);
        CHECK(UmiGtk4ActionGroupAppend(group,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else {
        gtk_window_present(GTK_WINDOW(window)); Pump();
        CHECK(gtk_widget_get_mapped(window));
        CHECK(gtk_widget_get_width(window)<800);
        CHECK(gtk_widget_grab_focus(buttons[3]));
        GtkWidget *focus=gtk_window_get_focus(GTK_WINDOW(window));
        CHECK(focus!=NULL && (focus==buttons[3] || gtk_widget_is_ancestor(focus,buttons[3])));
        graphene_rect_t first, last;
        CHECK(gtk_widget_compute_bounds(buttons[0],group,&first));
        CHECK(gtk_widget_compute_bounds(buttons[total-1U],group,&last));
        CHECK(last.origin.y>first.origin.y);
        for (size_t i=0U;i<total;++i) {
            graphene_rect_t bounds;
            CHECK(gtk_widget_compute_bounds(buttons[i],group,&bounds));
            CHECK(bounds.size.width>0 && bounds.origin.x>=-1.0F);
            CHECK(bounds.origin.x+bounds.size.width<=(float)gtk_widget_get_width(group)+1.0F);
        }
        int narrowWidth=gtk_widget_get_width(group);
        gtk_window_set_default_size(GTK_WINDOW(window),920,700); Pump();
        CHECK(gtk_widget_get_width(group)>narrowWidth);
        graphene_rect_t widerLast;
        CHECK(gtk_widget_compute_bounds(buttons[total-1U],group,&widerLast));
        CHECK(widerLast.origin.y<last.origin.y);
        CHECK(gtk_window_get_focus(GTK_WINDOW(window))==focus);
        for (size_t i=0U;i<total;++i) {
            GtkFlowBoxChild *row=gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(group),(int)i);
            CHECK(row!=NULL && gtk_flow_box_child_get_child(row)==buttons[i]);
        }
        CHECK(calls==0U);
        if (!strcmp(mode,"activation")) {
            g_signal_emit_by_name(buttons[3],"clicked");
            CHECK(calls==1U);
            /* The container never dispatches a second application command. */
            g_signal_emit_by_name(group,"child-activated",
                gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(group),3));
            CHECK(calls==1U);
        }
    }
done:
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window); g_object_unref(group); g_object_unref(other);
    for (size_t i=0U;i<G_N_ELEMENTS(buttons);++i) g_clear_object(&buttons[i]);
    g_object_unref(extra);
    return failed;
}
