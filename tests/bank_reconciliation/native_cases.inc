/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reconciliation/native_cases.inc
 * PURPOSE: Exercise investigation through the actual Bank form, review page and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

/* Use established form metadata and explicit operator identity selection. */
static void ReconciliationAction(BankUi *ui, UmiBankAction action)
{
    Action(ui, action, 2U);
    gtk_editable_set_text(GTK_EDITABLE(ui->id), "break");
    gtk_editable_set_text(GTK_EDITABLE(ui->owner), "match");
    gtk_editable_set_text(GTK_EDITABLE(ui->name), "Investigated timing difference");
}

/* Compile the production adapter once through the existing native fixture. */
static int VerifyNativeReconciliation(const char *name)
{
    GtkWidget *window; BankUi *ui = Create(&window);
    Fixture f = {0}; f.bank = ui->operations; f.serial = 100;
    Id(&f.operator.id, "test-operator"); f.operator.capabilities = UMI_BANK_CAP_OPERATE | UMI_BANK_CAP_TEST_FUNDING;
    Compare(&f, "break", "account", 90000); Compare(&f, "match", "account", 100000);
    Refresh(ui); ReconciliationAction(ui, UMI_BANK_RECONCILIATION_RESOLVE);
    CHECK(gtk_widget_get_sensitive(GTK_WIDGET(ui->owner)) && gtk_widget_get_sensitive(GTK_WIDGET(ui->name)));
    CHECK(!gtk_widget_get_sensitive(GTK_WIDGET(ui->minor)) && !gtk_widget_get_sensitive(GTK_WIDGET(ui->source)));
    if (strcmp(name, "reconciliation-invalid") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(ui->owner), "break"); ReviewCommand(NULL, window); CHECK(ui->review == NULL);
    } else {
        ReviewCommand(NULL, window); CHECK(ui->review != NULL && Contains(ui->reviewBuffer, "Open break -> Resolved break"));
        CHECK(Contains(ui->reviewBuffer, "Linked comparison match") && Contains(ui->reviewBuffer, "GBP 900.00"));
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED); Balance(&f, 100000, 0);
        if (strcmp(name, "reconciliation-resolve") == 0 || strcmp(name, "reconciliation-reopen") == 0) {
            Submit(NULL, window); CHECK(ui->review == NULL && ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_RESOLVED);
            GtkWidget *label = gtk_grid_get_child_at(ui->reconciliation, 6, 1);
            CHECK(GTK_IS_LABEL(label) && strcmp(gtk_label_get_text(GTK_LABEL(label)), "Resolved break") == 0);
            label = gtk_grid_get_child_at(ui->reconciliation, 5, 1);
            CHECK(GTK_IS_LABEL(label) && strcmp(gtk_label_get_text(GTK_LABEL(label)), "Unmatched") == 0);
            if (strcmp(name, "reconciliation-reopen") == 0) {
                ReconciliationAction(ui, UMI_BANK_RECONCILIATION_REOPEN);
                CHECK(!gtk_widget_get_sensitive(GTK_WIDGET(ui->owner)));
                ReviewCommand(NULL, window); CHECK(ui->review != NULL && Contains(ui->reviewBuffer, "Resolved break -> Reopened break"));
                Submit(NULL, window); CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_REOPENED);
                CHECK(strcmp(ReadBreak(&f).evidenceId.value, "match") == 0);
            }
        } else if (strcmp(name, "reconciliation-edit") == 0) {
            gtk_editable_set_text(GTK_EDITABLE(ui->name), "A revised explanation");
            CHECK(ui->review == NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
            Submit(NULL, window); CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
        } else if (strcmp(name, "reconciliation-stale") == 0) {
            Compare(&f, "newer", "account", 95000);
            Submit(NULL, window); CHECK(ui->review == NULL && ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
            CHECK(strstr(gtk_label_get_text(ui->message), "State changed") != NULL);
        } else if (strcmp(name, "reconciliation-close") == 0) {
            GtkWidget *button = gtk_button_new(), *reason = g_object_ref(GTK_WIDGET(ui->name));
            gtk_box_append(GTK_BOX(gtk_window_get_child(GTK_WINDOW(window))), button); g_object_ref(button);
            g_signal_connect_object(button, "clicked", G_CALLBACK(Submit), G_OBJECT(window), 0);
            gtk_window_destroy(GTK_WINDOW(window)); CHECK(ui->closed && ui->review == NULL && ui->operations == NULL);
            g_signal_emit_by_name(button, "clicked"); gtk_editable_set_text(GTK_EDITABLE(reason), "After close");
            g_object_unref(window); window = NULL; g_signal_emit_by_name(button, "clicked");
            g_object_unref(button); g_object_unref(reason);
        } else return 2;
    }
    if (window != NULL) { Balance(&f, 100000, 0); gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window); }
    return 0;
}
