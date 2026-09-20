/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/finance_review_gtk4.c
 * PURPOSE:
 *   Render manual financial review fields while keeping calculations in finance_ui.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/finance_review.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/finance_ui/funds_review.h"
#include "umicom/finance/decimal.h"
#include "umicom/finance/currency.h"
#include <stdio.h>
#include <string.h>

typedef struct FinanceReviewFields {
    UmiGtk4FinanceReviewKind kind;
    GtkWidget *currency;
    GtkWidget *scale;
    GtkWidget *amounts[4];
    GtkWidget *result;
    size_t count;
} FinanceReviewFields;

/* The closure watches root rather than borrowing a product runtime.
 * Keeping a reference while publishing a label also covers reentrant observers. */
static void InvalidateReview(GtkEditable *entry, gpointer data)
{
    (void)entry;
    GtkWidget *root = data;
    FinanceReviewFields *fields = g_object_get_data(G_OBJECT(root), "umicom-finance-review");
    if (fields == NULL) return;
    g_object_ref(root);
    gtk_label_set_text(GTK_LABEL(fields->result),
        "Inputs changed. Calculate again; the previous result no longer applies.");
    g_object_unref(root);
}

static void InvalidateScale(GtkSpinButton *spin, gpointer data)
{
    (void)spin;
    InvalidateReview(NULL, data);
}

static void CalculateReview(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = data;
    FinanceReviewFields *fields = g_object_get_data(G_OBJECT(root), "umicom-finance-review");
    if (fields == NULL) return;
    g_object_ref(root);
    const char *code = gtk_editable_get_text(GTK_EDITABLE(fields->currency));
    UmiCurrency currency = {{0}};
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    char message[640];
    (void)snprintf(message, sizeof message, "%s", "Enter a three-letter uppercase currency code, such as GBP.");
    if (strlen(code) == 3U) {
        memcpy(currency.code, code, 3U);
        if (umi_currency_valid(&currency)) status = UMI_STATUS_OK;
    }
    UmiMoney inputs[4] = {{0}};
    uint8_t scale = (uint8_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(fields->scale));
    for (size_t index = 0U; status == UMI_STATUS_OK && index < fields->count; ++index) {
        const char *text = gtk_editable_get_text(GTK_EDITABLE(fields->amounts[index]));
        UmiDecimal parsed;
        status = UmiDecimalParse(text, strlen(text), scale, &parsed);
        if (status != UMI_STATUS_OK) {
            (void)snprintf(message, sizeof message,
                "Amount %zu is invalid or too large. Use a decimal point, no commas, and at most %u decimal places.",
                index + 1U, (unsigned)scale);
            break;
        }
        inputs[index].minor_units = parsed.coefficient;
        inputs[index].scale = parsed.scale;
        inputs[index].currency = currency;
    }
    if (status == UMI_STATUS_OK) {
        char first[UMI_DECIMAL_TEXT_MAX + 1U], second[UMI_DECIMAL_TEXT_MAX + 1U];
        char third[UMI_DECIMAL_TEXT_MAX + 1U];
        if (fields->kind == UMI_GTK4_FINANCE_REVIEW_ACCOUNT) {
            UmiFinanceAccountReview review;
            status = UmiFinanceReviewAccount(&inputs[0], &inputs[1], &inputs[2], &review);
            if (status == UMI_STATUS_OK)
                status = UmiDecimalFormat((UmiDecimal){review.available.minor_units, scale}, first, sizeof first);
            if (status == UMI_STATUS_OK)
                status = UmiDecimalFormat((UmiDecimal){review.remaining_after_payment.minor_units, scale}, second, sizeof second);
            if (status == UMI_STATUS_OK)
                (void)snprintf(message, sizeof message,
                    "Manual review (%s): available %s; remaining after proposed debit %s. No payment or account update was made.",
                    code, first, second);
        } else {
            UmiFinanceCashReview review;
            status = UmiFinanceReviewCash(&inputs[0], &inputs[1], &inputs[2], &inputs[3], &review);
            if (status == UMI_STATUS_OK)
                status = UmiDecimalFormat((UmiDecimal){review.net_flow.minor_units, scale}, first, sizeof first);
            if (status == UMI_STATUS_OK)
                status = UmiDecimalFormat((UmiDecimal){review.projected.minor_units, scale}, second, sizeof second);
            if (status == UMI_STATUS_OK)
                status = UmiDecimalFormat((UmiDecimal){review.buffer_headroom.minor_units, scale}, third, sizeof third);
            if (status == UMI_STATUS_OK)
                (void)snprintf(message, sizeof message,
                    "Manual forecast (%s): net flow %s; projected cash %s; buffer headroom %s. No trade or settlement was created.",
                    code, first, second, third);
        }
        if (status != UMI_STATUS_OK)
            (void)snprintf(message, sizeof message,
                "Cannot calculate this review: %s. Reserved funds, payments, inflows, outflows and buffer must be nonnegative. No result was published.",
                umi_status_text(status));
    }
    gtk_label_set_text(GTK_LABEL(fields->result), message);
    g_object_unref(root);
}

static void CalculateFromEntry(GtkEntry *entry, gpointer data)
{
    (void)entry;
    CalculateReview(NULL, data);
}

static void ClearReview(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = data;
    FinanceReviewFields *fields = g_object_get_data(G_OBJECT(root), "umicom-finance-review");
    if (fields == NULL) return;
    g_object_ref(root);
    for (size_t index = 0U; index < fields->count; ++index)
        gtk_editable_set_text(GTK_EDITABLE(fields->amounts[index]), "0");
    gtk_label_set_text(GTK_LABEL(fields->result),
        "Review cleared. Enter values and calculate; no saved financial data was changed.");
    g_object_unref(root);
}

static void AddField(GtkGrid *grid, int row, const char *caption, GtkWidget *entry)
{
    GtkWidget *label = gtk_label_new_with_mnemonic(caption);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_grid_attach(grid, label, 0, row, 1, 1);
    gtk_grid_attach(grid, entry, 1, row, 1, 1);
    gtk_widget_set_hexpand(entry, TRUE);
}

UmiStatus UmiGtk4FinanceReviewCreate(UmiGtk4FinanceReviewKind kind, GtkWidget **outWidget)
{
    if (outWidget == NULL || (kind != UMI_GTK4_FINANCE_REVIEW_ACCOUNT &&
        kind != UMI_GTK4_FINANCE_REVIEW_CASH)) return UMI_STATUS_INVALID_ARGUMENT;
    *outWidget = NULL;
    FinanceReviewFields *fields = g_try_new0(FinanceReviewFields, 1);
    if (fields == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    fields->kind = kind;
    fields->count = kind == UMI_GTK4_FINANCE_REVIEW_ACCOUNT ? 3U : 4U;
    GtkWidget *root = gtk_expander_new(kind == UMI_GTK4_FINANCE_REVIEW_ACCOUNT
        ? "Account funds review — manual values" : "Cash forecast review — manual values");
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *calculate = gtk_button_new_with_mnemonic("_Calculate");
    GtkWidget *clear = gtk_button_new_with_label("Clear amounts");
    fields->currency = gtk_entry_new();
    fields->scale = gtk_spin_button_new_with_range(0.0, 9.0, 1.0);
    gtk_entry_set_max_length(GTK_ENTRY(fields->currency), 3);
    gtk_editable_set_text(GTK_EDITABLE(fields->currency), "GBP");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(fields->scale), 2.0);
    gtk_widget_set_tooltip_text(fields->scale,
        "Explicit decimal places (0 to 9). This is not inferred from the currency code.");
    fields->result = gtk_label_new("Enter values for a manual review. This panel is not connected to account, payment or trading services.");
    gtk_label_set_wrap(GTK_LABEL(fields->result), TRUE);
    gtk_label_set_selectable(GTK_LABEL(fields->result), TRUE);
    gtk_label_set_xalign(GTK_LABEL(fields->result), 0.0F);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6U);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12U);
    gtk_widget_set_margin_start(body, 12);
    gtk_widget_set_margin_end(body, 12);
    gtk_widget_set_margin_top(body, 8);
    gtk_widget_set_margin_bottom(body, 12);
    AddField(GTK_GRID(grid), 0, "_Currency", fields->currency);
    AddField(GTK_GRID(grid), 1, "Decimal _places", fields->scale);
    static const char *accountLabels[] = {"_Ledger balance", "_Reserved funds", "Proposed _debit"};
    static const char *cashLabels[] = {"_Opening cash", "_Inflows", "_Outflows", "_Buffer"};
    for (size_t index = 0U; index < fields->count; ++index) {
        fields->amounts[index] = gtk_entry_new();
        gtk_entry_set_max_length(GTK_ENTRY(fields->amounts[index]), (int)UMI_DECIMAL_TEXT_MAX);
        gtk_editable_set_text(GTK_EDITABLE(fields->amounts[index]), "0");
        const char *caption = kind == UMI_GTK4_FINANCE_REVIEW_ACCOUNT ? accountLabels[index] : cashLabels[index];
        AddField(GTK_GRID(grid), (int)index + 2, caption, fields->amounts[index]);
        gtk_widget_set_tooltip_text(fields->amounts[index],
            "Decimal amount using a point, without grouping commas or exponent notation.");
        char id[80];
        (void)snprintf(id, sizeof id, "finance.review.amount.%zu", index);
        (void)umi_gtk4_automation_tag_widget(fields->amounts[index], id);
        g_signal_connect_object(fields->amounts[index], "changed", G_CALLBACK(InvalidateReview), root, 0);
        g_signal_connect_object(fields->amounts[index], "activate", G_CALLBACK(CalculateFromEntry), root, 0);
    }
    g_object_set_data_full(G_OBJECT(root), "umicom-finance-review", fields, g_free);
    g_signal_connect_object(fields->currency, "changed", G_CALLBACK(InvalidateReview), root, 0);
    g_signal_connect_object(fields->scale, "value-changed", G_CALLBACK(InvalidateScale), root, 0);
    g_signal_connect_object(calculate, "clicked", G_CALLBACK(CalculateReview), root, 0);
    g_signal_connect_object(clear, "clicked", G_CALLBACK(ClearReview), root, 0);
    (void)umi_gtk4_automation_tag_widget(root, "finance.review");
    (void)umi_gtk4_automation_tag_widget(fields->currency, "finance.review.currency");
    (void)umi_gtk4_automation_tag_widget(fields->scale, "finance.review.scale");
    (void)umi_gtk4_automation_tag_widget(calculate, "finance.review.calculate");
    (void)umi_gtk4_automation_tag_widget(clear, "finance.review.clear");
    (void)umi_gtk4_automation_tag_widget(fields->result, "finance.review.result");
    gtk_box_append(GTK_BOX(actions), calculate);
    gtk_box_append(GTK_BOX(actions), clear);
    gtk_box_append(GTK_BOX(body), grid);
    gtk_box_append(GTK_BOX(body), actions);
    gtk_box_append(GTK_BOX(body), fields->result);
    gtk_expander_set_child(GTK_EXPANDER(root), body);
    *outWidget = root;
    return UMI_STATUS_OK;
}
