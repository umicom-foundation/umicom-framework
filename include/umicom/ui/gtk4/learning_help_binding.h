/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/learning_help_binding.h
 *
 * PURPOSE:
 *   Bind explicit product help to native context controls without borrowing application state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_LEARNING_HELP_BINDING_H
#define UMICOM_UI_GTK4_LEARNING_HELP_BINDING_H
#include "umicom/ui/gtk4/learning_catalogue.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLearningHelpGtkState {
    UmiLearningHelpRequest request;
    uint64_t attempts;
    UmiStatus last_status;
    int dispatching;
} UmiLearningHelpGtkState;

/* Bind one explicit route to a context widget and add an unmodified F1 action.
 * Both widgets must be live on the GTK owner thread. The catalogue must not be
 * retired. Binding copies the route and holds the catalogue weakly. It performs
 * no file read and does not load a pack, navigate, present a window or grant
 * content trust. A missing pack is allowed: it may be admitted later.
 *
 * A context can have one binding. Rebinding returns ALREADY_EXISTS; explicitly
 * Unbind before replacing policy. Invalid routes leave any current binding
 * intact. The application owns route identities, never lesson HTML or user
 * document text. Existing keyboard shortcuts remain installed.
 *
 * Retire/destroy the catalogue with its host. A retained or reparented context
 * never recreates a closed catalogue or silently selects another destination. */
UmiStatus UmiLearningHelpGtk4Bind(GtkWidget *context, GtkWidget *catalogue,
    const UmiLearningHelpRoute *route);
void UmiLearningHelpGtk4Unbind(GtkWidget *context);

/* Find the nearest bound ancestor, including origin, and use the same route as
 * F1. This lets an explicit Help button call Open(button) without storing an
 * application pointer. The nearest binding is authoritative even when its
 * catalogue is gone, retired, busy or missing the requested pack.
 *
 * OK means the catalogue accepted an asynchronous read, not that it finished.
 * Observe catalogue busy/last_status for completion. NOT_FOUND can mean no
 * binding or unavailable route/pack/resource. A retired/dead destination returns
 * INVALID_STATE. Missing help in a live idle catalogue receives a local notice.
 * No browser, shell, external URL, automatic pack lookup or fallback locale is
 * used. Call only in response to an explicit help action.
 *
 * Refusal does not change the current reader/history. Notification callbacks
 * may unbind/close controls: the in-flight request owns its copied policy. */
UmiStatus UmiLearningHelpGtk4Open(GtkWidget *origin);
/* Copy nearest binding's last admission result. attempts saturates at UINT64_MAX.
 * dispatching is set while admission/status observers run; nested Open returns
 * BUSY without increasing attempts. last_status is provisional BUSY then final
 * admission status. This receipt is not completion or proof that help rendered.
 * Outputs are preserved on refusal. */
UmiStatus UmiLearningHelpGtk4CopyState(GtkWidget *origin, UmiLearningHelpGtkState *out_state);
#ifdef __cplusplus
}
#endif
#endif
