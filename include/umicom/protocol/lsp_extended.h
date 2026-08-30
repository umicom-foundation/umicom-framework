/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/lsp_extended.h
 *
 * PURPOSE:
 *   Expose the modern Language Server Protocol requests used by Umicom Studio
 *   without leaking JSON construction into applications or UI adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROTOCOL_LSP_EXTENDED_H
#define UMICOM_PROTOCOL_LSP_EXTENDED_H

#include "umicom/protocol/lsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLspRange {
    UmiLspPosition start;
    UmiLspPosition end;
} UmiLspRange;

UmiStatus umi_lsp_did_close(UmiLspClient *client, const char *uri);
UmiStatus umi_lsp_did_save(UmiLspClient *client, const char *uri,
                           const char *text);
UmiStatus umi_lsp_hover(UmiLspClient *client, const char *uri,
                        UmiLspPosition position, int64_t *out_request_id);
UmiStatus umi_lsp_signature_help(UmiLspClient *client, const char *uri,
                                 UmiLspPosition position,
                                 int64_t *out_request_id);
UmiStatus umi_lsp_definition(UmiLspClient *client, const char *uri,
                             UmiLspPosition position,
                             int64_t *out_request_id);
UmiStatus umi_lsp_implementation(UmiLspClient *client, const char *uri,
                                 UmiLspPosition position,
                                 int64_t *out_request_id);
UmiStatus umi_lsp_type_definition(UmiLspClient *client, const char *uri,
                                  UmiLspPosition position,
                                  int64_t *out_request_id);
UmiStatus umi_lsp_references(UmiLspClient *client, const char *uri,
                             UmiLspPosition position,
                             int include_declaration,
                             int64_t *out_request_id);
UmiStatus umi_lsp_document_symbols(UmiLspClient *client, const char *uri,
                                   int64_t *out_request_id);
UmiStatus umi_lsp_workspace_symbols(UmiLspClient *client, const char *query,
                                    int64_t *out_request_id);
UmiStatus umi_lsp_rename(UmiLspClient *client, const char *uri,
                         UmiLspPosition position, const char *new_name,
                         int64_t *out_request_id);
UmiStatus umi_lsp_prepare_rename(UmiLspClient *client, const char *uri,
                                 UmiLspPosition position,
                                 int64_t *out_request_id);
UmiStatus umi_lsp_formatting(UmiLspClient *client, const char *uri,
                             uint32_t tab_size, int insert_spaces,
                             int64_t *out_request_id);
UmiStatus umi_lsp_range_formatting(UmiLspClient *client, const char *uri,
                                   UmiLspRange range, uint32_t tab_size,
                                   int insert_spaces,
                                   int64_t *out_request_id);
UmiStatus umi_lsp_code_action(UmiLspClient *client, const char *uri,
                              UmiLspRange range,
                              int64_t *out_request_id);
UmiStatus umi_lsp_semantic_tokens_full(UmiLspClient *client, const char *uri,
                                       int64_t *out_request_id);
UmiStatus umi_lsp_inlay_hints(UmiLspClient *client, const char *uri,
                              UmiLspRange range,
                              int64_t *out_request_id);
UmiStatus umi_lsp_folding_ranges(UmiLspClient *client, const char *uri,
                                 int64_t *out_request_id);
UmiStatus umi_lsp_document_highlights(UmiLspClient *client, const char *uri,
                                      UmiLspPosition position,
                                      int64_t *out_request_id);
UmiStatus umi_lsp_execute_command(UmiLspClient *client, const char *command,
                                  const char *arguments_json,
                                  int64_t *out_request_id);

#ifdef __cplusplus
}
#endif
#endif
