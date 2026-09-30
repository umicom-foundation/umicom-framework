/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_review/test_gtk_review.c
 * PURPOSE:
 *   Compile the actual adapter once into a white-box unit host. Never click the persistent
 *   launcher: the test supplies the canonical memory service instead.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compile the actual adapter once into a white-box unit host. Never click the
 * persistent launcher: the test supplies the canonical memory service instead.
 */
#include "../../adapters/gtk4/bank_operations_gtk4.c"
#include "fixture.h"
static BankUi *Create(GtkWidget **outWindow)
{
    Fixture f = {0};
    BankUi *ui = g_new0(BankUi, 1);
    static const char *const actions[] = {"create",NULL};
    static const char *const actors[] = {"maker","checker","operator",NULL};
    static const char *const states[] = {"active","blocked","closed",NULL};
    OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);ui->operations=f.bank;
    *outWindow=gtk_window_new();g_object_ref_sink(*outWindow);ui->window=GTK_WINDOW(*outWindow);
    g_object_set_data_full(G_OBJECT(*outWindow),"umicom-bank-operations",ui,UiFree);
/* Exercise the production close observer while retaining the unpresented window. The previous implementation remains for engineering review. */
#if 0
    g_signal_connect(*outWindow,"destroy",G_CALLBACK(UiClosed),ui);
#endif
    UmiGtk4ObserveWindowRemoval(GTK_WINDOW(*outWindow), G_OBJECT(*outWindow), UiClosed, ui);
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);gtk_window_set_child(ui->window,box);
    ui->action=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(actions));
    ui->identity=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(actors));
    ui->recordState=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(states));
    GtkEntry **entries[]={&ui->request,&ui->id,&ui->owner,&ui->source,&ui->destination,&ui->name,&ui->minor,&ui->currency,&ui->scale,&ui->date};
    for(size_t i=0;i<G_N_ELEMENTS(entries);++i){*entries[i]=GTK_ENTRY(gtk_entry_new());gtk_box_append(GTK_BOX(box),GTK_WIDGET(*entries[i]));}
    GtkWidget *view=gtk_text_view_new();ui->reviewBuffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));gtk_box_append(GTK_BOX(box),view);
    ui->submit=GTK_BUTTON(gtk_button_new());ui->message=GTK_LABEL(gtk_label_new(NULL));ui->summary=GTK_LABEL(gtk_label_new(NULL));ui->hint=GTK_LABEL(gtk_label_new(NULL));
    ui->pages=GTK_NOTEBOOK(gtk_notebook_new());
    GtkWidget *widgets[]={GTK_WIDGET(ui->action),GTK_WIDGET(ui->identity),GTK_WIDGET(ui->recordState),GTK_WIDGET(ui->submit),GTK_WIDGET(ui->message),GTK_WIDGET(ui->summary),GTK_WIDGET(ui->hint),GTK_WIDGET(ui->pages)};
    for(size_t i=0;i<G_N_ELEMENTS(widgets);++i)gtk_box_append(GTK_BOX(box),widgets[i]);
    ui->displayedRevision=6;ui->timestampMillis=10;
    return ui;
}
int main(int argc,char **argv)
{
    if(!gtk_init_check())return 77;
    CHECK(argc==2);GtkWidget *window;BankUi *ui=Create(&window);
    Fixture f={0};f.bank=ui->operations;f.maker.capabilities=UMI_BANK_CAP_ALL;Id(&f.maker.id,"maker");f.serial=10;
    UmiBankCommand c=Transfer(&f);OK(UmiBankOperationsReview(ui->operations,&f.maker,&c,&ui->review));
    if(strcmp(argv[1],"invalidate")==0){InvalidateReview(ui);CHECK(ui->review==NULL&&!gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));}
    else if(strcmp(argv[1],"edit")==0){ReviewEdited(NULL,window);CHECK(ui->review==NULL);}
    else if(strcmp(argv[1],"identity")==0){ReviewSelectionChanged(NULL,NULL,window);CHECK(ui->review==NULL);}
    else if(strcmp(argv[1],"no_review")==0){InvalidateReview(ui);Submit(NULL,window);Funds(&f,"payer",100000,0);}
    else if(strcmp(argv[1],"close")==0){GtkWidget *held=GTK_WIDGET(ui->submit);g_object_ref(held);gtk_window_destroy(GTK_WINDOW(window));CHECK(ui->closed&&ui->review==NULL&&ui->operations==NULL);Submit(GTK_BUTTON(held),window);g_object_unref(held);}
    else if(strcmp(argv[1],"independent")==0){GtkWidget *second;BankUi *other=Create(&second);InvalidateReview(other);CHECK(ui->review!=NULL);gtk_window_destroy(GTK_WINDOW(second));g_object_unref(second);}
    else CHECK(0);
    gtk_window_destroy(GTK_WINDOW(window));g_object_unref(window);return 0;
}
