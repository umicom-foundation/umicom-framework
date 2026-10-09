/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/security/local_profile_gate_gtk4.c
 * PURPOSE: Keep asynchronous credential handling and start-screen ownership in Framework.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/gtk4/local_profile_gate.h"
#include "umicom/security/secrets.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>

struct UmiLocalProfileGate {
    unsigned references;
    gboolean closed, busy;
    UmiLocalProfileGateConfig config;
    UmiLocalProfileStore *store;
    GtkWidget *root, *form, *name, *password, *confirmation, *remove_confirmation, *message;
    /* Registration owns separate fields; switching pages erases every password. */
    GtkWidget *pages, *register_name, *register_password, *broker_modes;
    GtkCssProvider *style;
    GdkDisplay *display;
    void (*open_environment)(void *, int);
    GCancellable *cancel;
    gint64 retry_after;
    unsigned failures;
};
typedef struct ProfileJob {
    UmiLocalProfileStore *store;
    unsigned operation;
    char name[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    char password[UMI_LOCAL_PROFILE_PASSWORD_CAPACITY];
} ProfileJob;
static UmiLocalProfileGate *GateFrom(GtkWidget *root)
{ return g_object_get_data(G_OBJECT(root),"umicom-local-profile-gate"); }
static void GateRelease(UmiLocalProfileGate *gate)
{
    if (--gate->references == 0U) {
        UmiLocalProfileStoreRelease(gate->store);
        g_clear_object(&gate->cancel); g_free(gate);
    }
}
/* Separate registration now owns another secret field. Clearing every page prevents hidden passwords remaining after navigation; the earlier clearing route is retained for review. The previous implementation is retained for engineering review. */
#if 0
static void GateClearFields(UmiLocalProfileGate *gate)
{
    gtk_editable_set_text(GTK_EDITABLE(gate->password),"");
    gtk_editable_set_text(GTK_EDITABLE(gate->confirmation),"");
}
#endif
static void GateClearFields(UmiLocalProfileGate *gate)
{
    gtk_editable_set_text(GTK_EDITABLE(gate->password),"");
    gtk_editable_set_text(GTK_EDITABLE(gate->confirmation),"");
    if (gate->register_password != NULL)
        gtk_editable_set_text(GTK_EDITABLE(gate->register_password),"");
}
static void GateMessage(UmiLocalProfileGate *gate, const char *text)
{ if (!gate->closed) gtk_label_set_text(GTK_LABEL(gate->message),text); }
static void ProfileJobFree(gpointer data)
{
    ProfileJob *job = data;
    UmiLocalProfileStoreRelease(job->store);
    umi_secret_clear(job,sizeof(*job)); g_free(job);
}
static void ProfileWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; ProfileJob *job = data;
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!g_cancellable_is_cancelled(cancel)) {
        if (job->operation == 0U) status = UmiLocalProfileVerify(job->store,job->name,job->password);
        else if (job->operation == 1U) status = UmiLocalProfileRegister(job->store,job->name,job->password);
        else status = UmiLocalProfileRemove(job->store,job->name,job->password);
    }
    umi_secret_clear(job->password,sizeof(job->password));
    g_task_return_int(task,(gssize)status);
}
static void GateEnter(UmiLocalProfileGate *gate, const char *name)
{
    ++gate->references;
    GateClearFields(gate);
    UmiStatus status = gate->config.open_workspace(gate->config.user_data,name);
    if (!gate->closed && status != UMI_STATUS_OK)
        GateMessage(gate,"The workspace could not open. Your profile remains available; try again.");
    GateRelease(gate);
}
static void ProfileCompleted(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; UmiLocalProfileGate *gate = data;
    ProfileJob *job = g_task_get_task_data(G_TASK(result));
    GError *error = NULL;
    gssize value = g_task_propagate_int(G_TASK(result),&error);
    UmiStatus status = error == NULL ? (UmiStatus)value : UMI_STATUS_CANCELLED;
    g_clear_error(&error);
    if (!gate->closed) {
        gate->busy = FALSE; gtk_widget_set_sensitive(gate->form,TRUE);
        if (status == UMI_STATUS_OK) {
            gate->failures = 0U;
            if (job->operation == 2U) {
                gtk_check_button_set_active(GTK_CHECK_BUTTON(gate->remove_confirmation),FALSE);
                GateMessage(gate,"Local sign-in removed. Saved layouts remain on this computer.");
            } else GateEnter(gate,job->name);
        } else {
            if (status == UMI_STATUS_PERMISSION_DENIED) {
                if (gate->failures < 5U) ++gate->failures;
                gate->retry_after = g_get_monotonic_time() + (gate->failures >= 5U ? 30000000 : 1000000);
                GateMessage(gate,gate->failures >= 5U ? "Sign-in failed. Wait 30 seconds before trying again."
                    : "The local username or password was not accepted.");
            } else if (status == UMI_STATUS_ALREADY_EXISTS) GateMessage(gate,"That local profile already exists. Choose Sign in.");
            else if (status == UMI_STATUS_PARSE_ERROR) GateMessage(gate,"The saved profile record is not supported or is damaged. No record was overwritten.");
            else if (status != UMI_STATUS_CANCELLED) GateMessage(gate,"Secure profile storage is unavailable. You can still open Simulator.");
        }
    }
    GateRelease(gate);
}
/* Login and registration now submit their own fields with an active-page guard. The shared verifier and worker remain authoritative; the single-form implementation is retained for review. The previous implementation is retained for engineering review. */
#if 0
static void ProfileAction(GtkButton *button, gpointer root_data)
{
    UmiLocalProfileGate *gate = GateFrom(GTK_WIDGET(root_data));
    if (gate == NULL || gate->closed || gate->busy) return;
    if (gate->store == NULL) { GateMessage(gate,"Secure profile storage is unavailable. Open Simulator to continue."); return; }
    if (g_get_monotonic_time() < gate->retry_after) { GateMessage(gate,"Please wait before trying to sign in again."); return; }
    unsigned operation = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button),"profile-operation"));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(gate->password));
    size_t length = strlen(password);
    ProfileJob *job = g_new0(ProfileJob,1);
    if (UmiLocalProfileName(gtk_editable_get_text(GTK_EDITABLE(gate->name)),job->name) != UMI_STATUS_OK) {
        g_free(job); GateMessage(gate,"Use 3–48 letters, numbers, hyphens or underscores, starting with a letter."); return;
    }
    if (length < 12U || length >= sizeof(job->password)) {
        g_free(job); GateMessage(gate,"Use a password between 12 and 256 UTF-8 bytes."); return;
    }
    if (operation == 1U && strcmp(password,gtk_editable_get_text(GTK_EDITABLE(gate->confirmation))) != 0) {
        g_free(job); GateMessage(gate,"The two passwords do not match."); return;
    }
    if (operation == 2U && !gtk_check_button_get_active(GTK_CHECK_BUTTON(gate->remove_confirmation))) {
        g_free(job); GateMessage(gate,"Confirm removal of this local sign-in first. Saved layouts will remain."); return;
    }
    job->store = gate->store; UmiLocalProfileStoreRetain(job->store);
    job->operation = operation; memcpy(job->password,password,length+1U);
    GateClearFields(gate); gate->busy = TRUE;
    gtk_widget_set_sensitive(gate->form,FALSE); GateMessage(gate,"Checking secure local profile…");
    ++gate->references;
    GTask *task = g_task_new(NULL,gate->cancel,ProfileCompleted,gate);
    g_task_set_task_data(task,job,ProfileJobFree);
    g_task_run_in_thread(task,ProfileWorker); g_object_unref(task);
}
#endif
static void ProfileAction(GtkButton *button, gpointer root_data)
{
    UmiLocalProfileGate *gate = GateFrom(GTK_WIDGET(root_data));
    if (gate == NULL || gate->closed || gate->busy) return;
    if (gate->store == NULL) { GateMessage(gate,"Secure profile storage is unavailable. Open Simulator to continue."); return; }
    if (g_get_monotonic_time() < gate->retry_after) { GateMessage(gate,"Please wait before trying to sign in again."); return; }
    unsigned operation = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button),"profile-operation"));
    /* Hidden-page actions cannot submit a different form through a retained button. */
    const char *page = gtk_stack_get_visible_child_name(GTK_STACK(gate->pages));
    if ((operation == 1U) != (g_strcmp0(page, "register") == 0)) return;
    GtkWidget *nameField = operation == 1U ? gate->register_name : gate->name;
    GtkWidget *passwordField = operation == 1U ? gate->register_password : gate->password;
    const char *password = gtk_editable_get_text(GTK_EDITABLE(passwordField));
    size_t length = strlen(password);
    ProfileJob *job = g_new0(ProfileJob,1);
    if (UmiLocalProfileName(gtk_editable_get_text(GTK_EDITABLE(nameField)),job->name) != UMI_STATUS_OK) {
        g_free(job); GateMessage(gate,"Use 3–48 letters, numbers, hyphens or underscores, starting with a letter."); return;
    }
    if (length < 12U || length >= sizeof(job->password)) {
        g_free(job); GateMessage(gate,"Use a password between 12 and 256 UTF-8 bytes."); return;
    }
    if (operation == 1U && strcmp(password,gtk_editable_get_text(GTK_EDITABLE(gate->confirmation))) != 0) {
        g_free(job); GateMessage(gate,"The two passwords do not match."); return;
    }
    if (operation == 2U && !gtk_check_button_get_active(GTK_CHECK_BUTTON(gate->remove_confirmation))) {
        g_free(job); GateMessage(gate,"Confirm removal of this local sign-in first. Saved layouts will remain."); return;
    }
    job->store = gate->store; UmiLocalProfileStoreRetain(job->store);
    job->operation = operation; memcpy(job->password,password,length+1U);
    GateClearFields(gate); gate->busy = TRUE;
    gtk_widget_set_sensitive(gate->form,FALSE); GateMessage(gate,"Checking secure local profile…");
    ++gate->references;
    GTask *task = g_task_new(NULL,gate->cancel,ProfileCompleted,gate);
    g_task_set_task_data(task,job,ProfileJobFree);
    g_task_run_in_thread(task,ProfileWorker); g_object_unref(task);
}
static void SimulatorAction(GtkButton *button, gpointer data)
{
    (void)button; UmiLocalProfileGate *gate = GateFrom(data);
    if (gate != NULL && !gate->closed && !gate->busy) GateEnter(gate,"");
}
static void BrokerAction(GtkButton *button, gpointer data)
{
    (void)button; UmiLocalProfileGate *gate = GateFrom(data);
    if (gate != NULL && !gate->closed && !gate->busy && gate->config.open_broker != NULL)
        gate->config.open_broker(gate->config.user_data);
}
static GtkWidget *GateLabel(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text); gtk_label_set_wrap(GTK_LABEL(label),TRUE);
    gtk_label_set_xalign(GTK_LABEL(label),0.0F); gtk_box_append(GTK_BOX(box),label); return label;
}
static GtkWidget *GateEntry(GtkWidget *box, const char *caption, const char *id, gboolean password)
{
    GtkWidget *label = GateLabel(box,caption);
    GtkWidget *entry = password ? gtk_password_entry_new() : gtk_entry_new();
    if (password) gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(entry),TRUE);
    gtk_accessible_update_property(GTK_ACCESSIBLE(entry),GTK_ACCESSIBLE_PROPERTY_LABEL,caption,-1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label),entry);
    (void)umi_gtk4_automation_tag_widget(entry,id);
    gtk_box_append(GTK_BOX(box),entry); return entry;
}
static GtkWidget *GateButton(UmiLocalProfileGate *gate, GtkWidget *box, const char *label,
                             const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button,id);
    g_signal_connect_object(button,"clicked",callback,gate->root,0);
    gtk_box_append(GTK_BOX(box),button); return button;
}

/* Navigation does not authenticate and must not retain a password from another
 * form. The root-bound closure becomes inert after the gate is destroyed. */
static void GatePageChanged(GObject *pages, GParamSpec *property, gpointer root)
{
    (void)pages; (void)property;
    UmiLocalProfileGate *gate = GateFrom(root);
    if (gate == NULL || gate->closed) return;
    GateClearFields(gate);
    GateMessage(gate, "Your local account stays on this computer. IBKR confirms broker access separately.");
}
static void BrokerEnvironmentAction(GtkButton *button, gpointer root)
{
    UmiLocalProfileGate *gate = GateFrom(root);
    if (gate == NULL || gate->closed || gate->busy || gate->open_environment == NULL) return;
    int live = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "profile-live"));
    gate->open_environment(gate->config.user_data, live);
}
void UmiLocalProfileGateSetBrokerEnvironmentAction(UmiLocalProfileGate *gate,
    void (*action)(void *, int))
{
    if (gate != NULL && !gate->closed && !gate->busy) {
        gate->open_environment = action;
        if (gate->broker_modes != NULL) gtk_widget_set_visible(gate->broker_modes, action != NULL);
    }
}

/* The combined login, registration and removal form made the first action unclear. Separate pages now preserve each workflow with shared authentication and lifecycle handling. The previous composition is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiLocalProfileGateCreate(const UmiLocalProfileGateConfig *config, UmiLocalProfileGate **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || config->application_id == NULL || config->title == NULL || config->open_workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLocalProfileGate *gate = g_new0(UmiLocalProfileGate,1);
    gate->references = 1U; gate->config = *config; gate->cancel = g_cancellable_new();
    if (config->store != NULL) { gate->store = config->store; UmiLocalProfileStoreRetain(gate->store); }
    else (void)UmiLocalProfileStorePlatform(config->application_id,&gate->store);
    gate->root = gtk_scrolled_window_new(); g_object_ref_sink(gate->root);
    g_object_set_data(G_OBJECT(gate->root),"umicom-local-profile-gate",gate);
    (void)umi_gtk4_automation_tag_widget(gate->root,"profile.start");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
    gtk_widget_set_margin_top(box,24); gtk_widget_set_margin_bottom(box,24);
    gtk_widget_set_margin_start(box,24); gtk_widget_set_margin_end(box,24);
    gtk_widget_set_halign(box,GTK_ALIGN_CENTER); gtk_widget_set_size_request(box,420,-1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(gate->root),box);
    GtkWidget *title = GateLabel(box,config->title); gtk_widget_add_css_class(title,"title-1");
    GateLabel(box,"Sign in to a local profile, create one, or open the simulator without a profile.");
    GateLabel(box,"This local password is separate from your broker password. Enter broker credentials and mobile approval only in TWS or IB Gateway.");
    gate->form = gtk_box_new(GTK_ORIENTATION_VERTICAL,8); gtk_box_append(GTK_BOX(box),gate->form);
    gate->name = GateEntry(gate->form,"Local username","profile.username",FALSE);
    gtk_entry_set_max_length(GTK_ENTRY(gate->name),48);
    gate->password = GateEntry(gate->form,"Local password","profile.password",TRUE);
    gate->confirmation = GateEntry(gate->form,"Confirm password when creating a profile","profile.confirm-password",TRUE);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8); gtk_box_append(GTK_BOX(gate->form),row);
    GtkWidget *sign_in = GateButton(gate,row,"Sign in","profile.sign-in",G_CALLBACK(ProfileAction));
    GtkWidget *create = GateButton(gate,row,"Create local profile","profile.create",G_CALLBACK(ProfileAction));
    g_object_set_data(G_OBJECT(create),"profile-operation",GUINT_TO_POINTER(1U));
    gate->remove_confirmation = gtk_check_button_new_with_label("Remove this local sign-in; keep saved layouts");
    (void)umi_gtk4_automation_tag_widget(gate->remove_confirmation,"profile.confirm-remove");
    gtk_box_append(GTK_BOX(gate->form),gate->remove_confirmation);
    GtkWidget *remove = GateButton(gate,gate->form,"Remove local sign-in","profile.remove",G_CALLBACK(ProfileAction));
    g_object_set_data(G_OBJECT(remove),"profile-operation",GUINT_TO_POINTER(2U));
    if (gate->store == NULL) { gtk_widget_set_sensitive(sign_in,FALSE); gtk_widget_set_sensitive(create,FALSE); gtk_widget_set_sensitive(remove,FALSE); }
    GateButton(gate,gate->form,"Open Simulator","profile.simulator",G_CALLBACK(SimulatorAction));
    if (config->open_broker != NULL)
        GateButton(gate,gate->form,"Interactive Brokers connection…","profile.broker",G_CALLBACK(BrokerAction));
    gate->message = GateLabel(box,gate->store != NULL
        ? "Local sign-in opens Simulation. It does not authorise broker trading. Save your layout before closing."
        : "Secure profile storage is unavailable on this system. Simulator remains available.");
    (void)umi_gtk4_automation_tag_widget(gate->message,"profile.message");
    *out = gate; return UMI_STATUS_OK;
}
#endif
UmiStatus UmiLocalProfileGateCreate(const UmiLocalProfileGateConfig *config, UmiLocalProfileGate **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || config->application_id == NULL || config->title == NULL || config->open_workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLocalProfileGate *gate = g_new0(UmiLocalProfileGate,1);
    gate->references = 1U; gate->config = *config; gate->cancel = g_cancellable_new();
    if (config->store != NULL) { gate->store = config->store; UmiLocalProfileStoreRetain(gate->store); }
    else (void)UmiLocalProfileStorePlatform(config->application_id,&gate->store);
    gate->root = gtk_scrolled_window_new(); g_object_ref_sink(gate->root);
    g_object_set_data(G_OBJECT(gate->root),"umicom-local-profile-gate",gate);
    (void)umi_gtk4_automation_tag_widget(gate->root,"profile.start");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
    gtk_widget_set_margin_top(box,24); gtk_widget_set_margin_bottom(box,24);
    gtk_widget_set_margin_start(box,24); gtk_widget_set_margin_end(box,24);
    gtk_widget_set_halign(box,GTK_ALIGN_CENTER); gtk_widget_set_size_request(box,420,-1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(gate->root),box);

    GtkWidget *title = GateLabel(box, config->title);
    gtk_widget_add_css_class(title, "title-1");
    GateLabel(box, "Connect to Interactive Brokers or use a local account for practice.");
    gate->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_box_append(GTK_BOX(box), gate->form);
    if (config->open_broker != NULL) {
        GtkWidget *broker = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_add_css_class(broker, "card");
        GateLabel(broker, "Interactive Brokers");
        GateLabel(broker, "Choose Paper or Live in the connection window. Sign in and complete "
            "two-factor authentication in TWS or IB Gateway, then connect here.");
        GateButton(gate, broker, "Connect Interactive Brokers…", "profile.broker", G_CALLBACK(BrokerAction));
        GtkWidget *modes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        gate->broker_modes = modes;
        gtk_widget_set_visible(modes, FALSE);
        GtkWidget *paper = GateButton(gate, modes, "Paper account", "profile.broker-paper",
            G_CALLBACK(BrokerEnvironmentAction));
        GtkWidget *live = GateButton(gate, modes, "Live account", "profile.broker-live",
            G_CALLBACK(BrokerEnvironmentAction));
        gtk_widget_set_hexpand(paper, TRUE); gtk_widget_set_hexpand(live, TRUE);
        g_object_set_data(G_OBJECT(live), "profile-live", GINT_TO_POINTER(1));
        gtk_box_append(GTK_BOX(broker), modes);
        gtk_box_append(GTK_BOX(gate->form), broker);
    }
    gate->pages = gtk_stack_new();
    gtk_stack_set_hhomogeneous(GTK_STACK(gate->pages), TRUE);
    gtk_stack_set_vhomogeneous(GTK_STACK(gate->pages), FALSE);
    (void)umi_gtk4_automation_tag_widget(gate->pages, "profile.pages");
    GtkWidget *tabs = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(tabs), GTK_STACK(gate->pages));
    gtk_widget_set_halign(tabs, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(gate->form), tabs);
    gtk_box_append(GTK_BOX(gate->form), gate->pages);

    GtkWidget *login = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gate->name = GateEntry(login, "Local username", "profile.username", FALSE);
    gtk_entry_set_max_length(GTK_ENTRY(gate->name), 48);
    gate->password = GateEntry(login, "Local password", "profile.password", TRUE);
    GtkWidget *sign_in = GateButton(gate, login, "Sign in", "profile.sign-in", G_CALLBACK(ProfileAction));
    gtk_widget_add_css_class(sign_in, "suggested-action");
    /* Account removal remains available, but is not a first-run primary action. */
    GtkWidget *manage = gtk_expander_new("Manage this local account");
    GtkWidget *manage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gate->remove_confirmation = gtk_check_button_new_with_label("Remove this local sign-in; keep saved layouts");
    (void)umi_gtk4_automation_tag_widget(gate->remove_confirmation, "profile.confirm-remove");
    gtk_box_append(GTK_BOX(manage_box), gate->remove_confirmation);
    GtkWidget *remove = GateButton(gate, manage_box, "Remove local sign-in", "profile.remove", G_CALLBACK(ProfileAction));
    g_object_set_data(G_OBJECT(remove), "profile-operation", GUINT_TO_POINTER(2U));
    gtk_expander_set_child(GTK_EXPANDER(manage), manage_box);
    gtk_box_append(GTK_BOX(login), manage);
    gtk_stack_add_titled(GTK_STACK(gate->pages), login, "login", "Sign in");

    GtkWidget *registration = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GateLabel(registration, "Create an account for this computer. This does not open an IBKR account.");
    gate->register_name = GateEntry(registration, "Choose a local username", "profile.register-username", FALSE);
    gtk_entry_set_max_length(GTK_ENTRY(gate->register_name), 48);
    gate->register_password = GateEntry(registration, "Choose a local password", "profile.register-password", TRUE);
    gate->confirmation = GateEntry(registration, "Confirm local password", "profile.confirm-password", TRUE);
    GateLabel(registration, "Use 12–256 UTF-8 bytes for the password. Use a separate password from your broker login.");
    GtkWidget *create = GateButton(gate, registration, "Create local account", "profile.create", G_CALLBACK(ProfileAction));
    gtk_widget_add_css_class(create, "suggested-action");
    g_object_set_data(G_OBJECT(create), "profile-operation", GUINT_TO_POINTER(1U));
    gtk_stack_add_titled(GTK_STACK(gate->pages), registration, "register", "Create account");
    gtk_stack_set_visible_child_name(GTK_STACK(gate->pages), "login");
    if (gate->store == NULL) {
        gtk_widget_set_sensitive(sign_in, FALSE);
        gtk_widget_set_sensitive(create, FALSE);
        gtk_widget_set_sensitive(remove, FALSE);
    }
    GateButton(gate, gate->form, "Continue in Simulator", "profile.simulator", G_CALLBACK(SimulatorAction));
    gate->message = GateLabel(box, gate->store != NULL
        ? "Local sign-in restores your profile. Broker connection and trading permissions are separate."
        : "Secure profile storage is unavailable. You can still use Simulator.");
    (void)umi_gtk4_automation_tag_widget(gate->message, "profile.message");
    g_signal_connect_object(gate->pages, "notify::visible-child",
        G_CALLBACK(GatePageChanged), gate->root, 0);
    /* Scope branding to this gate; other applications and system dialogs keep
     * their own theme. The provider is detached when the gate closes. */
    gtk_widget_add_css_class(gate->root, "umicom-profile");
    gate->display = g_object_ref(gtk_widget_get_display(gate->root));
    gate->style = gtk_css_provider_new();
    const char *profile_css =
        ".umicom-profile { background: #111b29; color: #e5eaf1; }"
        ".umicom-profile entry { background: #182638; color: #f1f5fa; border: 1px solid #45607b; padding: 9px; }"
        ".umicom-profile button { background: #223248; color: #f1f5fa; padding: 9px 14px; }"
        ".umicom-profile button.suggested-action { background: #215fa6; }"
        ".umicom-profile .card { background: #192739; padding: 14px; border-radius: 8px; }";
#if GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(gate->style, profile_css);
#else
    gtk_css_provider_load_from_data(gate->style, profile_css, -1);
#endif
    gtk_style_context_add_provider_for_display(gate->display, GTK_STYLE_PROVIDER(gate->style),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    *out = gate;
    return UMI_STATUS_OK;
}
GtkWidget *UmiLocalProfileGateWidget(UmiLocalProfileGate *gate)
{ return gate != NULL && !gate->closed ? gate->root : NULL; }
void UmiLocalProfileGateDestroy(UmiLocalProfileGate *gate)
{
    if (gate == NULL || gate->closed) return;
    gate->closed = TRUE; g_cancellable_cancel(gate->cancel); GateClearFields(gate);
    gtk_widget_set_sensitive(gate->root,FALSE);
    g_object_set_data(G_OBJECT(gate->root),"umicom-local-profile-gate",NULL);
    if (gate->style != NULL && gate->display != NULL)
        gtk_style_context_remove_provider_for_display(gate->display, GTK_STYLE_PROVIDER(gate->style));
    g_clear_object(&gate->style);
    g_clear_object(&gate->display);
    g_clear_object(&gate->root); GateRelease(gate);
}
