/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_help_binding.c
 *
 * PURPOSE:
 *   Share one copied contextual-help policy between F1 and explicit native Help actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_catalogue_private.h"
#include "umicom/ui/gtk4/learning_help_binding.h"

#define HELP_BINDING_KEY "umicom-learning-help-binding"
typedef struct HelpBinding {
    unsigned int references;
    GWeakRef catalogue;
    UmiLearningHelpMap *map;
    GtkEventController *keys; /* Owned by the context while attached. */
    UmiLearningHelpGtkState state;
} HelpBinding;

static void Release(gpointer data)
{
    HelpBinding *binding = data;
    if (--binding->references != 0U) return;
    g_weak_ref_clear(&binding->catalogue);
    UmiLearningHelpMapDestroy(binding->map);
    g_free(binding);
}
static HelpBinding *Nearest(GtkWidget *origin)
{
    if (origin == NULL || !GTK_IS_WIDGET(origin)) return NULL;
    for (GtkWidget *widget = origin; widget != NULL; widget = gtk_widget_get_parent(widget)) {
        HelpBinding *binding = g_object_get_data(G_OBJECT(widget), HELP_BINDING_KEY);
        if (binding != NULL) return binding;
    }
    return NULL;
}
/* A catalogue is shared by several Help contexts and direct guide actions.
 * Protect notice publication at that shared owner, not only at one binding:
 * synchronous label observers may otherwise enter through a different route.
 * Callers hold the root while this helper publishes. Retirement remains valid. */
static void MissingNotice(LearningCatalogue *owner, UmiStatus status)
{
    if (owner == NULL || owner->state.retired || owner->state.busy ||
        (status != UMI_STATUS_NOT_FOUND && status != UMI_STATUS_UNAVAILABLE)) return;
    owner->state.busy = 1;
    LearningCatalogueMessage(owner, status,
        "This help topic is not available. Load its matching local learning pack, then choose Help again.");
    owner->state.busy = 0;
}
static UmiStatus OpenBinding(HelpBinding *binding)
{
    /* A status or dispatch observer may remove this binding. Keep its policy
     * alive independently of the context's data slot until the call returns. */
    if (binding->state.dispatching) return UMI_STATUS_BUSY;
    ++binding->references;
    binding->state.dispatching = 1;
    if (binding->state.attempts != UINT64_MAX) ++binding->state.attempts;
    binding->state.last_status = UMI_STATUS_BUSY;
    GtkWidget *catalogue = g_weak_ref_get(&binding->catalogue);
    UmiStatus status = catalogue != NULL ?
        UmiLearningCatalogueGtk4OpenHelp(catalogue, binding->map, &binding->state.request) :
        UMI_STATUS_INVALID_STATE;
    binding->state.last_status = status;
    LearningCatalogue *owner = catalogue != NULL ? LearningCatalogueOwner(catalogue) : NULL;
    /* Retain the former binding-only notice for review. The shared owner
     * boundary below also protects other bindings and explicit routes. */
#if 0
    if (owner != NULL && !owner->state.retired && !owner->state.busy &&
        (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_UNAVAILABLE))
        LearningCatalogueMessage(owner, status,
            "This help topic is not available. Load its matching local learning pack, then choose Help again.");
#endif
    MissingNotice(owner, status);
    g_clear_object(&catalogue);
    binding->state.dispatching = 0;
    Release(binding);
    return status;
}
static gboolean KeyPressed(GtkEventControllerKey *keys, guint keyval,
    guint keycode, GdkModifierType modifiers, gpointer data)
{
    (void)keycode;
    const GdkModifierType command_modifiers = GDK_SHIFT_MASK | GDK_CONTROL_MASK |
        GDK_ALT_MASK | GDK_SUPER_MASK | GDK_HYPER_MASK | GDK_META_MASK;
    if (keyval != GDK_KEY_F1 || (modifiers & command_modifiers) != 0) return FALSE;
    GtkWidget *context = GTK_WIDGET(data);
    HelpBinding *binding = g_object_get_data(G_OBJECT(context), HELP_BINDING_KEY);
    /* A caller may retain a removed controller. It cannot activate a later
     * binding or fall through into an unrelated ancestor's help policy. */
    if (binding == NULL || binding->keys != GTK_EVENT_CONTROLLER(keys)) return FALSE;
    (void)OpenBinding(binding);
    return TRUE;
}
UmiStatus UmiLearningHelpGtk4Bind(GtkWidget *context, GtkWidget *catalogue,
    const UmiLearningHelpRoute *route)
{
    if (context == NULL || !GTK_IS_WIDGET(context) || route == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningCatalogueGtkState state;
    UmiStatus status = UmiLearningCatalogueGtk4CopyState(catalogue, &state);
    if (status != UMI_STATUS_OK) return status;
    if (state.retired) return UMI_STATUS_INVALID_STATE;
    if (g_object_get_data(G_OBJECT(context), HELP_BINDING_KEY) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    UmiLearningHelpMap *map = NULL;
    status = UmiLearningHelpMapCreate(route, 1U, &map);
    if (status != UMI_STATUS_OK) return status;
    HelpBinding *binding = g_try_new0(HelpBinding, 1U);
    if (binding == NULL) { UmiLearningHelpMapDestroy(map); return UMI_STATUS_OUT_OF_MEMORY; }
    binding->references = 1U;
    binding->map = map;
    binding->state.request = route->request;
    binding->state.last_status = UMI_STATUS_NOT_FOUND; /* No action attempted yet. */
    g_weak_ref_init(&binding->catalogue, catalogue);
    binding->keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(binding->keys, GTK_PHASE_BUBBLE);
    g_signal_connect_object(binding->keys, "key-pressed", G_CALLBACK(KeyPressed), context, 0);
    /* Publish policy before controller attachment, which can notify observers.
     * Local references protect both objects if an observer unbinds immediately. */
    g_object_ref(context);
    ++binding->references;
    g_object_set_data_full(G_OBJECT(context), HELP_BINDING_KEY, binding, Release);
    gtk_widget_add_controller(context, binding->keys);
    status = g_object_get_data(G_OBJECT(context), HELP_BINDING_KEY) == binding ?
        UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    Release(binding);
    g_object_unref(context);
    return status;
}
void UmiLearningHelpGtk4Unbind(GtkWidget *context)
{
    if (context == NULL || !GTK_IS_WIDGET(context)) return;
    HelpBinding *binding = g_object_steal_data(G_OBJECT(context), HELP_BINDING_KEY);
    if (binding == NULL) return;
    /* Clearing the slot first makes reentrant actions refuse the retired policy. */
    GtkEventController *keys = binding->keys;
    binding->keys = NULL;
    gtk_widget_remove_controller(context, keys);
    Release(binding);
}
UmiStatus UmiLearningHelpGtk4Open(GtkWidget *origin)
{
    if (origin == NULL || !GTK_IS_WIDGET(origin)) return UMI_STATUS_INVALID_ARGUMENT;
    HelpBinding *binding = Nearest(origin);
    return binding != NULL ? OpenBinding(binding) : UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiLearningHelpGtk4CopyState(GtkWidget *origin, UmiLearningHelpGtkState *out)
{
    if (origin == NULL || !GTK_IS_WIDGET(origin) || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    HelpBinding *binding = Nearest(origin);
    if (binding == NULL) return UMI_STATUS_NOT_FOUND;
    *out = binding->state;
    return UMI_STATUS_OK;
}

UmiStatus UmiLearningHelpGtk4OpenRoute(GtkWidget *catalogue,
    const UmiLearningHelpRoute *route)
{
    LearningCatalogue *owner=LearningCatalogueOwner(catalogue);
    if(owner==NULL || route==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(owner->state.retired)return UMI_STATUS_INVALID_STATE;
    if(owner->state.busy)return UMI_STATUS_BUSY;
    UmiLearningHelpMap *map=NULL;
    UmiStatus status=UmiLearningHelpMapCreate(route,1U,&map);
    if(status!=UMI_STATUS_OK)return status;
    /* Admission copies the selected pack/resource identity into its worker.
     * Neither the transient map nor the product's route is borrowed by it. */
    g_object_ref(catalogue);
    status=UmiLearningCatalogueGtk4OpenHelp(catalogue,map,&route->request);
    UmiLearningHelpMapDestroy(map);
    /* Keep the former direct-route boundary for review. Both entry points
     * now share MissingNotice so their callback guarantees cannot diverge. */
#if 0
    if(!owner->state.retired && !owner->state.busy &&
        (status==UMI_STATUS_NOT_FOUND || status==UMI_STATUS_UNAVAILABLE)) {
        /* A missing route does not dispatch a worker, but its local notice can
         * still invoke synchronous observers. Hold the same mutation boundary. */
        owner->state.busy=1;
        LearningCatalogueMessage(owner,status,
            "This help topic is not available. Load its matching local learning pack, then choose Help again.");
        owner->state.busy=0;
    }
#endif
    MissingNotice(owner, status);
    g_object_unref(catalogue);
    return status;
}
