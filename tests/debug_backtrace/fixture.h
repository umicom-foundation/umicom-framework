/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_backtrace/fixture.h
 *
 * PURPOSE:
 *   Build a synthetic selected-thread workspace without starting a debugger.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_BACKTRACE_FIXTURE_H
#define UMICOM_TEST_BACKTRACE_FIXTURE_H
#include "umicom/debug/backtrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct TraceFixture {
  UmiProtocolTransport *transport;
  UmiProtocolClient *client;
  UmiDapClient dap;
  UmiDebugService *service;
  UmiDebugController *controller;
  UmiDebugWorkspace *workspace;
  UmiDebugThreadSnapshot thread;
  UmiDebugStackFrameSnapshot frame;
} TraceFixture;
static inline void TraceFixtureCreate(TraceFixture *f) {
  memset(f, 0, sizeof *f);
  OK(umi_protocol_transport_create_memory(16U, &f->transport));
  OK(umi_protocol_client_create(f->transport, &f->client));
  OK(umi_protocol_client_start(f->client));
  OK(umi_dap_client_init(&f->dap, f->client));
  OK(umi_debug_service_create(&f->service));
  OK(umi_debug_controller_create(&f->dap, f->service, &f->controller));
  OK(umi_debug_workspace_create(f->service, f->controller, &f->workspace));
  strcpy(f->thread.id, "thread-7");
  strcpy(f->thread.session_id, "session");
  strcpy(f->thread.name, "main thread");
  f->thread.stopped = 1;
  f->thread.current = 1;
  OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f->service), &f->thread));
  strcpy(f->frame.id, "frame-0");
  strcpy(f->frame.thread_id, f->thread.id);
  strcpy(f->frame.name, "main");
  strcpy(f->frame.source_uri, "/private/project/main.c");
  f->frame.line = 42U;
  f->frame.column = 3U;
  OK(umi_debug_stack_frame_registry_upsert(umi_debug_service_stack_frame(f->service), &f->frame));
}
static inline void TraceFixtureDestroy(TraceFixture *f) {
  umi_debug_workspace_destroy(f->workspace);
  umi_debug_controller_destroy(f->controller);
  umi_debug_service_destroy(f->service);
  umi_protocol_client_destroy(f->client);
  umi_protocol_transport_destroy(f->transport);
  memset(f, 0, sizeof *f);
}
static inline bool TraceKnown(const char *name, const char *const *names, size_t count) {
  for (size_t i = 0U; i < count; ++i)
    if (strcmp(name, names[i]) == 0)
      return true;
  return false;
}
#endif
