/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/forward_practice_gtk4.c
 *
 * PURPOSE:
 *   Compose local treasury capture, immutable review and explicit application in GTK4.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/forward_practice.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/open_tms/fx_forward_review.h"
#include "umicom/open_tms/fx_forward_file.h"
#include "window_removal_private.h"
#include <errno.h>
#include <math.h>
#include <string.h>

enum { FORWARD_ID, FORWARD_ACCOUNT, FORWARD_PARTY, FORWARD_BASE, FORWARD_QUOTE,
       FORWARD_NOTIONAL, FORWARD_RATE, FORWARD_MARKET, FORWARD_FIELDS };
typedef struct ForwardPanel {
    UmiOpenTmsFxForwardBook book;
    UmiOpenTmsFxForwardReview *review;
    GPtrArray *retained;
    GtkWidget *fields[FORWARD_FIELDS], *selected, *next, *ack, *apply, *note, *preview, *rows;
    uint64_t generation, acknowledged;
    gboolean busy, retired;
    /* File work owns captures independently; review identity belongs to this host. */
    GtkWidget *filePath, *fileAck, *fileApply, *filePreview;
    UmiOpenTmsFxForwardBook *expected;
    unsigned char *pendingBytes;
    size_t pendingSize;
    uint64_t fileGeneration, fileAcknowledged;
    gboolean fileBusy;
} ForwardPanel;
static ForwardPanel *Panel(gpointer window) {
    return g_object_get_data(G_OBJECT(window), "umicom-forward-practice");
}
static GtkWidget *Keep(ForwardPanel *panel, GtkWidget *widget, const char *name) {
    g_ptr_array_add(panel->retained, g_object_ref_sink(widget));
    gtk_widget_set_name(widget, name);
    (void)umi_gtk4_automation_tag_widget(widget, name);
    return widget;
}
static void Invalidate(ForwardPanel *panel) {
    umi_open_tms_fx_forward_review_destroy(panel->review);
    panel->review = NULL; panel->acknowledged = 0U;
    g_clear_pointer(&panel->expected, g_free);
    g_clear_pointer(&panel->pendingBytes, g_free);
    panel->pendingSize = 0U; panel->fileGeneration = 0U; panel->fileAcknowledged = 0U;
    if (panel->generation != UINT64_MAX) ++panel->generation;
}
static void Destroy(gpointer value) {
    ForwardPanel *panel = value;
    umi_open_tms_fx_forward_review_destroy(panel->review);
    g_free(panel->expected); g_free(panel->pendingBytes);
    g_ptr_array_unref(panel->retained);
    g_free(panel);
}
static void Retire(GtkWidget *window, gpointer unused) {
    (void)unused;
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return;
    g_object_ref(window);
    panel->retired = TRUE; Invalidate(panel);
    gtk_widget_set_sensitive(panel->apply, FALSE);
    gtk_widget_set_sensitive(window, FALSE);
    g_object_unref(window);
}
static gboolean Live(ForwardPanel *panel, uint64_t generation) {
    return !panel->retired && panel->generation == generation;
}
static void RevokeFileDisplay(ForwardPanel *panel) {
    gtk_widget_set_sensitive(panel->fileApply, FALSE);
    if (panel->retired) return;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->fileAck), FALSE);
    if (panel->retired) return;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->filePreview)),
        "Open an archive to prepare a replacement review.", -1);
}
static void RevokeDisplay(ForwardPanel *panel) {
    RevokeFileDisplay(panel);
    if (panel->retired) return;
    gtk_widget_set_sensitive(panel->apply, FALSE);
    if (panel->retired) return;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->ack), FALSE);
    if (panel->retired) return;
    gtk_label_set_text(GTK_LABEL(panel->preview), "Prepare a fresh review after any field change.");
}
static void Edited(GtkEditable *field, gpointer window) {
    (void)field;
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return;
    g_object_ref(window);
    /* An observer may edit during a label notification. Revoke identity even
     * while publication is guarded; the outer action rechecks its generation. */
    Invalidate(panel);
    if (!panel->busy) {
        panel->busy = TRUE; RevokeDisplay(panel); panel->busy = FALSE;
    }
    g_object_unref(window);
}
static void SelectionChanged(GObject *object, GParamSpec *spec, gpointer window) {
    (void)object; (void)spec; Edited(NULL, window);
}
static const char *StateName(UmiOpenTmsFxForwardState state) {
    static const char *names[] = {"Draft", "Confirmed", "Settlement pending", "Settled", "Cancelled"};
    return state >= UMI_OPEN_TMS_FX_FORWARD_DRAFT && state <= UMI_OPEN_TMS_FX_FORWARD_CANCELLED
        ? names[(unsigned)state] : "Invalid";
}
static void RenderRows(ForwardPanel *panel) {
    GString *text = g_string_new("Local practice records (not bank or settlement receipts)\n");
    for (size_t i = 0U; i < panel->book.count; ++i) {
        const UmiOpenTmsFxForward *row = &panel->book.trades[i];
        g_string_append_printf(text, "%s | %s/%s | %s | version %" G_GUINT64_FORMAT "\n",
            row->tradeId, row->baseCurrency, row->quoteCurrency, StateName(row->state),
            (guint64)row->version);
    }
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->rows)), text->str, -1);
    g_string_free(text, TRUE);
}
static gboolean CopyText(GtkWidget *field, char *out, size_t capacity) {
    const char *value = gtk_editable_get_text(GTK_EDITABLE(field));
    size_t length = strlen(value);
    if (length == 0U || length >= capacity) return FALSE;
    memcpy(out, value, length + 1U);
    return TRUE;
}
static gboolean Number(GtkWidget *field, double *out) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(field));
    size_t length = strlen(text);
    if (length == 0U || length > 63U) return FALSE;
    /* Accept decimal input only, independently of the desktop locale.
     * NaN, infinity, hexadecimal forms and whitespace are not number entries. */
    gboolean digit = FALSE, dot = FALSE;
    for (size_t i = 0U; i < length; ++i) {
        if (text[i] >= '0' && text[i] <= '9') digit = TRUE;
        else if (text[i] == '.' && !dot) dot = TRUE;
        else return FALSE;
    }
    if (!digit) return FALSE;
    char *end = NULL;
    errno = 0;
    double value = g_ascii_strtod(text, &end);
    if (errno == ERANGE || end == text || *end != '\0' || !isfinite(value)) return FALSE;
    *out = value;
    return TRUE;
}
static void Capture(GtkButton *button, gpointer window) {
    (void)button;
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window);
    panel->busy = TRUE; Invalidate(panel);
    UmiOpenTmsFxForward trade = {0};
    gboolean valid = CopyText(panel->fields[FORWARD_ID], trade.tradeId, sizeof trade.tradeId) &&
        CopyText(panel->fields[FORWARD_ACCOUNT], trade.accountId, sizeof trade.accountId) &&
        CopyText(panel->fields[FORWARD_PARTY], trade.counterpartyId, sizeof trade.counterpartyId) &&
        CopyText(panel->fields[FORWARD_BASE], trade.baseCurrency, sizeof trade.baseCurrency) &&
        CopyText(panel->fields[FORWARD_QUOTE], trade.quoteCurrency, sizeof trade.quoteCurrency) &&
        Number(panel->fields[FORWARD_NOTIONAL], &trade.baseNotional) &&
        Number(panel->fields[FORWARD_RATE], &trade.forwardRate) &&
        Number(panel->fields[FORWARD_MARKET], &trade.marketForwardRate);
    trade.version = 1U; trade.state = UMI_OPEN_TMS_FX_FORWARD_DRAFT;
    UmiStatus status = valid ? umi_open_tms_fx_forward_book_add(&panel->book, &trade) : UMI_STATUS_INVALID_ARGUMENT;
    RevokeDisplay(panel);
    if (!panel->retired && status == UMI_STATUS_OK)
        gtk_editable_set_text(GTK_EDITABLE(panel->selected), trade.tradeId);
    if (!panel->retired) RenderRows(panel);
    if (!panel->retired) {
        char *text = g_strdup_printf("%s. Records exist only in this practice window; nothing was submitted or saved.",
            status == UMI_STATUS_OK ? "Draft captured locally" : umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->note), text); g_free(text);
    }
    panel->busy = FALSE;
    g_object_unref(window);
}
static void Prepare(GtkButton *button, gpointer window) {
    (void)button;
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window);
    panel->busy = TRUE; Invalidate(panel);
    uint64_t generation = panel->generation;
    const char *id = gtk_editable_get_text(GTK_EDITABLE(panel->selected));
    const UmiOpenTmsFxForward *trade = umi_open_tms_fx_forward_book_find(&panel->book, id);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->next));
    static const UmiOpenTmsFxForwardState states[] = {
        UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING,
        UMI_OPEN_TMS_FX_FORWARD_SETTLED, UMI_OPEN_TMS_FX_FORWARD_CANCELLED
    };
    UmiStatus status = trade == NULL ? UMI_STATUS_NOT_FOUND :
        trade->version == UINT64_MAX || generation == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED :
        selected >= G_N_ELEMENTS(states) ? UMI_STATUS_INVALID_ARGUMENT :
        umi_open_tms_fx_forward_review_create(&panel->book, id, states[selected], trade->version + 1U, &panel->review);
    UmiOpenTmsFxForwardReviewSnapshot snapshot;
    if (status == UMI_STATUS_OK) status = umi_open_tms_fx_forward_review_read(panel->review, &snapshot);
    RevokeDisplay(panel);
    if (status == UMI_STATUS_OK && Live(panel, generation) && panel->review != NULL) {
        /* Show all financial assumptions alongside the action. Decimal display
         * is locale independent; these numbers remain observations, not postings. */
        char notional[G_ASCII_DTOSTR_BUF_SIZE], rate[G_ASCII_DTOSTR_BUF_SIZE], market[G_ASCII_DTOSTR_BUF_SIZE];
        g_ascii_dtostr(notional, sizeof notional, snapshot.before.baseNotional);
        g_ascii_dtostr(rate, sizeof rate, snapshot.before.forwardRate);
        g_ascii_dtostr(market, sizeof market, snapshot.before.marketForwardRate);
        /* The shorter first display is retained for engineering review; the
         * complete summary below makes account and numeric assumptions visible. */
#if 0
        char *text = g_strdup_printf("Trade %s | %s/%s\n%s (version %" G_GUINT64_FORMAT
            ") -> %s (version %" G_GUINT64_FORMAT ")\nBook revision %" G_GUINT64_FORMAT
            " -> %" G_GUINT64_FORMAT "\nThis is a local lifecycle change, not approval or settlement.",
            snapshot.before.tradeId, snapshot.before.baseCurrency, snapshot.before.quoteCurrency,
            StateName(snapshot.before.state), (guint64)snapshot.before.version,
            StateName(snapshot.after.state), (guint64)snapshot.after.version,
            (guint64)snapshot.beforeRevision, (guint64)snapshot.afterRevision);
#endif
        /* Archived records may carry a maturity. The former unspecified-only
         * display is retained; show the actual recorded timestamp below. */
#if 0
        char *text = g_strdup_printf("Trade %s | Account %s | Counterparty %s\n%s/%s | Notional %s | Contract %s | Market %s\n"
            "Maturity: unspecified\n%s (version %" G_GUINT64_FORMAT ") -> %s (version %" G_GUINT64_FORMAT
            ")\nBook revision %" G_GUINT64_FORMAT " -> %" G_GUINT64_FORMAT
            "\nThis is a local lifecycle change, not approval or settlement.",
            snapshot.before.tradeId, snapshot.before.accountId, snapshot.before.counterpartyId,
            snapshot.before.baseCurrency, snapshot.before.quoteCurrency, notional, rate,
            snapshot.before.marketForwardRate == 0.0 ? "unavailable" : market,
            StateName(snapshot.before.state), (guint64)snapshot.before.version,
            StateName(snapshot.after.state), (guint64)snapshot.after.version,
            (guint64)snapshot.beforeRevision, (guint64)snapshot.afterRevision);
#endif
        char *maturity = snapshot.before.maturityMilliseconds == 0U ? g_strdup("unspecified") :
            g_strdup_printf("%" G_GUINT64_FORMAT " milliseconds since epoch", (guint64)snapshot.before.maturityMilliseconds);
        char *text = g_strdup_printf("Trade %s | Account %s | Counterparty %s\n%s/%s | Notional %s | Contract %s | Market %s\n"
            "Maturity: %s\n%s (version %" G_GUINT64_FORMAT ") -> %s (version %" G_GUINT64_FORMAT
            ")\nBook revision %" G_GUINT64_FORMAT " -> %" G_GUINT64_FORMAT
            "\nThis is a local lifecycle change, not approval or settlement.",
            snapshot.before.tradeId, snapshot.before.accountId, snapshot.before.counterpartyId,
            snapshot.before.baseCurrency, snapshot.before.quoteCurrency, notional, rate,
            snapshot.before.marketForwardRate == 0.0 ? "unavailable" : market,
            maturity, StateName(snapshot.before.state), (guint64)snapshot.before.version,
            StateName(snapshot.after.state), (guint64)snapshot.after.version,
            (guint64)snapshot.beforeRevision, (guint64)snapshot.afterRevision);
        g_free(maturity);
        gtk_label_set_text(GTK_LABEL(panel->preview), text); g_free(text);
    }
    if (Live(panel, generation))
        gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_OK
            ? "Review the change, then acknowledge it to enable Apply." : umi_status_text(status));
    /* A reentrant edit may revoke the review while its label is being set.
     * Leave no old success display or enabled apply behind that event. */
    if (!panel->retired && (!Live(panel, generation) || panel->review == NULL)) RevokeDisplay(panel);
    panel->busy = FALSE;
    g_object_unref(window);
}
static void Acknowledge(GtkCheckButton *check, gpointer window) {
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window); panel->busy = TRUE;
    gboolean active = gtk_check_button_get_active(check) && panel->review != NULL;
    panel->acknowledged = active ? panel->generation : 0U;
    uint64_t generation = panel->generation;
    gtk_widget_set_sensitive(panel->apply, active);
    if (!Live(panel, generation) || panel->review == NULL || !gtk_check_button_get_active(check)) {
        panel->acknowledged = 0U;
        gtk_widget_set_sensitive(panel->apply, FALSE);
    }
    panel->busy = FALSE; g_object_unref(window);
}
static void Apply(GtkButton *button, gpointer window) {
    (void)button;
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy || panel->review == NULL ||
        panel->acknowledged != panel->generation || !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->ack))) return;
    g_object_ref(window); panel->busy = TRUE;
    UmiStatus status = umi_open_tms_fx_forward_review_apply(&panel->book, panel->review);
    Invalidate(panel); RevokeDisplay(panel);
    if (!panel->retired) RenderRows(panel);
    if (!panel->retired)
        gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_OK
            ? "Local lifecycle changed. No external approval, cash movement or settlement occurred." : umi_status_text(status));
    panel->busy = FALSE; g_object_unref(window);
}
#include "forward_practice_files.inc"

static void AddEntry(ForwardPanel *panel, GtkWidget *box, GtkWindow *window,
    GtkWidget **out, const char *label, const char *name, const char *initial, int maximum) {
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *caption = gtk_label_new(label);
    gtk_widget_set_size_request(caption, 180, -1);
    gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    gtk_box_append(GTK_BOX(row), caption);
    *out = Keep(panel, gtk_entry_new(), name);
    gtk_entry_set_max_length(GTK_ENTRY(*out), maximum);
    gtk_widget_set_hexpand(*out, TRUE);
    gtk_editable_set_text(GTK_EDITABLE(*out), initial);
    gtk_box_append(GTK_BOX(row), *out);
    gtk_box_append(GTK_BOX(box), row);
    g_signal_connect_object(*out, "changed", G_CALLBACK(Edited), window, 0);
}
static GtkWidget *AddButton(ForwardPanel *panel, GtkWidget *box, GtkWindow *window,
    const char *label, const char *name, GCallback callback) {
    GtkWidget *button = Keep(panel, gtk_button_new_with_label(label), name);
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect_object(button, "clicked", callback, window, 0);
    return button;
}
GtkWindow *UmiGtk4ForwardPracticeCreate(GtkWindow *parent) {
    /* A retained destroyed parent is no longer a usable native host. */
    if (parent != NULL && (!GTK_IS_WINDOW(parent) || !UmiGtk4WindowRegistered(parent))) return NULL;
    ForwardPanel *panel = g_new0(ForwardPanel, 1);
    panel->retained = g_ptr_array_new_with_free_func(g_object_unref);
    panel->generation = 1U; panel->busy = TRUE;
    umi_open_tms_fx_forward_book_init(&panel->book);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    /* Keep construction alive if a native removal observer closes the host. */
    g_object_ref(window);
    g_object_set_data_full(G_OBJECT(window), "umicom-forward-practice", panel, Destroy);
    gtk_widget_set_name(GTK_WIDGET(window), "treasury.forward.window");
    gtk_window_set_title(window, "FX-forward lifecycle — local practice");
    gtk_window_set_default_size(window, 900, 720);
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent); gtk_window_set_destroy_with_parent(window, TRUE);
        /* Parent removal observation is installed after all controls exist. */
    }
    GtkWidget *scroll = gtk_scrolled_window_new(), *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(root, 16); gtk_widget_set_margin_end(root, 16);
    gtk_widget_set_margin_top(root, 16); gtk_widget_set_margin_bottom(root, 16);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), root);
    gtk_window_set_child(window, scroll);
    /* The original transient-book description is retained; explicit archives now
     * preserve selected snapshots while closing still discards unsaved changes. */
#if 0
    GtkWidget *intro = gtk_label_new("Independent practice book. Closing this window discards its records.\nNo authentication, provider connection, account posting or external settlement.");
#endif
    GtkWidget *intro = gtk_label_new("Independent practice book. Save explicitly to a new archive; closing discards unsaved changes.\nNo authentication, provider connection, account posting or external settlement.");
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE); gtk_box_append(GTK_BOX(root), intro);
    static const char *labels[] = {"Trade ID", "Practice account ID", "Counterparty label", "Base currency", "Quote currency", "Base notional", "Contract forward rate", "Market forward rate (0 = unavailable)"};
    static const char *names[] = {"id", "account", "party", "base", "quote", "notional", "rate", "market"};
    static const char *initial[] = {"", "practice", "synthetic-party", "GBP", "USD", "1000", "1.25", "0"};
    for (size_t i = 0U; i < FORWARD_FIELDS; ++i) {
        char *name = g_strdup_printf("treasury.forward.%s", names[i]);
        AddEntry(panel, root, window, &panel->fields[i], labels[i], name, initial[i], 63);
        g_free(name);
    }
    AddButton(panel, root, window, "Capture local draft", "treasury.forward.capture", G_CALLBACK(Capture));
    panel->rows = Keep(panel, gtk_text_view_new(), "treasury.forward.rows");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->rows), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->rows), TRUE);
    GtkWidget *table = gtk_scrolled_window_new();
    gtk_widget_set_size_request(table, -1, 140); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(table), panel->rows);
    gtk_box_append(GTK_BOX(root), table);
    AddEntry(panel, root, window, &panel->selected, "Trade ID to review", "treasury.forward.selected", "", 63);
    const char *states[] = {"Confirm local record", "Mark settlement pending locally", "Mark settled locally", "Cancel local record", NULL};
    panel->next = Keep(panel, gtk_drop_down_new_from_strings(states), "treasury.forward.next");
    gtk_box_append(GTK_BOX(root), panel->next);
    g_signal_connect_object(panel->next, "notify::selected", G_CALLBACK(SelectionChanged), window, 0);
    AddButton(panel, root, window, "Prepare transition review", "treasury.forward.prepare", G_CALLBACK(Prepare));
    panel->preview = Keep(panel, gtk_label_new("Prepare a review before changing a record."), "treasury.forward.preview");
    gtk_label_set_wrap(GTK_LABEL(panel->preview), TRUE); gtk_label_set_selectable(GTK_LABEL(panel->preview), TRUE);
    gtk_box_append(GTK_BOX(root), panel->preview);
    panel->ack = Keep(panel, gtk_check_button_new_with_label("I reviewed this local change"), "treasury.forward.ack");
    gtk_box_append(GTK_BOX(root), panel->ack);
    g_signal_connect_object(panel->ack, "toggled", G_CALLBACK(Acknowledge), window, 0);
    panel->apply = AddButton(panel, root, window, "Apply reviewed local change", "treasury.forward.apply", G_CALLBACK(Apply));
    gtk_widget_set_sensitive(panel->apply, FALSE);
    panel->note = Keep(panel, gtk_label_new("Capture a draft to begin."), "treasury.forward.note");
    gtk_label_set_wrap(GTK_LABEL(panel->note), TRUE); gtk_box_append(GTK_BOX(root), panel->note);
    AddEntry(panel, root, window, &panel->filePath, "Archive file (absolute path)",
        "treasury.forward.path", "", (int)UMI_PATH_CAPACITY - 1);
    GtkWidget *files = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(root), files);
    AddButton(panel, files, window, "Save captured records to new file",
        "treasury.forward.save", G_CALLBACK(FileStart));
    GtkWidget *open = AddButton(panel, files, window, "Open archive for review",
        "treasury.forward.open", G_CALLBACK(FileStart));
    g_object_set_data(G_OBJECT(open), "forward-file-load", GINT_TO_POINTER(1));
    panel->filePreview = Keep(panel, gtk_text_view_new(), "treasury.forward.file-preview");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->filePreview), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->filePreview), TRUE);
    GtkWidget *fileScroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(fileScroll, -1, 180);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(fileScroll), panel->filePreview);
    gtk_box_append(GTK_BOX(root), fileScroll);
    panel->fileAck = Keep(panel, gtk_check_button_new_with_label("I reviewed both books and want to replace these local records"),
        "treasury.forward.file-ack");
    gtk_box_append(GTK_BOX(root), panel->fileAck);
    g_signal_connect_object(panel->fileAck, "toggled", G_CALLBACK(FileAcknowledge), window, 0);
    panel->fileApply = AddButton(panel, root, window, "Replace with reviewed archive",
        "treasury.forward.file-apply", G_CALLBACK(FileApply));
    gtk_widget_set_sensitive(panel->fileApply, FALSE);
    RevokeFileDisplay(panel);
    RenderRows(panel); panel->busy = FALSE;
    g_signal_connect(window, "destroy", G_CALLBACK(Retire), NULL);
    UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), Retire, NULL);
    if (parent != NULL)
        UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
    gboolean retired = panel->retired;
    g_object_unref(window);
    return retired ? NULL : window;
}
UmiStatus UmiGtk4ForwardPracticeRead(GtkWindow *window, UmiOpenTmsFxForwardBook *outBook) {
    if (window == NULL || outBook == NULL || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ForwardPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return UMI_STATUS_INVALID_STATE;
    memcpy(outBook, &panel->book, sizeof *outBook);
    return UMI_STATUS_OK;
}
static void Open(GtkButton *button, gpointer unused) {
    (void)unused;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(root) || !UmiGtk4WindowRegistered(GTK_WINDOW(root))) return;
    GtkWindow *window = UmiGtk4ForwardPracticeCreate(GTK_WINDOW(root));
    if (window != NULL) gtk_window_present(window);
}
GtkWidget *UmiGtk4ForwardPracticeLauncherCreate(void) {
    GtkWidget *button = gtk_button_new_with_label("FX-forward practice");
    gtk_widget_set_name(button, "treasury.forward.launch");
    (void)umi_gtk4_automation_tag_widget(button, "treasury.forward.launch");
    g_signal_connect(button, "clicked", G_CALLBACK(Open), NULL);
    return button;
}
