/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_feed_comparison.inc
 *
 * PURPOSE:
 *   Retain one explicit import for comparison with a later accepted import.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The retained side is independent from the current file-request generation.
 * Changing a path clears only the current import; an explicit Forget action
 * releases the retained side. No hidden import history or filesystem copy is made. */
static void FeedRememberComparison(GtkButton *button, gpointer data) {
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    BankFeedUi *ui = FeedOwner(window);
    if (ui != NULL && !ui->loading && !ui->publishing && !ui->invalidating) {
        UmiBankFeed *copy = NULL;
        UmiStatus status = ui->feed == NULL ? UMI_STATUS_INVALID_STATE : UmiBankFeedClone(ui->feed, &copy);
        char *message = NULL;
        if (status == UMI_STATUS_OK) {
            UmiBankFeedSummary summary;
            (void)UmiBankFeedRead(copy, &summary);
            UmiBankFeedDestroy(ui->comparisonBefore);
            ui->comparisonBefore = copy;
            ui->comparisonInfo = ui->archiveInfo;
            message = g_strdup_printf(
                "Retained before import: %zu rows, all accounts. Source SHA-256: %s\n"
                "Now import the after side and choose Compare retained import. "
                "A restored source digest is an unverified claim.",
                summary.rows, ui->comparisonInfo.sourceSha256);
        } else {
            message = g_strdup_printf("Could not retain import: %s. Previous retained import is unchanged.",
                                      umi_status_text(status));
        }
        ui->publishing = TRUE;
        gtk_label_set_text(GTK_LABEL(ui->comparisonNote), message);
        ui = FeedOwner(window);
        if (ui != NULL) ui->publishing = FALSE;
        g_free(message);
    }
    g_object_unref(window);
}
static void FeedForgetComparison(GtkButton *button, gpointer data) {
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    BankFeedUi *ui = FeedOwner(window);
    if (ui != NULL && !ui->publishing && !ui->invalidating) {
        UmiBankFeedDestroy(ui->comparisonBefore);
        ui->comparisonBefore = NULL;
        memset(&ui->comparisonInfo, 0, sizeof ui->comparisonInfo);
        ui->publishing = TRUE;
        gtk_label_set_text(GTK_LABEL(ui->comparisonNote), "No retained import. Import a file, then retain it explicitly.");
        ui = FeedOwner(window);
        if (ui != NULL) ui->publishing = FALSE;
    }
    g_object_unref(window);
}
static void FeedCompareImports(GtkButton *button, gpointer data) {
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    BankFeedUi *ui = FeedOwner(window);
    if (ui != NULL && !ui->loading && !ui->publishing && !ui->invalidating &&
        ui->feed != NULL && ui->comparisonBefore != NULL) {
        /* Framework's review owns both snapshots. Later imports, clearing the
         * retained side or closing this host cannot leave borrowed row pointers. */
        ui->publishing = TRUE;
        GtkWindow *review = UmiBankFeedComparisonGtk4Create(ui->comparisonBefore, ui->feed, window);
        ui = FeedOwner(window);
        if (ui != NULL) {
            ui->publishing = FALSE;
            if (review != NULL && UmiGtk4WindowRegistered(review))
                gtk_window_present(review);
        }
    } else if (ui != NULL && !ui->loading && !ui->publishing && !ui->invalidating) {
        /* Explain missing explicit inputs rather than silently ignoring a
         * normal user click. Retained observations remain untouched. */
        ui->publishing = TRUE;
        gtk_label_set_text(GTK_LABEL(ui->comparisonNote),
            "Retain a before import and import an after file before comparing. Existing retained data is unchanged.");
        ui = FeedOwner(window);
        if (ui != NULL) ui->publishing = FALSE;
    }
    g_object_unref(window);
}
static void FeedComparisonControls(BankFeedUi *ui, GtkWidget *root, GtkWindow *window) {
    GtkWidget *section = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(root), section);
    GtkWidget *label = gtk_label_new("Compare two imports (all accounts; no merging or posting)");
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(section), label);
    ui->comparisonNote = FeedNamed(
        gtk_label_new("No retained import. Import a file, then retain it explicitly."),
        "bank-feed-comparison-note");
    gtk_label_set_wrap(GTK_LABEL(ui->comparisonNote), TRUE);
    gtk_label_set_selectable(GTK_LABEL(ui->comparisonNote), TRUE);
    gtk_box_append(GTK_BOX(section), ui->comparisonNote);
    GtkWidget *retain = FeedNamed(gtk_button_new_with_label("Retain current import as before"),
                                  "bank-feed-retain-before");
    GtkWidget *compare = FeedNamed(gtk_button_new_with_label("Compare retained import"),
                                   "bank-feed-compare-imports");
    GtkWidget *forget = FeedNamed(gtk_button_new_with_label("Forget retained import"),
                                  "bank-feed-forget-before");
    gtk_box_append(GTK_BOX(section), retain);
    gtk_box_append(GTK_BOX(section), compare);
    gtk_box_append(GTK_BOX(section), forget);
    g_signal_connect_object(retain, "clicked", G_CALLBACK(FeedRememberComparison), window, 0);
    g_signal_connect_object(compare, "clicked", G_CALLBACK(FeedCompareImports), window, 0);
    g_signal_connect_object(forget, "clicked", G_CALLBACK(FeedForgetComparison), window, 0);
}
