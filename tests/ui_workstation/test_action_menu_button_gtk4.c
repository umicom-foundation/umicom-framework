/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_action_menu_button_gtk4.c
 *
 * PURPOSE:
 *   Verify shortcut rows preserve search, activation, text and native ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/action_menu.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); failed=1; goto cleanup; } } while(0)
static void Pump(void) { for(unsigned i=0;i<512U && g_main_context_pending(NULL);++i) (void)g_main_context_iteration(NULL,FALSE); }
static void Clicked(GtkButton *button,gpointer data) { (void)button; ++*(unsigned *)data; }
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    const char *mode=argv[1],*cases[]={"ordinary","unicode","shortcut","function-key","no-shortcut","append-search","changed-child","relabel","retained-button","activation","hidden","null-output","invalid-label","label-limit","label-over","invalid-utf8","invalid-shortcut","modifier-without-key"};
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i) if(!strcmp(mode,cases[i])) known=1;
    if(!known) return 2;
    if(!gtk_init_check()) return 77;
    int failed=0;
    unsigned invoked=0U;
    GtkWidget *button=NULL,*menu=NULL;
    gchar *large=NULL;
    if(!strcmp(mode,"null-output")) {
        CHECK(UmiGtk4ActionMenuButtonCreate("Save",GDK_KEY_s,GDK_CONTROL_MASK,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    const char *label=!strcmp(mode,"unicode")?"Enregistrer édition":"Save <draft> & continue";
    guint key=GDK_KEY_s;
    GdkModifierType modifiers=GDK_CONTROL_MASK;
    if(!strcmp(mode,"invalid-label")) label="";
    if(!strcmp(mode,"invalid-utf8")) label="\xff";
    if(!strcmp(mode,"label-limit")||!strcmp(mode,"label-over")) {
        large=g_strnfill(!strcmp(mode,"label-limit")?512U:513U,'s');
        label=large;
    }
    if(!strcmp(mode,"no-shortcut")) { key=0U;modifiers=0; }
    if(!strcmp(mode,"function-key")) { key=GDK_KEY_F5;modifiers=0; }
    if(!strcmp(mode,"invalid-shortcut")) { key=GDK_KEY_Shift_L;modifiers=0; }
    if(!strcmp(mode,"modifier-without-key")) key=0U;
    UmiStatus status=UmiGtk4ActionMenuButtonCreate(label,key,modifiers,&button);
    if(!strncmp(mode,"invalid-",8U)||!strcmp(mode,"label-over")||!strcmp(mode,"modifier-without-key")) {
        CHECK(status==UMI_STATUS_INVALID_ARGUMENT && button==NULL); goto cleanup;
    }
    CHECK(status==UMI_STATUS_OK && GTK_IS_BUTTON(button));
    g_object_ref_sink(button);
    CHECK(gtk_widget_get_parent(button)==NULL);
    GtkWidget *row=gtk_button_get_child(GTK_BUTTON(button));
    GtkWidget *caption=gtk_widget_get_first_child(row);
    CHECK(GTK_IS_LABEL(caption) && !strcmp(gtk_label_get_text(GTK_LABEL(caption)),label));
    CHECK(!gtk_label_get_use_markup(GTK_LABEL(caption)));
    CHECK(gtk_label_get_xalign(GTK_LABEL(caption))==0.0F);
    GtkWidget *shortcut=gtk_widget_get_next_sibling(caption);
    if(key==0U) CHECK(shortcut==NULL);
    else {
        guint parsed=0U;GdkModifierType parsedMods=0;
        CHECK(GTK_IS_SHORTCUT_LABEL(shortcut));
        CHECK(gtk_accelerator_parse(gtk_shortcut_label_get_accelerator(GTK_SHORTCUT_LABEL(shortcut)),&parsed,&parsedMods));
        CHECK(parsed==key && parsedMods==modifiers);
    }
    g_signal_connect(button,"clicked",G_CALLBACK(Clicked),&invoked);
    CHECK(UmiGtk4ActionMenuCreate("File",&menu)==UMI_STATUS_OK);
    g_object_ref_sink(menu);
    if(!strcmp(mode,"changed-child")) {
        gtk_button_set_child(GTK_BUTTON(button),gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0));
        CHECK(UmiGtk4ActionMenuAppend(menu,button,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_widget_get_parent(button)==NULL);goto cleanup;
    }
    if(!strcmp(mode,"relabel")) gtk_button_set_label(GTK_BUTTON(button),"Export archive");
    CHECK(UmiGtk4ActionMenuAppend(menu,button,"document operation")==UMI_STATUS_OK);
    Pump();
    CHECK(UmiGtk4ActionMenuVisibleCount(menu)==1U && invoked==0U);
    if(!strcmp(mode,"append-search")||!strcmp(mode,"unicode")||!strcmp(mode,"relabel")) {
        const char *query=!strcmp(mode,"relabel")?"EXPORT":!strcmp(mode,"unicode")?"ÉDITION":"<DRAFT>";
        CHECK(UmiGtk4ActionMenuSetFilter(menu,query)==UMI_STATUS_OK);Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu)==1U && invoked==0U);
        CHECK(UmiGtk4ActionMenuSetFilter(menu,"not in this action")==UMI_STATUS_OK);Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu)==0U && invoked==0U);
    }
    if(!strcmp(mode,"hidden")) {
        gtk_widget_set_visible(button,FALSE);Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu)==0U);
        gtk_widget_set_visible(button,TRUE);Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu)==1U);
    }
    if(!strcmp(mode,"retained-button")) {
        g_clear_object(&menu);Pump();
        CHECK(GTK_IS_BUTTON(button) && gtk_widget_get_parent(button)==NULL);
        CHECK(GTK_IS_LABEL(gtk_widget_get_first_child(gtk_button_get_child(GTK_BUTTON(button)))));
    }
    if(!strcmp(mode,"activation")||!strcmp(mode,"retained-button")) {
        g_signal_emit_by_name(button,"clicked");CHECK(invoked==1U);
    }
cleanup:
    g_clear_object(&menu);g_clear_object(&button);g_free(large);Pump();
    return failed;
}
