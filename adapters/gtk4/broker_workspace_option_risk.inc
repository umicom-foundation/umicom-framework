/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_option_risk.inc
 *
 * PURPOSE:
 *   Present broker option calculations without inventing values or additional subscriptions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Quote ownership remains in the connection. Switching calculation tabs only
 * changes the view; it never asks the broker for data or starts an order. */
static void WorkspaceOptionRiskRender(UmiBrokerWorkspaceGtk *owner)
{
    for (size_t basis = 0U; basis < UMI_IBKR_OPTION_BASIS_COUNT; ++basis) {
        if (owner->closed) return;
        const UmiIbkrOptionComputation *calculation = &owner->quote.options[basis];
        const char *attribute = !owner->hasQuote || !calculation->received ?
            "No calculation received" : !calculation->attributeReceived ?
            "Volatility basis not supplied by this protocol" :
            calculation->tickAttribute == 0U ? "Return-based volatility" :
            calculation->tickAttribute == 1U ? "Price-based volatility" :
            "Unrecognised provider volatility basis";
        LabelText(owner->optionAttributes[basis], attribute);
        for (size_t measure = 0U; measure < UMI_IBKR_OPTION_MEASURE_COUNT; ++measure) {
            if (owner->closed) return;
            const UmiIbkrQuoteValue *value = &calculation->values[measure];
            bool received = owner->hasQuote && calculation->received && value->received;
            LabelText(owner->optionValues[basis][measure], !received ? "Not received" :
                value->unavailable ? "Unavailable" : value->text);
            if (owner->closed) return;
            char *state = !received ? g_strdup("Waiting") :
                g_strdup_printf("%s • %s", UmiIbkrMarketDataTypeName(value->dataType),
                    value->stale ? "STALE" : "Received within 15 seconds");
            LabelText(owner->optionStates[basis][measure], state);
            g_free(state);
        }
    }
}

static GtkWidget *WorkspaceOptionRiskPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *expander = gtk_expander_new("Option risk observations");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    gtk_widget_set_name(expander, "broker-workspace-option-risk");
    GtkWidget *tabs = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(tabs), TRUE);
    const char *bases[] = {"Bid", "Ask", "Last", "Model"};
    const char *measures[] = {"Implied volatility", "Delta", "Option price",
        "Present value of dividends", "Gamma", "Vega", "Theta", "Underlying price"};
    for (int basis = 0; basis < UMI_IBKR_OPTION_BASIS_COUNT; ++basis) {
        GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        owner->optionAttributes[basis] = Cell("No calculation received", FALSE);
        char *name = g_strdup_printf("broker-workspace-option-basis-%d", basis);
        gtk_widget_set_name(owner->optionAttributes[basis], name);
        g_free(name);
        gtk_box_append(GTK_BOX(page), owner->optionAttributes[basis]);
        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
        gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
        for (int measure = 0; measure < UMI_IBKR_OPTION_MEASURE_COUNT; ++measure) {
            owner->optionValues[basis][measure] = Cell("Not received", FALSE);
            owner->optionStates[basis][measure] = Cell("Waiting", FALSE);
            name = g_strdup_printf("broker-workspace-option-value-%d-%d", basis, measure);
            gtk_widget_set_name(owner->optionValues[basis][measure], name);
            g_free(name);
            name = g_strdup_printf("broker-workspace-option-state-%d-%d", basis, measure);
            gtk_widget_set_name(owner->optionStates[basis][measure], name);
            g_free(name);
            gtk_grid_attach(GTK_GRID(grid), Cell(measures[measure], TRUE), 0, measure, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), owner->optionValues[basis][measure], 1, measure, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), owner->optionStates[basis][measure], 2, measure, 1, 1);
        }
        gtk_box_append(GTK_BOX(page), grid);
        gtk_notebook_append_page(GTK_NOTEBOOK(tabs), page, gtk_label_new(bases[basis]));
    }
    gtk_box_append(GTK_BOX(box), tabs);
    GtkWidget *help = Cell("Subscribe to an option contract to receive broker calculations. "
        "Both the option and underlying need the relevant market-data permissions. "
        "Values use the broker's units and volatility basis. A missing calculation is not zero. "
        "These observations do not value a portfolio or submit an order.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(box), help);
    return expander;
}
