/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/profile_secrets/test_profile_keys_gtk4.c
 * PURPOSE: Exercise the native key workflow without opening a personal credential store.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../adapters/gtk4/security/profile_keys_internal.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"

static KeyFixture fixture;
static gint opens, hold_worker, worker_entered, release_worker;
static bool unavailable;
static UmiStatus OpenFixture(const char *application, const char *profile, UmiProfileSecrets **out)
{
    CHECK(strcmp(application, "studio") == 0 && strcmp(profile, "desktop") == 0);
    g_atomic_int_inc(&opens);
    if (g_atomic_int_get(&hold_worker)) {
        g_atomic_int_set(&worker_entered, 1);
        gint64 deadline = g_get_monotonic_time() + 5000000;
        while (!g_atomic_int_get(&release_worker) && g_get_monotonic_time() < deadline) g_usleep(1000U);
        if (!g_atomic_int_get(&release_worker)) return UMI_STATUS_TIMEOUT;
    }
    if (unavailable) { *out = NULL; return UMI_STATUS_UNAVAILABLE; }
    *out = KeyService(&fixture, profile); return UMI_STATUS_OK;
}
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static GtkWidget *Control(UmiProfileKeysGtk *panel, const char *id)
{
    GtkWidget *widget = Find(UmiProfileKeysGtkWidget(panel), id); CHECK(widget != NULL); return widget;
}
static void Text(UmiProfileKeysGtk *panel, const char *id, const char *value)
{
    gtk_editable_set_text(GTK_EDITABLE(Control(panel, id)), value);
}
static void Confirm(UmiProfileKeysGtk *panel, const char *id)
{
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(panel, id)), TRUE);
}
static void Click(UmiProfileKeysGtk *panel, const char *id)
{
    g_signal_emit_by_name(Control(panel, id), "clicked");
}
static void Wait(UmiProfileKeysGtk *panel)
{
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (UmiProfileKeysGtkBusy(panel) && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    CHECK(!UmiProfileKeysGtkBusy(panel));
}
static void Inputs(UmiProfileKeysGtk *panel, bool save)
{
    Text(panel, "keys.password", KEY_PASSWORD);
    Text(panel, "keys.alias", "personal-key");
    if (save) Text(panel, "keys.value", KEY_VALUE);
}
static void EmptyFields(UmiProfileKeysGtk *panel)
{
    const char *ids[] = {"keys.password", "keys.password-confirm", "keys.value"};
    for (size_t i = 0U; i < sizeof(ids)/sizeof(ids[0]); ++i)
        CHECK(AllZero(gtk_editable_get_text(GTK_EDITABLE(Control(panel, ids[i]))), 1U));
}
static void MessageContains(UmiProfileKeysGtk *panel, const char *text)
{
    const char *message = gtk_label_get_text(GTK_LABEL(Control(panel, "keys.message")));
    CHECK(strstr(message, text) != NULL);
    CHECK(strstr(message, KEY_PASSWORD) == NULL && strstr(message, KEY_VALUE) == NULL);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    UmiProfileKeysGtk *panel = NULL;
    CHECK(UmiProfileKeysGtkCreate("studio", "desktop", &panel) == UMI_STATUS_OK);
    panel->open_service = OpenFixture;
    CHECK(g_atomic_int_get(&opens) == 0);
    if (strcmp(argv[1], "create") != 0) {
        UmiProfileSecrets *service = KeyService(&fixture, "desktop");
        CHECK(UmiProfileSecretsRegister(service, KEY_PASSWORD) == UMI_STATUS_OK);
        UmiProfileSecretsDestroy(service); fixture.disposals = 0U;
    }
    if (strcmp(argv[1], "create") == 0) {
        Text(panel, "keys.password", KEY_PASSWORD); Text(panel, "keys.password-confirm", KEY_PASSWORD);
        Confirm(panel, "keys.confirm-create"); Click(panel, "keys.create"); EmptyFields(panel); Wait(panel);
        CHECK(fixture.profile.present && fixture.profile.writes == 1 && fixture.writes == 0U);
        MessageContains(panel, "profile created");
        /* Repeating creation must preserve the existing verifier. */
        Text(panel, "keys.password", "different-password"); Text(panel, "keys.password-confirm", "different-password");
        Confirm(panel, "keys.confirm-create"); Click(panel, "keys.create"); Wait(panel);
        CHECK(fixture.profile.writes == 1); MessageContains(panel, "already exists");
    } else if (strcmp(argv[1], "save-check-remove") == 0) {
        Inputs(panel, true); Confirm(panel, "keys.confirm-save"); Click(panel, "keys.save");
        EmptyFields(panel); Wait(panel); CHECK(fixture.present && fixture.writes == 1U);
        MessageContains(panel, "Key saved locally");
        Inputs(panel, false); Click(panel, "keys.check"); Wait(panel);
        CHECK(fixture.reads == 1U); MessageContains(panel, "No remote login"); EmptyFields(panel);
        Inputs(panel, false); Confirm(panel, "keys.confirm-remove"); Click(panel, "keys.remove"); Wait(panel);
        CHECK(!fixture.present && fixture.removes == 1U && fixture.profile.present);
        MessageContains(panel, "Saved references remain");
    } else if (strcmp(argv[1], "confirmation") == 0) {
        Inputs(panel, true); Click(panel, "keys.save"); EmptyFields(panel);
        CHECK(g_atomic_int_get(&opens) == 0 && fixture.writes == 0U);
        Inputs(panel, false); Click(panel, "keys.remove"); EmptyFields(panel);
        CHECK(g_atomic_int_get(&opens) == 0 && fixture.removes == 0U);
        MessageContains(panel, "Select the confirmation");
    } else if (strcmp(argv[1], "changed-intent") == 0) {
        Inputs(panel, true); Confirm(panel, "keys.confirm-save");
        Text(panel, "keys.value", "edited-test-key");
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Control(panel, "keys.confirm-save"))));
        Click(panel, "keys.save"); CHECK(g_atomic_int_get(&opens) == 0); EmptyFields(panel);
    } else if (strcmp(argv[1], "mismatch") == 0) {
        Text(panel, "keys.password", KEY_PASSWORD); Text(panel, "keys.password-confirm", "different-password");
        Confirm(panel, "keys.confirm-create"); Click(panel, "keys.create");
        CHECK(g_atomic_int_get(&opens) == 0); EmptyFields(panel); MessageContains(panel, "do not match");
    } else if (strcmp(argv[1], "denied") == 0) {
        Inputs(panel, true); Text(panel, "keys.password", "wrong-local-password");
        Confirm(panel, "keys.confirm-save"); Click(panel, "keys.save"); Wait(panel);
        CHECK(fixture.reads == 0U && fixture.writes == 0U && panel->failures == 1U);
        MessageContains(panel, "not accepted");
        /* Test the retry gate with a known future deadline, not elapsed timing. */
        panel->retry_after = g_get_monotonic_time() + 30000000;
        int before = g_atomic_int_get(&opens);
        Inputs(panel, true); Confirm(panel, "keys.confirm-save"); Click(panel, "keys.save");
        CHECK(g_atomic_int_get(&opens) == before); EmptyFields(panel);
    } else if (strcmp(argv[1], "oversized") == 0) {
        char value[UMI_PLATFORM_SECRET_VALUE_CAPACITY + 1U]; memset(value, 'k', sizeof(value)); value[sizeof(value)-1U] = '\0';
        Inputs(panel, true); Text(panel, "keys.value", value); Confirm(panel, "keys.confirm-save"); Click(panel, "keys.save");
        CHECK(g_atomic_int_get(&opens) == 0); EmptyFields(panel); MessageContains(panel, "byte limit");
    } else if (strcmp(argv[1], "private") == 0) {
        Inputs(panel, true);
        CHECK(UmiGtk4RecordingIsPrivate(Control(panel, "keys.password")));
        CHECK(UmiGtk4RecordingIsPrivate(Control(panel, "keys.password-confirm")));
        CHECK(UmiGtk4RecordingIsPrivate(Control(panel, "keys.value")));
        CHECK(UmiGtk4RecordingContainsPrivate(UmiProfileKeysGtkWidget(panel)));
        UmiGtk4AutomationDriver *native = NULL;
        CHECK(umi_gtk4_automation_driver_create(UmiProfileKeysGtkWidget(panel), &native) == UMI_STATUS_OK);
        UmiUiAutomationDriver driver = umi_gtk4_automation_driver_interface(native);
        UmiUiAutomationStep step = {0}; strcpy(step.step_id, "private-check"); strcpy(step.target_id, "keys.value");
        step.operation = UMI_UI_AUTOMATION_ASSERT_TEXT; strcpy(step.value, "never observe private text");
        UmiUiAutomationObservation observed = {0}; char message[512] = {0};
        CHECK(driver.perform(driver.context, &step, &observed, message, sizeof(message)) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(observed.text[0] == '\0' && strstr(message, KEY_VALUE) == NULL);
        umi_gtk4_automation_driver_destroy(native);
    } else if (strcmp(argv[1], "retained") == 0 || strcmp(argv[1], "pending-close") == 0) {
        bool pending = strcmp(argv[1], "pending-close") == 0;
        GtkWidget *root = g_object_ref(UmiProfileKeysGtkWidget(panel));
        GtkWidget *button = g_object_ref(Control(panel, "keys.save"));
        GtkWidget *password = Control(panel, "keys.password"), *value = Control(panel, "keys.value");
        Inputs(panel, true); Confirm(panel, "keys.confirm-save");
        if (pending) {
            g_atomic_int_set(&hold_worker, 1); Click(panel, "keys.save");
            gint64 until = g_get_monotonic_time() + 5000000;
            while (!g_atomic_int_get(&worker_entered) && g_get_monotonic_time() < until) g_usleep(1000U);
            CHECK(g_atomic_int_get(&worker_entered) && !UmiProfileKeysGtkCanClose(panel));
        }
        ++panel->references;
        UmiProfileKeysGtkDestroy(panel);
        CHECK(gtk_editable_get_text(GTK_EDITABLE(password))[0] == '\0' && gtk_editable_get_text(GTK_EDITABLE(value))[0] == '\0');
        CHECK(g_object_get_data(G_OBJECT(root), "umicom-profile-keys") == NULL && !gtk_widget_get_sensitive(root));
        g_signal_emit_by_name(button, "clicked");
        g_atomic_int_set(&release_worker, 1);
        gint64 deadline = g_get_monotonic_time() + 8000000;
        while (panel->references > 1U && g_get_monotonic_time() < deadline) {
            while (g_main_context_iteration(NULL, FALSE)) {}
            g_usleep(1000U);
        }
        CHECK(panel->references == 1U && fixture.writes == (pending ? 1U : 0U));
        CHECK(g_atomic_int_get(&opens) == (pending ? 1 : 0));
        UmiProfileKeysRelease(panel); panel = NULL;
        g_object_unref(button); g_object_unref(root);
    } else if (strcmp(argv[1], "unavailable") == 0) {
        unavailable = true; Inputs(panel, true); Confirm(panel, "keys.confirm-save"); Click(panel, "keys.save"); Wait(panel);
        CHECK(!fixture.present && fixture.writes == 0U); EmptyFields(panel); MessageContains(panel, "no plaintext fallback");
    } else if (strcmp(argv[1], "invalid") == 0) {
        UmiProfileKeysGtk *bad = NULL;
        CHECK(UmiProfileKeysGtkCreate("studio", "Desktop", &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiProfileKeysGtkCreate("../studio", "desktop", &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiProfileKeysGtkCreate("studio", "desktop", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(g_atomic_int_get(&opens) == 0);
    } else { UmiProfileKeysGtkDestroy(panel); return 2; }
    UmiProfileKeysGtkDestroy(panel);
    return 0;
}
