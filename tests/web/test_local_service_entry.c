/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_local_service_entry.c
 *
 * PURPOSE:
 *   Exercise the actual example argument entry and rooted file response with a controlled
 * transport, including Unicode process arguments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiWebService *entry_service;
static int entry_created, entry_dispatched, entry_failed;
static unsigned char server_identity;
static UmiStatus EntryCreate(const UmiWebServerConfig *config, UmiWebService *service,
                             UmiWebServer **out);
static UmiStatus EntryStart(UmiWebServer *server);
static void EntryDestroy(UmiWebServer *server);
static UmiStatus EntryServe(UmiWebServer *server, uint32_t wait, uint32_t exchange,
                            const UmiCancellationToken *cancel, UmiWebExchangeResult *out);
/* Keep the real entry, UTF-8 capture, route admission and rooted reads. Replace
 * only listener ownership so this regression never opens a network endpoint. */
#define main LocalServiceEntry
#define umi_web_server_create EntryCreate
#define umi_web_server_start EntryStart
#define umi_web_server_destroy EntryDestroy
#define UmiWebServerServeNext EntryServe
#include "../../examples/web/local_service.c"
#undef main
#undef umi_web_server_create
#undef umi_web_server_start
#undef umi_web_server_destroy
#undef UmiWebServerServeNext
static UmiStatus EntryCreate(const UmiWebServerConfig *config, UmiWebService *service,
                             UmiWebServer **out) {
  if (config->port != 8080U || service == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  entry_service = service;
  *out = (UmiWebServer *)&server_identity;
  ++entry_created;
  return UMI_STATUS_OK;
}
static UmiStatus EntryStart(UmiWebServer *server) {
  return server == (UmiWebServer *)&server_identity ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static void EntryDestroy(UmiWebServer *server) {
  if (server != NULL && server != (UmiWebServer *)&server_identity)
    entry_failed = 1;
  entry_service = NULL;
}
static UmiStatus EntryServe(UmiWebServer *server, uint32_t wait, uint32_t exchange,
                            const UmiCancellationToken *cancel, UmiWebExchangeResult *out) {
  (void)cancel;
  memset(out, 0, sizeof(*out));
  stop_requested = 1;
  if (server != (UmiWebServer *)&server_identity || wait != 100U || exchange != 3000U) {
    entry_failed = 1;
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  UmiWebRequest *request = calloc(1U, sizeof(*request));
  UmiWebResponse *response = calloc(1U, sizeof(*response));
  if (request == NULL || response == NULL) {
    free(request);
    free(response);
    entry_failed = 1;
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  static const char wire[] = "GET /message.txt HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n";
  UmiStatus status = UmiWebRequestParseStrict(wire, sizeof(wire) - 1U, sizeof(wire), request);
  if (status == UMI_STATUS_OK)
    status = umi_web_service_handle(entry_service, request, response);
  static const char expected[] = "Unicode root reached.\n";
  if (status != UMI_STATUS_OK || response->status != 200 ||
      response->body_length != sizeof(expected) - 1U ||
      memcmp(response->body, expected, sizeof(expected) - 1U) != 0)
    entry_failed = 1;
  ++entry_dispatched;
  out->dispatched = 1;
  out->response_complete = !entry_failed;
  free(request);
  free(response);
  return status;
}
int main(int argc, char **argv) {
  int result = LocalServiceEntry(argc, argv);
  if (entry_failed || (result == 0 && (entry_created != 1 || entry_dispatched != 1)))
    return 1;
  if (result == 2 && (entry_created != 0 || entry_dispatched != 0))
    return 1;
  return result;
}
