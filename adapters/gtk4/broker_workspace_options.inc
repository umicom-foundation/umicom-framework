/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_options.inc
 *
 * PURPOSE:
 *   Browse option definitions and resolve explicit candidate identities without placing orders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static gboolean WorkspaceOptionsReady(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->optionsBusy;
}
static gboolean WorkspaceOptionsComplete(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->hasOptions && owner->options.complete && !owner->options.stale &&
        !owner->options.failed && !owner->options.abandoned;
}
static void WorkspaceOptionsRender(UmiBrokerWorkspaceGtk *owner)
{
    gboolean pending = owner->hasOptions && !owner->options.complete &&
        !owner->options.failed && !owner->options.abandoned;
    gboolean ready = WorkspaceOptionsReady(owner);
    gboolean choices = ready && WorkspaceOptionsComplete(owner) && owner->optionsItemValid;
    guint expiry = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsExpiry));
    guint strike = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsStrike));
    gboolean selected = choices && expiry < owner->optionsItem.expiryCount &&
        strike < owner->optionsItem.strikeCount;
    GtkWidget *controls[] = {owner->optionsRead, owner->optionsAbandon, owner->optionsResolve,
        owner->optionsChain, owner->optionsExpiry, owner->optionsStrike,
        owner->optionsContract, owner->optionsSymbol, owner->optionsType, owner->optionsFilter};
    gboolean enabled[] = {ready && !pending, ready && pending, selected,
        ready && WorkspaceOptionsComplete(owner), choices, choices,
        !pending && !owner->optionsBusy, !pending && !owner->optionsBusy,
        !pending && !owner->optionsBusy, !pending && !owner->optionsBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed) return;
    char *text = owner->hasOptions ? g_strdup_printf(
        "Request %" PRIu32 " • %s (%" PRIu32 ", %s) • %zu chains • %s%s\n%s",
        owner->options.requestId, owner->options.query.underlyingSymbol,
        owner->options.query.underlyingContractId, owner->options.query.underlyingSecurityType,
        owner->options.count, owner->options.abandoned ? "Abandoned" : owner->options.failed ? "Failed"
        : owner->options.complete ? "Complete" : "Receiving",
        owner->options.stale ? " • STALE" : "", owner->options.message)
        : g_strdup("No option definitions requested.");
    LabelText(owner->optionsStatus, text); g_free(text);
}
static void WorkspaceOptionsSetModel(GtkWidget *widget, GtkStringList *model)
{
    gtk_drop_down_set_model(GTK_DROP_DOWN(widget), G_LIST_MODEL(model));
    /* Replacing a model must not silently choose the first financial instrument. */
    gtk_drop_down_set_selected(GTK_DROP_DOWN(widget), GTK_INVALID_LIST_POSITION);
}
static void WorkspaceOptionsChoices(UmiBrokerWorkspaceGtk *owner)
{
    owner->optionsRendering = TRUE;
    owner->optionsItemValid = FALSE;
    GtkStringList *expiries = gtk_string_list_new(NULL), *strikes = gtk_string_list_new(NULL);
    guint chain = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsChain));
    if (owner->connection != NULL && WorkspaceOptionsComplete(owner) &&
        owner->optionsDisplayedRequest == owner->options.requestId && chain < owner->options.count &&
        UmiIbkrGtkCopyOptionChain(owner->connection, owner->options.requestId, chain,
            &owner->optionsItem) == UMI_STATUS_OK &&
        owner->optionsItem.expiryCount <= UMI_IBKR_OPTION_EXPIRY_LIMIT &&
        owner->optionsItem.strikeCount <= UMI_IBKR_OPTION_STRIKE_LIMIT) {
        owner->optionsItemValid = TRUE;
        for (size_t i = 0U; i < owner->optionsItem.expiryCount; ++i)
            gtk_string_list_append(expiries, owner->optionsItem.expirations[i]);
        for (size_t i = 0U; i < owner->optionsItem.strikeCount; ++i)
            gtk_string_list_append(strikes, owner->optionsItem.strikes[i].reportedText);
    }
    if (!owner->closed) WorkspaceOptionsSetModel(owner->optionsExpiry, expiries);
    if (!owner->closed) WorkspaceOptionsSetModel(owner->optionsStrike, strikes);
    g_object_unref(expiries); g_object_unref(strikes);
    owner->optionsRendering = FALSE;
}
static void WorkspaceOptionsModels(UmiBrokerWorkspaceGtk *owner)
{
    owner->optionsRendering = TRUE;
    GtkStringList *chains = gtk_string_list_new(NULL);
    owner->optionsDisplayedRequest = 0U;
    if (WorkspaceOptionsComplete(owner) && owner->options.count <= UMI_IBKR_OPTION_CHAIN_LIMIT) {
        UmiIbkrOptionChain *item = g_new0(UmiIbkrOptionChain, 1);
        for (size_t i = 0U; i < owner->options.count; ++i) {
            UmiStatus status = UmiIbkrGtkCopyOptionChain(owner->connection, owner->options.requestId, i, item);
            /* A failed copy occupies its original slot. Compacting here would
             * resolve a different contract from the one the user selected. */
            char *label = status == UMI_STATUS_OK ? g_strdup_printf("%s • %s • multiplier %s",
                item->exchange, item->tradingClass, item->multiplier)
                : g_strdup_printf("Chain %zu unavailable — request definitions again", i + 1U);
            char *display = g_utf8_make_valid(label, -1);
            gtk_string_list_append(chains, display); g_free(display); g_free(label);
        }
        g_free(item);
        owner->optionsDisplayedRequest = owner->options.requestId;
    }
    WorkspaceOptionsSetModel(owner->optionsChain, chains);
    g_object_unref(chains);
    owner->optionsRendering = FALSE;
    if (!owner->closed) WorkspaceOptionsChoices(owner);
}
static void WorkspaceOptionsRefresh(UmiBrokerWorkspaceGtk *owner)
{
    if (owner->closed || owner->connection == NULL) return;
    UmiIbkrOptionChainSnapshot copy;
    UmiStatus status = UmiIbkrGtkCopyOptionChains(owner->connection, 60000U, &copy);
    gboolean reset = FALSE;
    if (status == UMI_STATUS_OK && copy.count <= UMI_IBKR_OPTION_CHAIN_LIMIT) {
        reset = !owner->hasOptions || copy.requestId != owner->options.requestId ||
            copy.count != owner->options.count || copy.complete != owner->options.complete;
        owner->options = copy; owner->hasOptions = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        reset = owner->hasOptions;
        memset(&owner->options, 0, sizeof(owner->options)); owner->hasOptions = FALSE;
    } else if (owner->hasOptions) owner->options.stale = true;
    if (reset) WorkspaceOptionsModels(owner);
    if (!owner->closed) WorkspaceOptionsRender(owner);
}
static void WorkspaceOptionsChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)spec;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->optionsRendering || owner->optionsBusy || owner->refreshing) return;
    ++owner->references;
    if (object == G_OBJECT(owner->optionsChain)) WorkspaceOptionsChoices(owner);
    if (!owner->closed) WorkspaceOptionsRender(owner);
    Release(owner);
}
static UmiStatus WorkspaceOptionsQuery(UmiBrokerWorkspaceGtk *owner, UmiIbkrOptionChainQuery *query)
{
    memset(query, 0, sizeof(*query));
    const char *text = gtk_editable_get_text(GTK_EDITABLE(owner->optionsContract));
    if (*text == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (const char *c = text; *c != '\0'; ++c)
        if (*c < '0' || *c > '9') return UMI_STATUS_INVALID_ARGUMENT;
    guint64 identity = g_ascii_strtoull(text, NULL, 10);
    if (identity == 0U || identity > INT_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    query->underlyingContractId = (uint32_t)identity;
    /* GTK limits characters; the protocol limits bytes. Reject overflow rather
     * than silently resolving a truncated, different instrument identity. */
    const char *symbol = gtk_editable_get_text(GTK_EDITABLE(owner->optionsSymbol));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(owner->optionsFilter));
    if (strlen(symbol) >= sizeof(query->underlyingSymbol) || strlen(exchange) >= sizeof(query->exchange))
        return UMI_STATUS_INVALID_ARGUMENT;
    g_strlcpy(query->underlyingSymbol, symbol, sizeof(query->underlyingSymbol));
    const char *types[] = {"STK", "IND", "FUT"};
    guint type = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsType));
    if (type >= G_N_ELEMENTS(types)) return UMI_STATUS_INVALID_ARGUMENT;
    g_strlcpy(query->underlyingSecurityType, types[type], sizeof(query->underlyingSecurityType));
    g_strlcpy(query->exchange, exchange, sizeof(query->exchange));
    return UmiIbkrOptionChainQueryValidate(query);
}
static void WorkspaceOptionsRead(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceOptionsReady(owner)) return;
    UmiIbkrOptionChainQuery query;
    UmiStatus status = WorkspaceOptionsQuery(owner, &query);
    ++owner->references; owner->optionsBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (status == UMI_STATUS_OK && !owner->closed) {
        if (owner->connection == NULL || owner->snapshot.stale || owner->snapshot.state != UMI_IBKR_READY)
            status = UMI_STATUS_UNAVAILABLE;
        else if (owner->hasOptions && !owner->options.complete && !owner->options.failed && !owner->options.abandoned)
            status = UMI_STATUS_BUSY;
        else status = UmiIbkrGtkReadOptionChains(owner->connection, &query);
    }
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->optionsBusy = FALSE;
    if (!owner->closed) WorkspaceOptionsRender(owner);
    if (!owner->closed) LabelText(owner->optionsNotice, status == UMI_STATUS_OK
        ? "Definitions requested. Wait for completion, then select a chain, expiry and strike."
        : status == UMI_STATUS_INVALID_ARGUMENT ? "Enter a positive underlying contract ID, symbol and type."
        : umi_status_text(status));
    Release(owner);
}
static void WorkspaceOptionsAbandon(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceOptionsReady(owner) || !owner->hasOptions) return;
    uint32_t expected = owner->options.requestId;
    ++owner->references; owner->optionsBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed && owner->connection != NULL)
        status = UmiIbkrGtkAbandonOptionChains(owner->connection, expected);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->optionsBusy = FALSE;
    if (!owner->closed) WorkspaceOptionsRender(owner);
    if (!owner->closed) LabelText(owner->optionsNotice, status == UMI_STATUS_OK
        ? "Local definitions abandoned. No quote or order was cancelled." : umi_status_text(status));
    Release(owner);
}
static void WorkspaceOptionsResolve(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceOptionsReady(owner) ||
        !WorkspaceOptionsComplete(owner) || !owner->optionsItemValid) return;
    /* Capture intent before a refresh can replace a model or invoke host code. */
    UmiIbkrOptionSelection selection = {0};
    selection.chainRequest = owner->options.requestId;
    selection.chainIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsChain));
    selection.expiryIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsExpiry));
    selection.strikeIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsStrike));
    guint right = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->optionsRight));
    selection.right = right == 0U ? 'C' : right == 1U ? 'P' : '\0';
    const char *currency = gtk_editable_get_text(GTK_EDITABLE(owner->optionsCurrency));
    const char *route = gtk_editable_get_text(GTK_EDITABLE(owner->optionsRoute));
    if (strlen(currency) >= sizeof(selection.currency) || strlen(route) >= sizeof(selection.exchange)) {
        ++owner->references;
        LabelText(owner->optionsNotice, "Currency or route exceeds the protocol byte limit.");
        Release(owner); return;
    }
    g_strlcpy(selection.currency, currency, sizeof(selection.currency));
    g_strlcpy(selection.exchange, route, sizeof(selection.exchange));
    if (selection.expiryIndex >= owner->optionsItem.expiryCount || selection.strikeIndex >= owner->optionsItem.strikeCount)
        return;
    ++owner->references; owner->optionsBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed) {
        if (owner->connection == NULL || owner->snapshot.stale || !WorkspaceOptionsComplete(owner))
            status = UMI_STATUS_INVALID_STATE;
        else if (selection.chainRequest != owner->options.requestId) status = UMI_STATUS_NOT_FOUND;
        else status = UmiIbkrGtkResolveOption(owner->connection, &selection, 60000U);
    }
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->optionsBusy = FALSE;
    if (!owner->closed) WorkspaceOptionsRender(owner);
    if (!owner->closed) LabelText(owner->optionsNotice, status == UMI_STATUS_OK
        ? "Candidate requested. Review the completed response on Instrument; zero or multiple matches need further review."
        : umi_status_text(status));
    Release(owner);
}
static GtkWidget *WorkspaceOptionsPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *scroll = gtk_scrolled_window_new(), *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8); gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    owner->optionsContract = gtk_entry_new(); owner->optionsSymbol = gtk_entry_new();
    owner->optionsFilter = gtk_entry_new(); owner->optionsCurrency = gtk_entry_new(); owner->optionsRoute = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->optionsContract), 10);
    gtk_entry_set_max_length(GTK_ENTRY(owner->optionsSymbol), 95);
    gtk_entry_set_max_length(GTK_ENTRY(owner->optionsFilter), 63);
    gtk_entry_set_max_length(GTK_ENTRY(owner->optionsCurrency), 15);
    gtk_entry_set_max_length(GTK_ENTRY(owner->optionsRoute), 63);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->optionsFilter), "Optional exchange filter");
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->optionsRoute), "Explicit broker-supported route");
    const char *types[] = {"Stock", "Index", "Future", NULL}, *rights[] = {"Call", "Put", NULL};
    owner->optionsType = gtk_drop_down_new_from_strings(types);
    owner->optionsRight = gtk_drop_down_new_from_strings(rights);
    owner->optionsChain = gtk_drop_down_new(NULL, NULL);
    owner->optionsExpiry = gtk_drop_down_new(NULL, NULL); owner->optionsStrike = gtk_drop_down_new(NULL, NULL);
    GtkWidget *fields[] = {owner->optionsContract, owner->optionsSymbol, owner->optionsType, owner->optionsFilter,
        owner->optionsChain, owner->optionsExpiry, owner->optionsStrike, owner->optionsRight,
        owner->optionsCurrency, owner->optionsRoute};
    const char *labels[] = {"Underlying contract ID", "Underlying symbol", "Underlying type", "Chain exchange",
        "Chain and trading class", "Expiry", "Strike", "Right", "Currency", "Resolution route"};
    for (size_t i = 0U; i < G_N_ELEMENTS(fields); ++i) {
        gtk_grid_attach(GTK_GRID(grid), Cell(labels[i], TRUE), 0, (int)i, 1, 1);
        gtk_widget_set_hexpand(fields[i], TRUE);
        gtk_grid_attach(GTK_GRID(grid), fields[i], 1, (int)i, 1, 1);
    }
    gtk_box_append(GTK_BOX(box), grid);
    owner->optionsRead = gtk_button_new_with_label("Read option definitions");
    owner->optionsAbandon = gtk_button_new_with_label("Abandon definitions");
    owner->optionsResolve = gtk_button_new_with_label("Resolve selected candidate");
    gtk_box_append(GTK_BOX(box), owner->optionsRead); gtk_box_append(GTK_BOX(box), owner->optionsAbandon);
    gtk_box_append(GTK_BOX(box), owner->optionsResolve);
    owner->optionsStatus = Cell("", FALSE); owner->optionsNotice = Cell("", FALSE);
    gtk_label_set_wrap(GTK_LABEL(owner->optionsStatus), TRUE); gtk_label_set_wrap(GTK_LABEL(owner->optionsNotice), TRUE);
    gtk_box_append(GTK_BOX(box), owner->optionsStatus); gtk_box_append(GTK_BOX(box), owner->optionsNotice);
    GtkWidget *note = Cell("Definitions are candidates. Not every expiry and strike forms a valid contract. "
        "Resolve one selection and review its Instrument description. Definitions expire after 60 seconds. "
        "This page does not start quotes, exercise options or submit orders.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(note), TRUE); gtk_box_append(GTK_BOX(box), note);
    GtkWidget *widgets[] = {owner->optionsContract, owner->optionsSymbol, owner->optionsType, owner->optionsFilter,
        owner->optionsChain, owner->optionsExpiry, owner->optionsStrike, owner->optionsRight, owner->optionsCurrency,
        owner->optionsRoute, owner->optionsRead, owner->optionsAbandon, owner->optionsResolve, owner->optionsStatus, owner->optionsNotice};
    const char *names[] = {"contract", "symbol", "type", "filter", "chain", "expiry", "strike", "right", "currency",
        "route", "read", "abandon", "resolve", "status", "notice"};
    for (size_t i = 0U; i < G_N_ELEMENTS(widgets); ++i) {
        char *name = g_strdup_printf("broker-workspace-options-%s", names[i]);
        gtk_widget_set_name(widgets[i], name); g_free(name);
    }
    g_signal_connect(owner->optionsRead, "clicked", G_CALLBACK(WorkspaceOptionsRead), owner);
    g_signal_connect(owner->optionsAbandon, "clicked", G_CALLBACK(WorkspaceOptionsAbandon), owner);
    g_signal_connect(owner->optionsResolve, "clicked", G_CALLBACK(WorkspaceOptionsResolve), owner);
    GtkWidget *choices[] = {owner->optionsChain, owner->optionsExpiry, owner->optionsStrike};
    for (size_t i = 0U; i < G_N_ELEMENTS(choices); ++i)
        g_signal_connect(choices[i], "notify::selected", G_CALLBACK(WorkspaceOptionsChanged), owner);
    return scroll;
}
