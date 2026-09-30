/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_interest/test_native.c
 * PURPOSE: Exercise actual banking forms with a volatile service and no presented windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/bank_operations_gtk4.c"
#include "fixture.h"
/* As in the existing native review tests, the actual adapter is compiled here
 * once. The persistent launcher is never invoked; no user database is opened. */
static BankUi *Create(GtkWidget **window)
{
    Fixture f={0}; OK(UmiBankOperationsOpenMemory(&f.bank)); Setup(&f);
    BankUi *ui=g_new0(BankUi,1); ui->operations=f.bank;
    *window=gtk_window_new(); g_object_ref_sink(*window); ui->window=GTK_WINDOW(*window);
    g_object_set_data_full(G_OBJECT(*window),"umicom-bank-operations",ui,UiFree);
    g_signal_connect(*window,"destroy",G_CALLBACK(UiClosed),ui);
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0); gtk_window_set_child(ui->window,box);
    const char *actions[UMI_BANK_ACTION_LAST+1U];
    for(unsigned i=0;i<(unsigned)UMI_BANK_ACTION_LAST;++i) actions[i]=UmiBankActionName((UmiBankAction)(i+1U));
    actions[UMI_BANK_ACTION_LAST]=NULL;
    const char *actors[]={"maker","checker","operator",NULL}; const char *states[]={"active","blocked","closed",NULL};
    ui->action=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(actions));
    ui->identity=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(actors)); ui->recordState=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(states));
    GtkEntry **entries[]={&ui->request,&ui->id,&ui->owner,&ui->source,&ui->destination,&ui->name,&ui->minor,&ui->currency,&ui->scale,&ui->date,
        &ui->interestRate,&ui->interestDays,&ui->interestBasis,&ui->statementAccount,&ui->statementFirst,&ui->statementLast};
    for(size_t i=0;i<G_N_ELEMENTS(entries);++i){*entries[i]=GTK_ENTRY(gtk_entry_new()); gtk_box_append(GTK_BOX(box),GTK_WIDGET(*entries[i]));
        g_signal_connect_object(*entries[i],"changed",G_CALLBACK(ReviewEdited),G_OBJECT(ui->window),0);}
    ui->submit=GTK_BUTTON(gtk_button_new()); ui->message=GTK_LABEL(gtk_label_new(NULL)); ui->summary=GTK_LABEL(gtk_label_new(NULL)); ui->hint=GTK_LABEL(gtk_label_new(NULL));
    ui->pages=GTK_NOTEBOOK(gtk_notebook_new()); ui->customers=Page(ui->pages,"Customers"); ui->accounts=Page(ui->pages,"Accounts");
    ui->transfers=Page(ui->pages,"Transfers"); ui->cards=Page(ui->pages,"Cards"); ui->ledger=Page(ui->pages,"Ledger");
    ui->reconciliation=Page(ui->pages,"Reconciliation"); ui->audit=Page(ui->pages,"Audit");
    GtkWidget *view=gtk_text_view_new(); ui->reviewBuffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)); gtk_notebook_append_page(ui->pages,view,gtk_label_new("Review"));
    ui->interestRequests=Page(ui->pages,"Interest"); view=gtk_text_view_new(); ui->statementBuffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    ui->statementPage=gtk_notebook_append_page(ui->pages,view,gtk_label_new("Statement"));
    GtkWidget *widgets[]={GTK_WIDGET(ui->action),GTK_WIDGET(ui->identity),GTK_WIDGET(ui->recordState),GTK_WIDGET(ui->submit),GTK_WIDGET(ui->message),GTK_WIDGET(ui->summary),GTK_WIDGET(ui->hint),GTK_WIDGET(ui->pages)};
    for(size_t i=0;i<G_N_ELEMENTS(widgets);++i) gtk_box_append(GTK_BOX(box),widgets[i]);
    g_signal_connect_object(ui->identity,"notify::selected",G_CALLBACK(ReviewSelectionChanged),G_OBJECT(ui->window),0);
    Refresh(ui); return ui;
}
static void Action(BankUi *ui,UmiBankAction action,guint actor)
{
    gtk_drop_down_set_selected(ui->action,(guint)action-1U); gtk_drop_down_set_selected(ui->identity,actor);
    ActionChanged(NULL,NULL,ui->window);
    gtk_editable_set_text(GTK_EDITABLE(ui->id),"interest");
    gtk_editable_set_text(GTK_EDITABLE(ui->date),"2026-09-30");
    if(action==UMI_BANK_INTEREST_SUBMIT){
        gtk_editable_set_text(GTK_EDITABLE(ui->owner),"2026-09"); gtk_editable_set_text(GTK_EDITABLE(ui->source),"account");
        gtk_editable_set_text(GTK_EDITABLE(ui->interestRate),"500"); gtk_editable_set_text(GTK_EDITABLE(ui->interestDays),"30"); gtk_editable_set_text(GTK_EDITABLE(ui->interestBasis),"365");
    }
}
static void Apply(BankUi *ui)
{ ReviewCommand(NULL,ui->window); CHECK(ui->review!=NULL); Submit(NULL,ui->window); CHECK(ui->review==NULL); }
static bool Contains(GtkTextBuffer *buffer,const char *needle)
{ GtkTextIter start,end; gtk_text_buffer_get_bounds(buffer,&start,&end); char *text=gtk_text_buffer_get_text(buffer,&start,&end,FALSE); bool found=strstr(text,needle)!=NULL; g_free(text); return found; }
int main(int argc,char **argv)
{
    CHECK(argc==2); if(!gtk_init_check())return 77;
    GtkWidget *window; BankUi *ui=Create(&window); Fixture f={0}; f.bank=ui->operations;
    Action(ui,UMI_BANK_INTEREST_SUBMIT,0);
    if(strcmp(argv[1],"form")==0){
        UmiBankActor actor; UmiBankCommand c; CHECK(ReadCommand(ui,&actor,&c)); CHECK(c.interest.annualRateBps==500 && c.interest.days==30 && c.interest.dayCountBasis==365);
        CHECK(gtk_widget_get_sensitive(GTK_WIDGET(ui->interestRate)) && !gtk_widget_get_sensitive(GTK_WIDGET(ui->minor)));
        ReviewCommand(NULL,window); CHECK(ui->review!=NULL && Contains(ui->reviewBuffer,"GBP 4.10")); Balance(&f,100000,0);
    }else if(strcmp(argv[1],"invalidation")==0){
        ReviewCommand(NULL,window); CHECK(ui->review!=NULL); gtk_editable_set_text(GTK_EDITABLE(ui->interestDays),"31");
        CHECK(ui->review==NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit))); Submit(NULL,window); Balance(&f,100000,0);
    }else if(strcmp(argv[1],"invalid")==0){
        gtk_editable_set_text(GTK_EDITABLE(ui->interestRate),"99999999999999999999999999999"); ReviewCommand(NULL,window); CHECK(ui->review==NULL); Balance(&f,100000,0);
    }else if(strcmp(argv[1],"close")==0){
        ReviewCommand(NULL,window); CHECK(ui->review!=NULL); GtkWidget *retained=g_object_ref(GTK_WIDGET(ui->interestRate));
        gtk_window_destroy(GTK_WINDOW(window)); CHECK(ui->closed && ui->operations==NULL && ui->review==NULL);
        gtk_editable_set_text(GTK_EDITABLE(retained),"1000"); ReviewCommand(NULL,window); Submit(NULL,window); g_object_unref(retained);
    }else{
        Apply(ui); Action(ui,UMI_BANK_INTEREST_APPROVE,0); ReviewCommand(NULL,window); CHECK(ui->review==NULL);
        Action(ui,UMI_BANK_INTEREST_APPROVE,1); Apply(ui); Action(ui,UMI_BANK_INTEREST_POST,2); Apply(ui); Balance(&f,100410,0);
        if(strcmp(argv[1],"reverse")==0){Action(ui,UMI_BANK_INTEREST_REVERSE,2); Apply(ui); Balance(&f,100000,0);}
        else if(strcmp(argv[1],"statement")==0){
            gtk_editable_set_text(GTK_EDITABLE(ui->statementAccount),"account"); gtk_editable_set_text(GTK_EDITABLE(ui->statementFirst),"6");
            gtk_editable_set_text(GTK_EDITABLE(ui->statementLast),"6"); ShowStatement(NULL,window);
            CHECK(Contains(ui->statementBuffer,"Opening: GBP 1000.00") && Contains(ui->statementBuffer,"Closing: GBP 1004.10"));
            gtk_editable_set_text(GTK_EDITABLE(ui->statementLast),"5"); ShowStatement(NULL,window); CHECK(!Contains(ui->statementBuffer,"Closing:")); Balance(&f,100410,0);
        }else CHECK(strcmp(argv[1],"posting")==0);
    }
    /* The compressed teardown made the unconditional release look guarded.
     * The explicit scopes below replace it while retaining the original
     * implementation here for engineering review. */
#if 0
    if(!ui->closed) gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window); return 0;
#endif
    /* A callback may already have closed the window. Destroy it only when
     * needed, then always release the reference owned by this test. */
    if (!ui->closed) {
        gtk_window_destroy(GTK_WINDOW(window));
    }
    g_object_unref(window);
    return 0;
}
