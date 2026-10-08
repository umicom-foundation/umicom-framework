/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_fill_policy.inc
 * PURPOSE: Review and watch full-quantity intent using the shared decimal and quote policy owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Hold the window during signal publication. A retained button is inert after
 * native closure, and a nested label notification cannot start another watch. */
static ConnectionUi *FillEnter(GtkWindow *window)
{
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || ui->fillBusy)
        return NULL;
    g_object_ref(window);
    ui->fillBusy = TRUE;
    return ui;
}
static void FillLeave(ConnectionUi *ui)
{
    ui->fillBusy = FALSE;
    g_object_unref(ui->window);
}
static UmiStatus FillInputDecimal(GtkWidget *entry, UmiDecimal *out)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    const size_t length = strlen(text);
    const char *point = strchr(text, '.');
    size_t scale = point != NULL ? length - (size_t)(point - text) - 1U : 0U;
    if (scale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiDecimalParse(text, length, (uint8_t)scale, out);
}
static UmiStatus FillReadPolicy(ConnectionUi *ui, UmiFullQuantityPolicy *out)
{
    UmiFullQuantityPolicy policy = {0};
    guint mode = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->fillMode));
    guint side = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->fillSide));
    guint tif = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->fillTif));
    if (mode > 2U || side > 1U || tif > 1U)
        return UMI_STATUS_INVALID_ARGUMENT;
    policy.mode = (UmiFullQuantityMode)mode;
    policy.side = side == 0U ? UMI_SIDE_BUY : UMI_SIDE_SELL;
    policy.waitingTimeInForce = tif == 0U ? UMI_TIF_DAY : UMI_TIF_GTC;
    policy.maximumAgeMilliseconds = (uint64_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->fillAge));
    policy.maximumSkewMilliseconds =
        (uint64_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->fillSkew));
    UmiStatus status = FillInputDecimal(ui->fillQuantity, &policy.quantity);
    if (status == UMI_STATUS_OK)
        status = FillInputDecimal(ui->fillPrice, &policy.limitPrice);
    if (status == UMI_STATUS_OK)
        status = FillInputDecimal(ui->fillBuffer, &policy.extraVisibleQuantity);
    if (status == UMI_STATUS_OK)
        status = UmiFullQuantityPolicyValidate(&policy);
    if (status == UMI_STATUS_OK)
        *out = policy;
    return status;
}
static gchar *FillDescription(const UmiFullQuantityPolicy *policy, const char *observation)
{
    UmiIbkrFillInstruction fields;
    if (UmiIbkrDescribeFillInstruction(policy, &fields) != UMI_STATUS_OK)
        return g_strdup("Invalid policy. No order was sent.");
    char quantity[UMI_DECIMAL_TEXT_MAX + 1U], price[UMI_DECIMAL_TEXT_MAX + 1U];
    if (UmiDecimalFormat(policy->quantity, quantity, sizeof quantity) != UMI_STATUS_OK ||
        UmiDecimalFormat(policy->limitPrice, price, sizeof price) != UMI_STATUS_OK)
        return g_strdup("Unable to format policy. No order was sent.");
    return g_strdup_printf(
        "%s %s units at limit %s in the provider's quoted price units.\n"
        "Requested API fields: orderType=%s; tif=%s; allOrNone=%s; minQty=unset.\n"
        "%s\nContract/route support: UNCONFIRMED. No order was sent.\n"
        "A local watch does not reserve liquidity or guarantee a full fill. "
        "FOK/AON must be supported and enforced by the broker for the exact contract and route. "
        "A minimum quantity or IOC order is not a replacement.",
        policy->side == UMI_SIDE_BUY ? "Buy" : "Sell", quantity, price, fields.orderType, fields.timeInForce,
        fields.allOrNone ? "true" : "false", observation);
}
static UmiStatus FillCurrentQuote(ConnectionUi *ui, uint64_t now, UmiIbkrQuoteSnapshot *out)
{
    if (ui->connection == NULL || ui->quoteRequest == 0U || ui->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_UNAVAILABLE;
    return UmiIbkrQuoteCopy(ui->connection, ui->quoteRequest, now, 15000U, out);
}
static void FillReviewClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    UmiFullQuantityPolicy policy;
    UmiStatus status = FillReadPolicy(ui, &policy);
    gchar *text;
    if (status != UMI_STATUS_OK)
        text = g_strdup("Enter positive quantity and limit, a nonnegative buffer, and skew no larger than "
                        "maximum age. Use decimal points, without group separators. No order was sent.");
    else
    {
        UmiIbkrQuoteSnapshot quote;
        UmiFullQuantityReview review;
        const uint64_t now = UmiIbkrMonotonicMilliseconds();
        status = FillCurrentQuote(ui, now, &quote);
        if (status == UMI_STATUS_OK)
            status =
                UmiIbkrReviewFullQuantity(&policy, &quote.contract, ui->quoteRequest, &quote, now, &review);
        text = FillDescription(&policy, status == UMI_STATUS_OK
                                            ? UmiLiquidityAssessmentText(review.assessment)
                                            : "No usable bound quote is available for this review.");
    }
    gtk_label_set_text(GTK_LABEL(ui->fillOutput), text);
    g_free(text);
    FillLeave(ui);
}
static void FillStartClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    UmiFullQuantityPolicy policy;
    UmiIbkrQuoteSnapshot quote;
    const uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = FillReadPolicy(ui, &policy);
    if (status == UMI_STATUS_OK)
        status = FillCurrentQuote(ui, now, &quote);
    UmiLiquidityObservation observation;
    if (status == UMI_STATUS_OK)
        status = UmiIbkrFillObservation(policy.side, &quote.contract, ui->quoteRequest, &quote, &observation);
    UmiFullQuantityWatch *candidate = NULL;
    if (status == UMI_STATUS_OK && !quote.subscribed)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = UmiFullQuantityWatchCreate(&policy, now, 300000U, &candidate);
    UmiFullQuantityWatchSnapshot snapshot;
    if (status == UMI_STATUS_OK)
        status = UmiFullQuantityWatchObserve(candidate, &observation, now, &snapshot);
    gchar *text;
    if (status == UMI_STATUS_OK)
    {
        FillWatchRetire(ui);
        ui->fillWatch = candidate;
        ui->fillRetired = FALSE;
        ui->fillPolicy = policy;
        ui->fillContract = quote.contract;
        ui->fillRequest = quote.requestId;
        text = FillDescription(&policy, UmiLiquidityAssessmentText(snapshot.review.assessment));
    }
    else
    {
        UmiFullQuantityWatchDestroy(candidate);
        text = g_strdup(
            "Local watch not started. Review the fields and subscribe to a quote first. No order was sent.");
    }
    gtk_label_set_text(GTK_LABEL(ui->fillOutput), text);
    g_free(text);
    FillLeave(ui);
}
static void FillWatchPaint(ConnectionUi *ui, uint64_t now)
{
    if ((ui->fillWatch == NULL && !ui->fillRetired) || ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    if (ui->fillWatch == NULL)
    {
        ui->fillRetired = FALSE;
        gtk_label_set_text(GTK_LABEL(ui->fillOutput),
                           "Local watch stopped. Review before starting another watch. No order was sent.");
        FillLeave(ui);
        return;
    }
    UmiIbkrQuoteSnapshot quote;
    UmiLiquidityObservation observation;
    UmiStatus status = FillCurrentQuote(ui, now, &quote);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrFillObservation(ui->fillPolicy.side, &ui->fillContract, ui->fillRequest, &quote,
                                        &observation);
    UmiFullQuantityWatchSnapshot snapshot;
    gchar *text;
    if (status == UMI_STATUS_OK)
        status = UmiFullQuantityWatchObserve(ui->fillWatch, &observation, now, &snapshot);
    if (status != UMI_STATUS_OK)
    {
        FillWatchRetire(ui);
        ui->fillRetired = FALSE;
        text = g_strdup(
            "Local watch stopped: the bound connection/quote is unavailable or invalid. No order was sent.");
    }
    else if (snapshot.state == UMI_FILL_WATCH_EXPIRED)
    {
        FillWatchRetire(ui);
        ui->fillRetired = FALSE;
        text = g_strdup("The five-minute local watch expired. Review the policy before starting another. No "
                        "order was sent.");
    }
    else
        text = FillDescription(&ui->fillPolicy, UmiLiquidityAssessmentText(snapshot.review.assessment));
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->fillOutput)), text) != 0)
        gtk_label_set_text(GTK_LABEL(ui->fillOutput), text);
    g_free(text);
    FillLeave(ui);
}
static void FillChanged(GtkWidget *widget, gpointer data)
{
    (void)widget;
    /* A nested label notification may edit a field while publication is busy.
     * Retire the watch immediately without publishing another GTK signal. The
     * next paint reports that an explicit review and restart are needed. */
    ConnectionUi *owner = Owner(GTK_WINDOW(data));
    if (owner != NULL && owner->fillBusy)
    {
        FillWatchRetire(owner);
        owner->fillRetired = TRUE;
        return;
    }
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    FillWatchRetire(ui);
    ui->fillRetired = FALSE;
    gtk_label_set_text(
        GTK_LABEL(ui->fillOutput),
        "Policy changed or watch stopped. Review before starting a new local watch. No order was sent.");
    FillLeave(ui);
}
static void FillChoiceChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    FillChanged(GTK_WIDGET(object), data);
}
static void FillPolicyControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Full-quantity policy and local liquidity watch");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    gtk_box_append(GTK_BOX(root), section);
    gtk_box_append(GTK_BOX(box),
                   Text("Review FOK or AON intent without sending an order. A five-minute watch compares "
                        "your rule with this inspector's quote. Confirm price and size units in TWS; a SMART "
                        "quote is not proof of a particular execution route's liquidity or support."));
    const char *modes[] = {"Fill all immediately (FOK)", "Wait for all (AON)",
                           "Watch locally before FOK review", NULL};
    const char *sides[] = {"Buy: inspect ask", "Sell: inspect bid", NULL};
    const char *times[] = {"DAY for AON", "GTC for AON", NULL};
    ui->fillMode = gtk_drop_down_new_from_strings(modes);
    ui->fillSide = gtk_drop_down_new_from_strings(sides);
    ui->fillTif = gtk_drop_down_new_from_strings(times);
    ui->fillQuantity = gtk_entry_new();
    ui->fillPrice = gtk_entry_new();
    ui->fillBuffer = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(ui->fillQuantity), "100");
    gtk_editable_set_text(GTK_EDITABLE(ui->fillBuffer), "0");
    ui->fillAge = gtk_spin_button_new_with_range(1, 15000, 100);
    ui->fillSkew = gtk_spin_button_new_with_range(0, 15000, 100);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->fillAge), 1500);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->fillSkew), 500);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    Field(GTK_GRID(grid), "Full-quantity preference", ui->fillMode, 0, "ibkr-fill-mode");
    Field(GTK_GRID(grid), "Side", ui->fillSide, 1, "ibkr-fill-side");
    Field(GTK_GRID(grid), "Waiting time in force", ui->fillTif, 2, "ibkr-fill-tif");
    Field(GTK_GRID(grid), "Quantity in reported units", ui->fillQuantity, 3, "ibkr-fill-quantity");
    Field(GTK_GRID(grid), "Limit in quoted price units", ui->fillPrice, 4, "ibkr-fill-price");
    Field(GTK_GRID(grid), "Extra displayed quantity", ui->fillBuffer, 5, "ibkr-fill-buffer");
    Field(GTK_GRID(grid), "Maximum field age (ms)", ui->fillAge, 6, "ibkr-fill-age");
    Field(GTK_GRID(grid), "Maximum price/size time gap (ms)", ui->fillSkew, 7, "ibkr-fill-skew");
    gtk_box_append(GTK_BOX(box), grid);
    ui->fillReview = gtk_button_new_with_label("Review policy");
    ui->fillStart = gtk_button_new_with_label("Watch displayed liquidity for five minutes");
    ui->fillStop = gtk_button_new_with_label("Stop local watch");
    gtk_widget_set_name(ui->fillReview, "ibkr-fill-review");
    gtk_widget_set_name(ui->fillStart, "ibkr-fill-start");
    gtk_widget_set_name(ui->fillStop, "ibkr-fill-stop");
    gtk_box_append(GTK_BOX(box), ui->fillReview);
    gtk_box_append(GTK_BOX(box), ui->fillStart);
    gtk_box_append(GTK_BOX(box), ui->fillStop);
    ui->fillOutput = Text(
        "Enter your policy and review it. Orders remain unavailable in this read-only connection window.");
    gtk_widget_set_name(ui->fillOutput, "ibkr-fill-output");
    gtk_label_set_selectable(GTK_LABEL(ui->fillOutput), TRUE);
    gtk_box_append(GTK_BOX(box), ui->fillOutput);
    g_signal_connect_object(ui->fillReview, "clicked", G_CALLBACK(FillReviewClicked), window, 0);
    g_signal_connect_object(ui->fillStart, "clicked", G_CALLBACK(FillStartClicked), window, 0);
    g_signal_connect_object(ui->fillStop, "clicked", G_CALLBACK(FillChanged), window, 0);
    GtkWidget *entries[] = {ui->fillQuantity, ui->fillPrice, ui->fillBuffer};
    for (size_t i = 0U; i < G_N_ELEMENTS(entries); ++i)
        g_signal_connect_object(entries[i], "changed", G_CALLBACK(FillChanged), window, 0);
    GtkWidget *choices[] = {ui->fillMode, ui->fillSide, ui->fillTif};
    for (size_t i = 0U; i < G_N_ELEMENTS(choices); ++i)
        g_signal_connect_object(choices[i], "notify::selected", G_CALLBACK(FillChoiceChanged), window, 0);
    g_signal_connect_object(ui->fillAge, "value-changed", G_CALLBACK(FillChanged), window, 0);
    g_signal_connect_object(ui->fillSkew, "value-changed", G_CALLBACK(FillChanged), window, 0);
}
