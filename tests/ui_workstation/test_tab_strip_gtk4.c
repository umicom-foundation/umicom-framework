/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_tab_strip_gtk4.c
 *
 * PURPOSE:
 *   Verify stable tab identity, focus, overflow and retired-owner rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/tab_strip.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed=1; goto done; } } while (0)
typedef struct Calls { unsigned count; unsigned released; char id[32]; } Calls;
static void Activate(const char *id, void *context)
{
    Calls *calls = context; ++calls->count; g_strlcpy(calls->id, id, sizeof calls->id);
}
static void Release(gpointer context) { ++((Calls *)context)->released; }
static void Invalidate(GObject *object, GParamSpec *spec, gpointer context)
{
    (void)object; (void)spec; UmiGtk4TabStripInvalidate(context);
}
static void Reenter(GObject *object, GParamSpec *spec, gpointer context)
{
    (void)object; (void)spec;
    UmiStatus *status = g_object_get_data(G_OBJECT(context), "test.reentry-status");
    *status = UmiGtk4TabStripSync(context, NULL, 0U);
}
static void DetachAnchor(GObject *object, GParamSpec *spec, gpointer context)
{
    (void)spec; (void)context;
    GtkWidget *box = gtk_widget_get_parent(GTK_WIDGET(object));
    if (GTK_IS_BOX(box)) {
        GtkWidget *anchor = gtk_widget_get_first_child(box);
        if (GTK_IS_LABEL(anchor)) gtk_box_remove(GTK_BOX(box), anchor);
    }
}
static int PumpMapped(GtkWidget *window)
{
    gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (g_get_monotonic_time() < end) {
        for (unsigned i=0U; i<32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        if (gtk_widget_get_mapped(window) && gtk_widget_get_width(window)>0) return 1;
        g_usleep(10000U);
    }
    return 0;
}
/* Pump for a bounded interval even if another UI source stays continuously ready. */
static void PumpFor(gint64 microseconds)
{
    gint64 end = g_get_monotonic_time() + microseconds;
    do {
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(10000U);
    } while (g_get_monotonic_time() < end);
}
static int WaitVisible(GtkWidget *strip, GtkWidget *button)
{
    gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    do {
        PumpFor(10000);
        GtkWidget *box = gtk_widget_get_parent(button);
        GtkAdjustment *adjustment = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(strip));
        graphene_rect_t bounds;
        double value = gtk_adjustment_get_value(adjustment);
        double page = gtk_adjustment_get_page_size(adjustment);
        if (box != NULL && gtk_widget_get_width(button) > 0 && page > 0.0 &&
            gtk_widget_compute_bounds(button, box, &bounds) &&
            (double)bounds.origin.x >= value - 1.0 &&
            (double)bounds.origin.x + (double)bounds.size.width <= value + page + 1.0)
            return 1;
    } while (g_get_monotonic_time() < end);
    fprintf(stderr, "Selected tab did not enter the visible viewport within five seconds.\n");
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    const char *mode=argv[1];
    if (strcmp(mode,"identity") && strcmp(mode,"invalid") && strcmp(mode,"retired") &&
        strcmp(mode,"reentrant") && strcmp(mode,"reparented") && strcmp(mode,"overflow") && strcmp(mode,"reveal")) return 2;
    int failed=0;
    Calls calls={0};
    GtkWidget *window=g_object_ref_sink(gtk_window_new());
    GtkWidget *strip=g_object_ref_sink(UmiGtk4TabStripCreate(Activate,&calls,Release));
    GtkWidget *first=NULL,*second=NULL;
    gtk_window_set_default_size(GTK_WINDOW(window),420,120);
    gtk_window_set_child(GTK_WINDOW(window),strip);
    UmiGtk4TabStripItem items[]={{"one","first.c","/project/first.c",1},
        {"two","second.c","/project/second.c",0}};
    CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_OK);
    GtkWidget *borrowedFirst=UmiGtk4TabStripFind(strip,"one");
    GtkWidget *borrowedSecond=UmiGtk4TabStripFind(strip,"two");
    CHECK(GTK_IS_BUTTON(borrowedFirst) && GTK_IS_BUTTON(borrowedSecond));
    first=g_object_ref(borrowedFirst);second=g_object_ref(borrowedSecond);
    if (!strcmp(mode,"identity")) {
        gtk_window_present(GTK_WINDOW(window)); CHECK(PumpMapped(window));
        CHECK(gtk_widget_grab_focus(first));
        for (unsigned tick=0U; tick<20U; ++tick)
            CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_OK);
        CHECK(UmiGtk4TabStripFind(strip,"one")==first);
        CHECK(gtk_window_get_focus(GTK_WINDOW(window))==first);
        items[0].title="first.c ●"; items[0].selected=0; items[1].selected=1;
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_OK);
        CHECK(UmiGtk4TabStripFind(strip,"one")==first);
        CHECK(gtk_window_get_focus(GTK_WINDOW(window))==first);
        UmiGtk4TabStripItem reordered[]={items[1],items[0]};
        CHECK(UmiGtk4TabStripSync(strip,reordered,2U)==UMI_STATUS_OK);
        CHECK(gtk_widget_get_next_sibling(second)==first);
        CHECK(calls.count==0U);
        g_signal_emit_by_name(first,"clicked"); CHECK(calls.count==1U && !strcmp(calls.id,"one"));
        CHECK(UmiGtk4TabStripSync(strip,&items[1],1U)==UMI_STATUS_OK);
        g_signal_emit_by_name(first,"clicked"); CHECK(calls.count==1U);
        CHECK(gtk_widget_get_parent(first)==NULL && UmiGtk4TabStripFind(strip,"two")==second);
        CHECK(UmiGtk4TabStripSync(strip,NULL,0U)==UMI_STATUS_OK);
        CHECK(UmiGtk4TabStripFind(strip,"two")==NULL);
    } else if (!strcmp(mode,"invalid")) {
        items[1].id="one";
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_INVALID_ARGUMENT);
        items[1].id="two";items[1].selected=1;
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_INVALID_ARGUMENT);
        items[1].selected=0;items[1].title="\xff";
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TabStripSync(strip,NULL,2U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TabStripSync(strip,items,UMI_GTK4_TAB_STRIP_MAX+1U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiGtk4TabStripFind(strip,"one")==first && UmiGtk4TabStripFind(strip,"two")==second);
        CHECK(!strcmp(gtk_button_get_label(GTK_BUTTON(first)),"first.c"));
    } else if (!strcmp(mode,"retired")) {
        g_signal_connect(first,"notify::label",G_CALLBACK(Invalidate),strip);
        items[0].title="changed.c";
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_CANCELLED);
        g_signal_emit_by_name(first,"clicked"); CHECK(calls.count==0U);
        UmiGtk4TabStripInvalidate(strip);
        CHECK(UmiGtk4TabStripSync(strip,NULL,0U)==UMI_STATUS_CANCELLED);
        CHECK(calls.released==0U);
    } else if (!strcmp(mode,"reparented")) {
        g_signal_connect(first,"notify::label",G_CALLBACK(DetachAnchor),strip);
        items[0].title="changed.c";
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_CANCELLED);
        g_signal_emit_by_name(first,"clicked");
        CHECK(calls.count==0U);
        CHECK(UmiGtk4TabStripFind(strip,"one")==NULL);
    } else if (!strcmp(mode,"reentrant")) {
        UmiStatus reentry=UMI_STATUS_OK;
        g_object_set_data(G_OBJECT(strip),"test.reentry-status",&reentry);
        gulong handler=g_signal_connect(first,"notify::label",G_CALLBACK(Reenter),strip);
        items[0].title="changed.c";
        CHECK(UmiGtk4TabStripSync(strip,items,2U)==UMI_STATUS_OK);
        g_signal_handler_disconnect(first,handler);
        CHECK(reentry==UMI_STATUS_BUSY && UmiGtk4TabStripFind(strip,"two")==second);
    } else if (!strcmp(mode,"reveal")) {
        /* Editor focus stays outside the scroller. Keyboard selection or opening
         * a document must reveal its tab without requiring a click on that tab. */
        GtkWidget *column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        GtkWidget *editor = gtk_entry_new();
        gtk_window_set_child(GTK_WINDOW(window), NULL);
        gtk_box_append(GTK_BOX(column), strip);
        gtk_box_append(GTK_BOX(column), editor);
        gtk_window_set_child(GTK_WINDOW(window), column);
        UmiGtk4TabStripItem many[24]; char ids[24][24];
        for (size_t i=0U; i<24U; ++i) {
            g_snprintf(ids[i], sizeof ids[i], "source-%zu.c", i);
            many[i]=(UmiGtk4TabStripItem){ids[i],ids[i],ids[i],i==23U};
        }
        CHECK(UmiGtk4TabStripSync(strip,many,24U)==UMI_STATUS_OK);
        gtk_window_present(GTK_WINDOW(window)); CHECK(PumpMapped(window));
        CHECK(gtk_widget_grab_focus(editor));
        GtkWidget *last=UmiGtk4TabStripFind(strip,ids[23]);
        GtkWidget *leading=UmiGtk4TabStripFind(strip,ids[0]);
        CHECK(last!=NULL && leading!=NULL);
        CHECK(WaitVisible(strip,last));
        GtkWidget *focus=gtk_window_get_focus(GTK_WINDOW(window));
        CHECK(focus==editor || (focus!=NULL && gtk_widget_is_ancestor(focus,editor)));
        GtkAdjustment *adjustment=gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(strip));
        CHECK(gtk_adjustment_get_value(adjustment)>0.0 && calls.count==0U);
        gtk_adjustment_set_value(adjustment,0.0);
        for (unsigned refresh=0U; refresh<5U; ++refresh)
            CHECK(UmiGtk4TabStripSync(strip,many,24U)==UMI_STATUS_OK);
        PumpFor(250000);
        CHECK(gtk_adjustment_get_value(adjustment)==0.0);
        CHECK(UmiGtk4TabStripFind(strip,ids[23])==last);
        many[23].selected=0; many[0].selected=1;
        gtk_adjustment_set_value(adjustment,gtk_adjustment_get_upper(adjustment));
        CHECK(UmiGtk4TabStripSync(strip,many,24U)==UMI_STATUS_OK);
        CHECK(WaitVisible(strip,leading));
        CHECK(gtk_adjustment_get_value(adjustment)<=1.0);
        many[0].selected=0; many[23].selected=1;
        CHECK(UmiGtk4TabStripSync(strip,many,24U)==UMI_STATUS_OK);
        UmiGtk4TabStripInvalidate(strip);
        PumpFor(250000);
        CHECK(gtk_adjustment_get_value(adjustment)<=1.0 && calls.count==0U);
        g_signal_emit_by_name(last,"clicked"); CHECK(calls.count==0U);
    } else {
        UmiGtk4TabStripItem many[24]; char ids[24][24];
        for (size_t i=0U;i<24U;++i) {
            g_snprintf(ids[i],sizeof ids[i],"source-%zu.c",i);
            many[i]=(UmiGtk4TabStripItem){ids[i],ids[i],ids[i],i==23U};
        }
        CHECK(UmiGtk4TabStripSync(strip,many,24U)==UMI_STATUS_OK);
        gtk_window_present(GTK_WINDOW(window)); CHECK(PumpMapped(window));
        GtkAdjustment *adjustment=gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(strip));
        CHECK(gtk_adjustment_get_upper(adjustment)>gtk_adjustment_get_page_size(adjustment));
        CHECK(gtk_widget_get_width(window)<1000);
        GtkWidget *last=UmiGtk4TabStripFind(strip,ids[23]); CHECK(last!=NULL);
        CHECK(gtk_widget_grab_focus(last));
        CHECK(UmiGtk4TabStripSync(strip,many,24U)==UMI_STATUS_OK);
        CHECK(UmiGtk4TabStripFind(strip,ids[23])==last);
    }
done:
    if(first!=NULL)g_signal_handlers_disconnect_by_data(first,strip);
    UmiGtk4TabStripInvalidate(strip);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);g_object_unref(strip);
    if(calls.released!=1U)failed=1;
    /* A retained control outlives its container safely; no caller data access. */
    if(first!=NULL)g_signal_emit_by_name(first,"clicked");
    g_clear_object(&first);g_clear_object(&second);
    return failed;
}
