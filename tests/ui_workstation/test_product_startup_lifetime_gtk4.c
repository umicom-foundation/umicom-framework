/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_product_startup_lifetime_gtk4.c
 * PURPOSE: Check startup cancellation without constructing or presenting products.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Compile the production private callbacks into this fixture translation unit.
 * Rename only its public runner: the fixture never calls activation, completion
 * or presentation, and does not publish a second production implementation. */
#define umi_application_product_gtk4_run umi_test_unused_product_gtk4_run
#include "../../adapters/gtk4/application_product_application_gtk4.c"
#undef umi_application_product_gtk4_run

#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; goto cleanup; \
    } \
} while (0)

/* A regression may increment this counter, but can never launch a product. */
static gboolean pending_startup_probe(gpointer data)
{
    unsigned *calls = data;
    ++*calls;
    return G_SOURCE_REMOVE;
}

/* Register the exact production lifetime callbacks on unpresented windows. */
static void observe_test_window(GtkWindow *window, ProductApplicationState *state)
{
    g_object_weak_ref(G_OBJECT(window), product_window_finalized, state);
    g_signal_connect(window, "destroy", G_CALLBACK(product_window_destroyed), state);
}

/* Retaining the temporary window deliberately postpones GObject finalization.
 * GtkWindow destruction itself must cancel pending work and the hidden main. */
int main(void)
{
    ProductApplicationState state = {0};
    GtkWindow *retained_main = NULL;
    GtkWindow *retained_startup = NULL;
    unsigned calls = 0U;
    int failed = 0;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    state.window = GTK_WINDOW(gtk_window_new());
    state.startup_window = GTK_WINDOW(gtk_window_new());
    retained_main = g_object_ref_sink(state.window);
    retained_startup = g_object_ref_sink(state.startup_window);
    observe_test_window(state.window, &state);
    observe_test_window(state.startup_window, &state);
    state.startup_source = g_idle_add(pending_startup_probe, &calls);
    CHECK(state.startup_source != 0U);
    CHECK(!gtk_widget_get_realized(GTK_WIDGET(retained_main)));
    CHECK(!gtk_widget_get_realized(GTK_WIDGET(retained_startup)));
    gtk_window_destroy(retained_startup);
    CHECK(state.startup_source == 0U && state.startup_window == NULL && state.window == NULL);
    for (unsigned index = 0U; index < 32U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
    CHECK(calls == 0U);
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(retained_main)));
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(retained_startup)));
    g_clear_object(&retained_main);
    g_clear_object(&retained_startup);

    /* Ordinary close-request cancellation follows the same safe contract. */
    state.window = GTK_WINDOW(gtk_window_new());
    state.startup_window = GTK_WINDOW(gtk_window_new());
    retained_main = g_object_ref_sink(state.window);
    retained_startup = g_object_ref_sink(state.startup_window);
    observe_test_window(state.window, &state);
    observe_test_window(state.startup_window, &state);
    state.startup_source = g_idle_add(pending_startup_probe, &calls);
    CHECK(product_cancel_startup(state.startup_window, &state));
    CHECK(state.startup_source == 0U && state.startup_window == NULL && state.window == NULL);
    for (unsigned index = 0U; index < 32U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
    CHECK(calls == 0U);
cleanup:
    if (state.startup_source != 0U) g_source_remove(state.startup_source);
    state.startup_source = 0U;
    product_release_window(&state, &state.startup_window);
    product_release_window(&state, &state.window);
    g_clear_object(&retained_main);
    g_clear_object(&retained_startup);
    return failed;
}

