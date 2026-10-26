/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/main_context.h
 *
 * PURPOSE:
 *   Bound asynchronous fixture dispatch so pending sources cannot hide deadlines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_MAIN_CONTEXT_H
#define UMICOM_TEST_MAIN_CONTEXT_H
#include <glib.h>

/* A nonblocking iteration can dispatch an always-ready source. Draining until
 * the context becomes idle is therefore not a completion condition. Bound each
 * slice and return control to the fixture's domain-state and deadline checks.
 * This cannot interrupt a callback that itself blocks; CTest retains its outer
 * timeout for that separate failure. NULL selects the default main context. */
static inline unsigned UmiTestDispatchSlice(GMainContext *context,
                                           gint64 deadline,
                                           unsigned maximum_iterations)
{
  unsigned dispatched = 0U;
  while (dispatched < maximum_iterations && g_get_monotonic_time() < deadline) {
    if (!g_main_context_iteration(context, FALSE))
      break;
    ++dispatched;
  }
  return dispatched;
}
#endif
