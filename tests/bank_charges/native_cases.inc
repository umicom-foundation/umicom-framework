/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_charges/native_cases.inc
 * PURPOSE: Exercise charges through the same real GTK form used by the Bank workstation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after the interest acceptance fixture. Both suites share the actual
 * adapter and the same unpresented, memory-only service fixture. */
static void ChargeAction(BankUi *ui,UmiBankAction action,guint actor)
{
    Action(ui,action,actor);
    gtk_editable_set_text(GTK_EDITABLE(ui->id),"native-charge");
    if(action==UMI_BANK_CHARGE_SUBMIT) {
        gtk_editable_set_text(GTK_EDITABLE(ui->owner),"statement-2026-09");
        gtk_editable_set_text(GTK_EDITABLE(ui->source),"account");
        gtk_editable_set_text(GTK_EDITABLE(ui->name),"Practice service charge");
        gtk_editable_set_text(GTK_EDITABLE(ui->minor),"250");
        gtk_editable_set_text(GTK_EDITABLE(ui->currency),"GBP");
        gtk_editable_set_text(GTK_EDITABLE(ui->scale),"2");
    }
}
static int VerifyNativeCharge(const char *name)
{
    GtkWidget *window; BankUi *ui=Create(&window); Fixture f={0}; f.bank=ui->operations;
    ChargeAction(ui,UMI_BANK_CHARGE_SUBMIT,0);
    if(strcmp(name,"charge-form")==0) {
        UmiBankActor actor; UmiBankCommand command; CHECK(ReadCommand(ui,&actor,&command));
        CHECK(command.amount.minor_units==250 && command.interest.days==0 && strcmp(command.name,"Practice service charge")==0);
        CHECK(gtk_widget_get_sensitive(GTK_WIDGET(ui->minor)) && !gtk_widget_get_sensitive(GTK_WIDGET(ui->interestRate)));
        ReviewCommand(NULL,window); CHECK(ui->review!=NULL && Contains(ui->reviewBuffer,"GBP 2.50")); Balance(&f,100000,0);
    } else if(strcmp(name,"charge-edit")==0) {
        ReviewCommand(NULL,window); CHECK(ui->review!=NULL);
        gtk_editable_set_text(GTK_EDITABLE(ui->minor),"251"); CHECK(ui->review==NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
        Submit(NULL,window); UmiBankCounts counts; OK(UmiBankOperationsCounts(ui->operations,&counts)); CHECK(counts.chargeRequests==0); Balance(&f,100000,0);
    } else if(strcmp(name,"charge-invalid")==0) {
        gtk_editable_set_text(GTK_EDITABLE(ui->minor),"18446744073709551616");
        ReviewCommand(NULL,window); CHECK(ui->review==NULL); Balance(&f,100000,0);
    } else if(strcmp(name,"charge-close")==0) {
        ReviewCommand(NULL,window); CHECK(ui->review!=NULL);
        GtkWidget *held=g_object_ref(GTK_WIDGET(ui->minor)); gtk_window_destroy(GTK_WINDOW(window));
        CHECK(ui->closed && ui->operations==NULL && ui->review==NULL);
        gtk_editable_set_text(GTK_EDITABLE(held),"999"); Submit(NULL,window); g_object_unref(held);
    } else {
        Apply(ui); ChargeAction(ui,UMI_BANK_CHARGE_APPROVE,0); ReviewCommand(NULL,window); CHECK(ui->review==NULL);
        ChargeAction(ui,UMI_BANK_CHARGE_APPROVE,1); Apply(ui);
        ChargeAction(ui,UMI_BANK_CHARGE_POST,2); Apply(ui); Balance(&f,99750,0);
        UmiBankChargeRequest charge; OK(UmiBankOperationsChargeAt(ui->operations,0,&charge)); CHECK(charge.state==UMI_BANK_TRANSFER_EXECUTED);
        GtkWidget *amount=gtk_grid_get_child_at(ui->chargeRequests,4,1);
        CHECK(GTK_IS_LABEL(amount) && strcmp(gtk_label_get_text(GTK_LABEL(amount)),"GBP 2.50")==0);
        if(strcmp(name,"charge-reverse")==0) {
            ChargeAction(ui,UMI_BANK_CHARGE_REVERSE,2); Apply(ui); Balance(&f,100000,0);
        } else if(strcmp(name,"charge-statement")==0) {
            gtk_editable_set_text(GTK_EDITABLE(ui->statementAccount),"account");
            gtk_editable_set_text(GTK_EDITABLE(ui->statementFirst),"6"); gtk_editable_set_text(GTK_EDITABLE(ui->statementLast),"6");
            ShowStatement(NULL,window); CHECK(Contains(ui->statementBuffer,"Closing: GBP 997.50"));
        } else CHECK(strcmp(name,"charge-post")==0);
    }
    if(!ui->closed) gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window); return 0;
}
