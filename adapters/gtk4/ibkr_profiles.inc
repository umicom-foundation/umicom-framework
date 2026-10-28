/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_profiles.inc
 *
 * PURPOSE:
 *   Save and restore explicit broker connection settings without blocking the GTK thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

typedef struct BrokerProfileTask {
    gchar *path;
    gboolean save;
    UmiBrokerAccessPlan plan;
    UmiBrokerAccessProfiles profiles;
} BrokerProfileTask;

static void ProfileTaskFree(gpointer data)
{
    BrokerProfileTask *job = data;
    g_free(job->path);
    g_free(job);
}

static void ProfileWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    BrokerProfileTask *job = data;
    UmiStatus status = job->save
        ? UmiBrokerAccessProfilesStore(job->path, &job->plan)
        : UmiBrokerAccessProfilesLoad(job->path, &job->profiles);
    g_task_return_int(task, (gint)status);
}

static const char *ProfileResult(UmiStatus status, gboolean save)
{
    if (status == UMI_STATUS_OK) return save
        ? "Settings saved locally. No credentials or Live acknowledgement were saved."
        : "Settings loaded. Review them before connecting; Live acknowledgement is cleared.";
    if (status == UMI_STATUS_NOT_FOUND)
        return save ? "Settings not saved. The parent directory must already exist."
                    : "No saved settings found. The current fields have not changed.";
    if (status == UMI_STATUS_BUSY)
        return "Settings are in use. Wait and try again; do not remove the guard file.";
    if (status == UMI_STATUS_PARSE_ERROR || status == UMI_STATUS_CAPACITY_EXCEEDED)
        return "Settings file is damaged, too large or unsupported. It has not been replaced.";
    if (status == UMI_STATUS_INVALID_ARGUMENT)
        return "Check the settings and use an absolute path to an ordinary local file.";
    return "Settings operation failed. Reload before retrying a save; publication may have completed.";
}

static UmiBrokerAccessPlan ProfileFields(ConnectionUi *ui)
{
    guint mode = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode));
    guint program = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program));
    UmiBrokerAccessPlan plan = {
        mode == 0U ? UMI_BROKER_ACCESS_PAPER : mode == 1U
            ? UMI_BROKER_ACCESS_LIVE : (UmiBrokerAccessMode)99,
        program <= 1U ? (UmiIbkrProgram)program : (UmiIbkrProgram)99,
        (uint16_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->port)),
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->client))
    };
    return plan;
}

static gboolean SamePlan(const UmiBrokerAccessPlan *a, const UmiBrokerAccessPlan *b)
{
    return a->mode == b->mode && a->program == b->program &&
        a->port == b->port && a->clientId == b->clientId;
}

static void ProfileComplete(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)unused;
    GTask *task = G_TASK(result);
    BrokerProfileTask *job = g_task_get_task_data(task);
    UmiStatus status = (UmiStatus)g_task_propagate_int(task, NULL);
    /* The task retains the window, never the widgets. Native window removal
     * marks its owner closed, so late completion cannot repaint retired fields. */
    ConnectionUi *ui = Owner(GTK_WINDOW(source));
    if (ui == NULL) return;
    const char *message = ProfileResult(status, job->save);
    if (status == UMI_STATUS_OK && !job->save) {
        UmiBrokerAccessPlan current = ProfileFields(ui);
        if (Connected(ui) || !SamePlan(&current, &job->plan) ||
            strcmp(gtk_editable_get_text(GTK_EDITABLE(ui->profilePath)), job->path) != 0) {
            message = "Settings were read, but the connection fields changed. Load again to apply them.";
        } else {
            const UmiBrokerAccessPlan *saved = current.mode == UMI_BROKER_ACCESS_PAPER
                ? &job->profiles.paper : &job->profiles.live;
            /* A setting notification can retire the window. The task reference
             * protects the owner; recheck closed before accessing another widget. */
            gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->program), (guint)saved->program);
            if (ui->closed) return;
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->port), saved->port);
            if (ui->closed) return;
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->client), saved->clientId);
            if (ui->closed) return;
            gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->ack), FALSE);
            if (ui->closed) return;
        }
    }
    gtk_label_set_text(GTK_LABEL(ui->profileStatus), message);
    if (ui->closed) return;
    ui->profileBusy = FALSE;
    Controls(ui);
}

static void ProfileClicked(GtkButton *button, gpointer data)
{
    GtkWindow *window = GTK_WINDOW(data);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || ui->profileBusy || Connected(ui)) return;
    g_object_ref(window);
    BrokerProfileTask *job = g_new0(BrokerProfileTask, 1);
    job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(ui->profilePath)));
    job->save = GTK_WIDGET(button) == ui->profileSave;
    job->plan = ProfileFields(ui);
    if (UmiBrokerAccessPlanValidate(&job->plan) != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->profileStatus), "Choose valid Paper or Live connection settings first.");
        ProfileTaskFree(job);
        g_object_unref(window);
        return;
    }
    /* Both operations capture their values before returning to the event loop.
     * An explicit Save click is the only route that writes a profile. */
    ui->profileBusy = TRUE;
    Controls(ui);
    if (!ui->closed)
        gtk_label_set_text(GTK_LABEL(ui->profileStatus), job->save ? "Saving local settings..." : "Reading local settings...");
    if (!ui->closed) {
        GTask *task = g_task_new(window, NULL, ProfileComplete, NULL);
        g_task_set_task_data(task, job, ProfileTaskFree);
        g_task_run_in_thread(task, ProfileWorker);
        g_object_unref(task);
    } else {
        ProfileTaskFree(job);
    }
    g_object_unref(window);
}

/* Changing an endpoint invalidates a previous Live decision even if the
 * mode label stays the same. Settings carry intent, never reusable consent. */
static void ProfileEndpointChanged(GtkSpinButton *button, gpointer data)
{
    (void)button;
    GtkWindow *window = GTK_WINDOW(data);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || Connected(ui)) return;
    g_object_ref(window);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->ack), FALSE);
    g_object_unref(window);
}

static void ProfileControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *expander = gtk_expander_new("Saved Paper and Live settings");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    gtk_box_append(GTK_BOX(root), expander);
    gtk_box_append(GTK_BOX(box), Text(
        "Save this mode's program, API port and client ID on this computer. "
        "Load applies only the currently selected mode. Neither action connects."));
    ui->profilePath = gtk_entry_new();
    gtk_widget_set_name(ui->profilePath, "ibkr-profile-path");
    gchar *path = g_build_filename(g_get_user_config_dir(), "umicom-broker-profiles.txt", NULL);
    gtk_editable_set_text(GTK_EDITABLE(ui->profilePath), path);
    g_free(path);
    gtk_widget_set_tooltip_text(ui->profilePath,
        "Absolute local file path. Use a trusted existing directory. The .umi-guard file must remain.");
    gtk_box_append(GTK_BOX(box), Text("Settings file"));
    gtk_box_append(GTK_BOX(box), ui->profilePath);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    ui->profileLoad = gtk_button_new_with_label("Load selected mode");
    ui->profileSave = gtk_button_new_with_label("Save selected mode");
    gtk_widget_set_name(ui->profileLoad, "ibkr-profile-load");
    gtk_widget_set_name(ui->profileSave, "ibkr-profile-save");
    gtk_box_append(GTK_BOX(buttons), ui->profileLoad);
    gtk_box_append(GTK_BOX(buttons), ui->profileSave);
    gtk_box_append(GTK_BOX(box), buttons);
    ui->profileStatus = Text("Settings are only read or written when you choose Load or Save.");
    gtk_widget_set_name(ui->profileStatus, "ibkr-profile-status");
    gtk_box_append(GTK_BOX(box), ui->profileStatus);
    g_signal_connect_object(ui->port, "value-changed", G_CALLBACK(ProfileEndpointChanged), window, 0);
    g_signal_connect_object(ui->client, "value-changed", G_CALLBACK(ProfileEndpointChanged), window, 0);
    g_signal_connect_object(ui->profileLoad, "clicked", G_CALLBACK(ProfileClicked), window, 0);
    g_signal_connect_object(ui->profileSave, "clicked", G_CALLBACK(ProfileClicked), window, 0);
}
