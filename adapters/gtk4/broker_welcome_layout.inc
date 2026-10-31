/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_welcome_layout.inc
 *
 * PURPOSE:
 *   Compose the resizable branded account and broker-authentication guidance views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Layout belongs to Framework, while the product retains mode-opening policy.
 * The earlier stacked view is preserved in broker_welcome.c for review. */
static void BuildSessionLayout(UmiBrokerWelcomeGtk *welcome)
{
    GtkWidget *split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_name(split, "broker-welcome-split");
    gtk_paned_set_resize_start_child(GTK_PANED(split), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_position(GTK_PANED(split), 380);
    gtk_widget_set_hexpand(split, TRUE);
    gtk_widget_set_vexpand(split, TRUE);
    gtk_box_append(GTK_BOX(welcome->root), split);
    gtk_widget_add_css_class(welcome->root, "umicom-session-surface");
    UmiGtkStartupStyle(welcome->root);

    GtkWidget *identity = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_start(identity, 28);
    gtk_widget_set_margin_end(identity, 28);
    gtk_widget_set_margin_top(identity, 36);
    gtk_widget_set_margin_bottom(identity, 36);
    gtk_widget_set_valign(identity, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(identity), UmiGtkBrandCreate(TRUE, 58));
    GtkWidget *tagline = Note("Your markets. Your workspace.");
    gtk_widget_add_css_class(tagline, "session-title");
    gtk_box_append(GTK_BOX(identity), tagline);
    GtkWidget *detail = Note("Follow markets, explore charts and manage your trading workspace.");
    gtk_label_set_max_width_chars(GTK_LABEL(detail), 32);
    gtk_widget_add_css_class(detail, "session-muted");
    gtk_box_append(GTK_BOX(identity), detail);
    GtkWidget *art = UmiGtkStartupBackdrop(identity);
    gtk_widget_set_name(art, "broker-welcome-brand");
    GtkWidget *brandScroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(brandScroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(brandScroll), art);
    gtk_paned_set_start_child(GTK_PANED(split), brandScroll);

    GtkWidget *form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_name(form, "broker-welcome-form");
    gtk_widget_add_css_class(form, "session-form");
    gtk_widget_set_valign(form, GTK_ALIGN_CENTER);
    GtkWidget *icon = UmiGtkBrandCreate(FALSE, 36);
    gtk_widget_set_halign(icon, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(form), icon);
    welcome->stage = gtk_stack_new();
    gtk_widget_set_name(welcome->stage, "broker-welcome-stage");
    gtk_stack_set_vhomogeneous(GTK_STACK(welcome->stage), FALSE);
    gtk_stack_set_hhomogeneous(GTK_STACK(welcome->stage), FALSE);
    gtk_stack_set_transition_type(GTK_STACK(welcome->stage), GTK_STACK_TRANSITION_TYPE_NONE);
    gtk_box_append(GTK_BOX(form), welcome->stage);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_name(scroll, "broker-welcome-form-scroll");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), form);
    gtk_paned_set_end_child(GTK_PANED(split), scroll);

    GtkWidget *accounts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    GtkWidget *title = Note("Umicom Trader");
    gtk_widget_add_css_class(title, "session-title");
    gtk_box_append(GTK_BOX(accounts), title);
    welcome->modes = gtk_stack_new();
    gtk_widget_set_name(welcome->modes, "broker-welcome-modes");
    gtk_stack_set_vhomogeneous(GTK_STACK(welcome->modes), FALSE);
    gtk_stack_set_transition_type(GTK_STACK(welcome->modes), GTK_STACK_TRANSITION_TYPE_NONE);
    GtkWidget *switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(welcome->modes));
    gtk_box_append(GTK_BOX(accounts), switcher);
    welcome->connectionOptions = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(welcome->connectionOptions, "broker-welcome-connection-options");
    gtk_box_append(GTK_BOX(welcome->connectionOptions), Note("Local broker application"));
    const char *programs[] = {"TWS", "IB Gateway", NULL};
    welcome->program = gtk_drop_down_new_from_strings(programs);
    gtk_widget_set_name(welcome->program, "broker-welcome-program");
    gtk_accessible_update_property(GTK_ACCESSIBLE(welcome->program),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Local broker application", -1);
    gtk_box_append(GTK_BOX(welcome->connectionOptions), welcome->program);
    gtk_box_append(GTK_BOX(accounts), welcome->connectionOptions);
    gtk_box_append(GTK_BOX(accounts), welcome->modes);
    gtk_stack_add_named(GTK_STACK(welcome->stage), accounts, "accounts");

    const char *titles[] = {"Guest", "Paper Trading (Demo)", "Live Trading"};
    const char *pages[] = {"demo", "paper", "live"};
    const char *descriptions[] = {
        "Explore Umicom Demo with simulated prices and orders. No account or credentials are needed.",
        "Connect your Interactive Brokers paper account through a local broker session.",
        "Connect your Interactive Brokers live account through a local broker session."
    };
    const unsigned order[] = {2U, 1U, 0U};
    for (size_t position = 0U; position < G_N_ELEMENTS(order); ++position) {
        unsigned i = order[position];
        GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
        GtkWidget *description = Note(descriptions[i]);
        gtk_label_set_max_width_chars(GTK_LABEL(description), 42);
        gtk_widget_add_css_class(description, "session-muted");
        gtk_box_append(GTK_BOX(page), description);
        GtkWidget *button = gtk_button_new_with_mnemonic(i == 0U ? "Start Umicom _Demo" : "_Continue");
        gtk_widget_add_css_class(button, "suggested-action");
        gtk_widget_set_hexpand(button, TRUE);
        if (i == 0U) {
            welcome->buttons[0] = button;
            gtk_widget_set_name(button, "broker-welcome-guest");
            g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(0U));
            g_signal_connect(button, "clicked", G_CALLBACK(Choose), welcome);
        } else {
            welcome->begin[i - 1U] = button;
            gtk_widget_set_name(button, i == 1U ? "broker-welcome-begin-paper" : "broker-welcome-begin-live");
            g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(i));
            g_signal_connect(button, "clicked", G_CALLBACK(BeginAuthentication), welcome);
        }
        gtk_box_append(GTK_BOX(page), button);
        gtk_stack_add_titled(GTK_STACK(welcome->modes), page, pages[i], titles[i]);
    }

    GtkWidget *authentication = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    GtkWidget *heading = Note("Broker authentication");
    gtk_widget_add_css_class(heading, "session-title");
    gtk_box_append(GTK_BOX(authentication), heading);
    welcome->authenticationText = Note("");
    gtk_widget_set_name(welcome->authenticationText, "broker-welcome-authentication-guide");
    gtk_label_set_max_width_chars(GTK_LABEL(welcome->authenticationText), 42);
    gtk_widget_add_css_class(welcome->authenticationText, "session-step");
    gtk_box_append(GTK_BOX(authentication), welcome->authenticationText);
    GtkWidget *boundary = Note("Enter your password and any authentication code only in the broker application. "
        "The TWS socket API connects to an already authenticated session.");
    gtk_label_set_max_width_chars(GTK_LABEL(boundary), 42);
    gtk_widget_add_css_class(boundary, "session-muted");
    gtk_box_append(GTK_BOX(authentication), boundary);
    for (unsigned i = 1U; i <= 2U; ++i) {
        GtkWidget *button = gtk_button_new_with_mnemonic("_Continue to Trader");
        welcome->buttons[i] = button;
        gtk_widget_set_name(button, i == 1U ? "broker-welcome-paper" : "broker-welcome-live");
        g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(i));
        gtk_widget_add_css_class(button, "suggested-action");
        g_signal_connect(button, "clicked", G_CALLBACK(Choose), welcome);
        gtk_box_append(GTK_BOX(authentication), button);
        gtk_widget_set_visible(button, FALSE);
    }
    GtkWidget *limit = Note("Current broker access is read-only. Choose Connect in the broker workspace to connect. This screen does not connect "
        "or send orders. Confirm the account and market-data availability in the connection window.");
    gtk_label_set_max_width_chars(GTK_LABEL(limit), 42);
    gtk_widget_add_css_class(limit, "session-muted");
    gtk_box_append(GTK_BOX(authentication), limit);
    welcome->back = gtk_button_new_with_mnemonic("_Back to account selection");
    gtk_widget_set_name(welcome->back, "broker-welcome-back");
    gtk_widget_add_css_class(welcome->back, "flat");
    g_signal_connect(welcome->back, "clicked", G_CALLBACK(BackToAccounts), welcome);
    gtk_box_append(GTK_BOX(authentication), welcome->back);
    gtk_stack_add_named(GTK_STACK(welcome->stage), authentication, "authentication");

    if (welcome->config.openLocalProfiles != NULL) {
        GtkWidget *button = gtk_button_new_with_mnemonic("Open saved _workspace");
        welcome->buttons[3] = button;
        gtk_widget_set_name(button, "broker-welcome-profiles");
        gtk_widget_add_css_class(button, "flat");
        g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(3U));
        g_signal_connect(button, "clicked", G_CALLBACK(Choose), welcome);
        gtk_box_append(GTK_BOX(accounts), button);
    }
    welcome->status = Note("");
    gtk_widget_set_name(welcome->status, "broker-welcome-status");
    gtk_label_set_max_width_chars(GTK_LABEL(welcome->status), 42);
    gtk_box_append(GTK_BOX(form), welcome->status);
    gtk_stack_set_visible_child_name(GTK_STACK(welcome->modes), "paper");
    gtk_stack_set_visible_child_name(GTK_STACK(welcome->stage), "accounts");
    g_signal_connect(welcome->modes, "notify::visible-child-name", G_CALLBACK(ModeChanged), welcome);
}
