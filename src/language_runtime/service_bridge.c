/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/service_bridge.c
 *
 * PURPOSE:
 *   Project LSP results into existing Language Service registries without parallel Studio models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/service_bridge.h"
#include "umicom/base/text.h"
#include <stdio.h>
#include <string.h>
/* The earlier clear-then-upsert publishers are retained for engineering
 * review. A later conversion or capacity failure could erase the last good
 * list; reference publication also cleared other documents. The replacement
 * stages checked values and publishes through document-scoped Framework
 * transactions. Bridge revisions now advance only after accepted publication. */
#if 0
/* Provide the ok operation used by this module and its client applications. */
static UmiStatus ok(UmiLanguageRuntimeServiceBridge *b) {
  return b && b->language ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Initialise language runtime service bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_service_bridge_init(UmiLanguageRuntimeServiceBridge *b,
                                                   UmiLanguageService *l) {
  /* Apply this branch only when its contract condition is satisfied. */
  if (!b || !l)
    return UMI_STATUS_INVALID_ARGUMENT;
  b->language = l;
  b->revision = 1;
  return UMI_STATUS_OK;
}
/* Provide the clear completion operation used by this module and its client applications. */
static void clear_completion(UmiLanguageCompletionRegistry *r, const char *d) {
  size_t i = umi_language_completion_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageCompletionSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_completion_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_completion_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish completion operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_completion(UmiLanguageRuntimeServiceBridge *b, const char *d,
                                                  uint32_t line, uint32_t col,
                                                  const UmiLanguageRuntimeCompletionResult *r) {
  size_t i;
  UmiLanguageCompletionRegistry *g;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_completion(b->language);
  clear_completion(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageCompletionSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_COMPLETION_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.completion.%zu", d, i);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.label, sizeof(x.label), "%s", r->items[i].label);
    (void)umi_text_format(x.detail, sizeof(x.detail), "%s", r->items[i].detail);
    (void)umi_text_format(x.insert_text, sizeof(x.insert_text), "%s", r->items[i].insert_text);
    (void)umi_text_format(x.kind, sizeof(x.kind), "%d", r->items[i].kind);
    (void)umi_text_format(x.sort_text, sizeof(x.sort_text), "%s", r->items[i].sort_text);
    x.line = line;
    x.column = col;
    x.revision = b->revision + 1;
    s = umi_language_completion_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/* Provide the clear hover operation used by this module and its client applications. */
static void clear_hover(UmiLanguageHoverRegistry *r, const char *d) {
  size_t i = umi_language_hover_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageHoverSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_hover_registry_at(r, i, &x) == UMI_STATUS_OK && strcmp(x.document_id, d) == 0)
      (void)umi_language_hover_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish hover operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_publish_hover(UmiLanguageRuntimeServiceBridge *b, const char *d,
                                             uint32_t line, uint32_t col,
                                             const UmiLanguageRuntimeHoverResult *r) {
  UmiLanguageHoverSnapshot x = {0};
  UmiLanguageHoverRegistry *g;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_hover(b->language);
  clear_hover(g, d);
  /* Apply this branch only when its contract condition is satisfied. */
  if (!r->contents[0])
    return UMI_STATUS_OK;
  x.struct_size = sizeof(x);
  x.api_version = UMI_LANGUAGE_HOVER_API_VERSION;
  (void)umi_text_format(x.id, sizeof(x.id), "%s.hover", d);
  (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
  (void)umi_text_format(x.contents, sizeof(x.contents), "%s", r->contents);
  x.line = line;
  x.column = col;
  /* Apply this branch only when its contract condition is satisfied. */
  if (r->has_range) {
    x.start_line = r->range.start.line;
    x.start_column = r->range.start.character;
    x.end_line = r->range.end.line;
    x.end_column = r->range.end.character;
  }
  x.revision = ++b->revision;
  return umi_language_hover_registry_upsert(g, &x);
}
/* Provide the clear sig operation used by this module and its client applications. */
static void clear_sig(UmiLanguageSignatureRegistry *r, const char *d) {
  size_t i = umi_language_signature_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageSignatureSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_signature_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_signature_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish signature operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_signature(UmiLanguageRuntimeServiceBridge *b, const char *d,
                                                 uint32_t line, uint32_t col,
                                                 const UmiLanguageRuntimeSignatureResult *r) {
  UmiLanguageSignatureSnapshot x = {0};
  UmiLanguageSignatureRegistry *g;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_signature(b->language);
  clear_sig(g, d);
  /* Apply this branch only when its contract condition is satisfied. */
  if (!r->available)
    return UMI_STATUS_OK;
  x.struct_size = sizeof(x);
  x.api_version = UMI_LANGUAGE_SIGNATURE_API_VERSION;
  (void)umi_text_format(x.id, sizeof(x.id), "%s.signature", d);
  (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
  (void)umi_text_format(x.label, sizeof(x.label), "%s", r->label);
  (void)umi_text_format(x.documentation, sizeof(x.documentation), "%s", r->documentation);
  x.active_parameter = r->active_parameter;
  x.line = line;
  x.column = col;
  x.revision = ++b->revision;
  return umi_language_signature_registry_upsert(g, &x);
}
/*
 * Provide the language runtime publish locations operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_locations(UmiLanguageRuntimeServiceBridge *b, const char *d,
                                                 const char *sid, int def,
                                                 const UmiLanguageRuntimeLocationList *r) {
  UmiLanguageReferenceRegistry *g;
  size_t i;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !sid || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_reference(b->language);
  umi_language_reference_registry_clear(g);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageReferenceSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_REFERENCE_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.reference.%zu", d, i);
    (void)umi_text_format(x.symbol_id, sizeof(x.symbol_id), "%s", sid);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.uri, sizeof(x.uri), "%s", r->items[i].uri);
    x.line = r->items[i].range.start.line;
    x.column = r->items[i].range.start.character;
    x.definition = def;
    x.revision = b->revision + 1;
    s = umi_language_reference_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/* Provide the clear symbols operation used by this module and its client applications. */
static void clear_symbols(UmiLanguageSymbolRegistry *r, const char *d) {
  size_t i = umi_language_symbol_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageSymbolSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_symbol_registry_at(r, i, &x) == UMI_STATUS_OK && strcmp(x.document_id, d) == 0)
      (void)umi_language_symbol_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish symbols operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_symbols(UmiLanguageRuntimeServiceBridge *b, const char *d,
                                               const UmiLanguageRuntimeSymbolList *r) {
  UmiLanguageSymbolRegistry *g;
  size_t i;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_symbol(b->language);
  clear_symbols(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageSymbolSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_SYMBOL_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.symbol.%zu", d, i);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.name, sizeof(x.name), "%s", r->items[i].name);
    (void)umi_text_format(x.kind, sizeof(x.kind), "%d", r->items[i].kind);
    (void)umi_text_format(x.container, sizeof(x.container), "%s", r->items[i].container);
    x.line = r->items[i].range.start.line;
    x.column = r->items[i].range.start.character;
    x.end_line = r->items[i].range.end.line;
    x.end_column = r->items[i].range.end.character;
    x.revision = b->revision + 1;
    s = umi_language_symbol_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}

/* Provide the clear diag operation used by this module and its client applications. */
static void clear_diag(UmiLanguageDiagnosticRegistry *r, const char *d) {
  size_t i = umi_language_diagnostic_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageDiagnosticSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_diagnostic_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_diagnostic_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish diagnostics operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_diagnostics(UmiLanguageRuntimeServiceBridge *b,
                                                   const char *d,
                                                   const UmiLanguageRuntimeDiagnosticList *r) {
  UmiLanguageDiagnosticRegistry *g;
  size_t i;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_diagnostic(b->language);
  clear_diag(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageDiagnosticSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_DIAGNOSTIC_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.diagnostic.%zu", d, i);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.message, sizeof(x.message), "%s", r->items[i].message);
    (void)umi_text_format(x.source, sizeof(x.source), "%s", r->items[i].source);
    (void)umi_text_format(x.code, sizeof(x.code), "%s", r->items[i].code);
    x.severity = r->items[i].severity;
    x.line = r->items[i].range.start.line;
    x.column = r->items[i].range.start.character;
    x.end_line = r->items[i].range.end.line;
    x.end_column = r->items[i].range.end.character;
    x.revision = b->revision + 1;
    s = umi_language_diagnostic_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/* Provide the clear actions operation used by this module and its client applications. */
static void clear_actions(UmiLanguageCodeActionRegistry *r, const char *d) {
  size_t i = umi_language_code_action_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageCodeActionSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_code_action_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_code_action_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish code actions operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_code_actions(UmiLanguageRuntimeServiceBridge *b,
                                                    const char *d,
                                                    const UmiLanguageRuntimeCodeActionList *r) {
  UmiLanguageCodeActionRegistry *g;
  size_t i;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_code_action(b->language);
  clear_actions(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageCodeActionSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_CODE_ACTION_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.code-action.%zu", d, i);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.title, sizeof(x.title), "%s", r->items[i].title);
    (void)umi_text_format(x.kind, sizeof(x.kind), "%s", r->items[i].kind);
    (void)umi_text_format(x.command_id, sizeof(x.command_id), "%s", r->items[i].command);
    x.preferred = r->items[i].preferred;
    x.revision = b->revision + 1;
    s = umi_language_code_action_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/* Provide the clear sem operation used by this module and its client applications. */
static void clear_sem(UmiLanguageSemanticTokenRegistry *r, const char *d) {
  size_t i = umi_language_semantic_token_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageSemanticTokenSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_semantic_token_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_semantic_token_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish semantic tokens operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_publish_semantic_tokens(UmiLanguageRuntimeServiceBridge *b,
                                                       const char *d,
                                                       const UmiLanguageRuntimeSemanticTokens *r) {
  UmiLanguageSemanticTokenRegistry *g;
  size_t i;
  uint32_t line = 0, col = 0;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_semantic_token(b->language);
  clear_sem(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i + 4 < r->count; i += 5) {
    UmiLanguageSemanticTokenSnapshot x = {0};
    UmiStatus s;
    uint32_t dl = r->data[i], ds = r->data[i + 1];
    /* Apply this branch only when its contract condition is satisfied. */
    if (dl) {
      line += dl;
      col = ds;
    } /* Use this fallback path when the earlier condition does not apply. */ else
      col += ds;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_SEMANTIC_TOKEN_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.semantic.%zu", d, i / 5);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.token_type, sizeof(x.token_type), "type-%u", r->data[i + 3]);
    (void)umi_text_format(x.modifiers, sizeof(x.modifiers), "0x%08x", r->data[i + 4]);
    x.line = line;
    x.column = col;
    x.length = r->data[i + 2];
    x.revision = b->revision + 1;
    s = umi_language_semantic_token_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/* Provide the clear hint operation used by this module and its client applications. */
static void clear_hint(UmiLanguageInlayHintRegistry *r, const char *d) {
  size_t i = umi_language_inlay_hint_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageInlayHintSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_inlay_hint_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_inlay_hint_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish inlay hints operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_inlay_hints(UmiLanguageRuntimeServiceBridge *b,
                                                   const char *d,
                                                   const UmiLanguageRuntimeInlayHintList *r) {
  UmiLanguageInlayHintRegistry *g;
  size_t i;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_inlay_hint(b->language);
  clear_hint(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageInlayHintSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_INLAY_HINT_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.inlay.%zu", d, i);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.label, sizeof(x.label), "%s", r->items[i].label);
    (void)umi_text_format(x.kind, sizeof(x.kind), "%d", r->items[i].kind);
    x.line = r->items[i].position.line;
    x.column = r->items[i].position.character;
    x.visible = 1;
    x.revision = b->revision + 1;
    s = umi_language_inlay_hint_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/* Provide the clear fold operation used by this module and its client applications. */
static void clear_fold(UmiLanguageFoldingRangeRegistry *r, const char *d) {
  size_t i = umi_language_folding_range_registry_count(r);
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (i--) {
    UmiLanguageFoldingRangeSnapshot x;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_language_folding_range_registry_at(r, i, &x) == UMI_STATUS_OK &&
        strcmp(x.document_id, d) == 0)
      (void)umi_language_folding_range_registry_remove(r, x.id);
  }
}
/*
 * Provide the language runtime publish folding ranges operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_publish_folding_ranges(UmiLanguageRuntimeServiceBridge *b,
                                                      const char *d,
                                                      const UmiLanguageRuntimeFoldingRangeList *r) {
  UmiLanguageFoldingRangeRegistry *g;
  size_t i;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !r)
    return UMI_STATUS_INVALID_ARGUMENT;
  g = umi_language_service_folding_range(b->language);
  clear_fold(g, d);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0; i < r->count; i++) {
    UmiLanguageFoldingRangeSnapshot x = {0};
    UmiStatus s;
    x.struct_size = sizeof(x);
    x.api_version = UMI_LANGUAGE_FOLDING_RANGE_API_VERSION;
    (void)umi_text_format(x.id, sizeof(x.id), "%s.fold.%zu", d, i);
    (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
    (void)umi_text_format(x.kind, sizeof(x.kind), "%s", r->items[i].kind);
    x.start_line = r->items[i].start_line;
    x.end_line = r->items[i].end_line;
    x.collapsed = 0;
    x.revision = b->revision + 1;
    s = umi_language_folding_range_registry_upsert(g, &x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK)
      return s;
  }
  b->revision++;
  return UMI_STATUS_OK;
}
/*
 * Provide the language runtime publish formatting available operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_publish_formatting_available(UmiLanguageRuntimeServiceBridge *b,
                                                            const char *d, const char *p,
                                                            uint32_t tab, int spaces) {
  UmiLanguageFormattingSnapshot x = {0};
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !p || !tab)
    return UMI_STATUS_INVALID_ARGUMENT;
  x.struct_size = sizeof(x);
  x.api_version = UMI_LANGUAGE_FORMATTING_API_VERSION;
  (void)umi_text_format(x.id, sizeof(x.id), "%s.formatting", d);
  (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
  (void)umi_text_format(x.provider_id, sizeof(x.provider_id), "%s", p);
  (void)umi_text_format(x.mode, sizeof(x.mode), "document");
  x.tab_size = tab;
  x.insert_spaces = spaces != 0;
  x.available = 1;
  x.revision = ++b->revision;
  return umi_language_formatting_registry_upsert(umi_language_service_formatting(b->language), &x);
}
/*
 * Provide the language runtime publish rename operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_publish_rename(UmiLanguageRuntimeServiceBridge *b, const char *d,
                                              const char *sid, const char *old, const char *newn,
                                              const UmiLanguageRuntimeWorkspaceEdit *e) {
  UmiLanguageRenameSnapshot x = {0};
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (ok(b) != UMI_STATUS_OK || !d || !sid || !old || !newn || !e)
    return UMI_STATUS_INVALID_ARGUMENT;
  x.struct_size = sizeof(x);
  x.api_version = UMI_LANGUAGE_RENAME_API_VERSION;
  (void)umi_text_format(x.id, sizeof(x.id), "%s.rename", d);
  (void)umi_text_format(x.symbol_id, sizeof(x.symbol_id), "%s", sid);
  (void)umi_text_format(x.old_name, sizeof(x.old_name), "%s", old);
  (void)umi_text_format(x.new_name, sizeof(x.new_name), "%s", newn);
  (void)umi_text_format(x.document_id, sizeof(x.document_id), "%s", d);
  x.state = e->count ? 1 : 0;
  x.conflict_count = 0;
  x.revision = ++b->revision;
  return umi_language_rename_registry_upsert(umi_language_service_rename(b->language), &x);
}

#endif

#include "publication_internal.h"

static UmiStatus publication_start(UmiLanguageRuntimeServiceBridge *bridge, const char *document_id)
{
    if (bridge == NULL || bridge->language == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (bridge->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_language_publication_document(document_id,
        sizeof(((UmiLanguageDiagnosticSnapshot *)0)->document_id));
}

UmiStatus umi_language_runtime_service_bridge_init(UmiLanguageRuntimeServiceBridge *b, UmiLanguageService *l)
{
    if (b == NULL || l == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    b->language = l; b->revision = 1U;
    return UMI_STATUS_OK;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_completion(UmiLanguageRuntimeServiceBridge *b, const char *d,
    uint32_t line, uint32_t col, const UmiLanguageRuntimeCompletionResult *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageCompletionSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageCompletionRegistry *registry = umi_language_service_completion(b->language);
    const uint64_t expected = umi_language_completion_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageCompletionSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.completion.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->label, sizeof(x->label), r->items[i].label, sizeof(r->items[i].label));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->detail, sizeof(x->detail), r->items[i].detail, sizeof(r->items[i].detail));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->insert_text, sizeof(x->insert_text), r->items[i].insert_text, sizeof(r->items[i].insert_text));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->sort_text, sizeof(x->sort_text), r->items[i].sort_text, sizeof(r->items[i].sort_text));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_text_format(x->kind, sizeof(x->kind), "%d", r->items[i].kind);
        if (status != UMI_STATUS_OK) goto done;
        x->line = line;
        x->column = col;
    }
    status = umi_language_completion_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_locations(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const char *sid, int def, const UmiLanguageRuntimeLocationList *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageReferenceSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (sid == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageReferenceRegistry *registry = umi_language_service_reference(b->language);
    const uint64_t expected = umi_language_reference_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageReferenceSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.reference.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->symbol_id, sizeof(x->symbol_id), sid, sizeof(x->symbol_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->uri, sizeof(x->uri), r->items[i].uri, sizeof(r->items[i].uri));
        if (status != UMI_STATUS_OK) goto done;
        x->line = r->items[i].range.start.line;
        x->column = r->items[i].range.start.character;
        x->definition = def;
    }
    status = umi_language_reference_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_symbols(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const UmiLanguageRuntimeSymbolList *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageSymbolSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageSymbolRegistry *registry = umi_language_service_symbol(b->language);
    const uint64_t expected = umi_language_symbol_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageSymbolSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.symbol.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->name, sizeof(x->name), r->items[i].name, sizeof(r->items[i].name));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->container, sizeof(x->container), r->items[i].container, sizeof(r->items[i].container));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_text_format(x->kind, sizeof(x->kind), "%d", r->items[i].kind);
        if (status != UMI_STATUS_OK) goto done;
        x->line = r->items[i].range.start.line;
        x->column = r->items[i].range.start.character;
        x->end_line = r->items[i].range.end.line;
        x->end_column = r->items[i].range.end.character;
    }
    status = umi_language_symbol_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_diagnostics(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const UmiLanguageRuntimeDiagnosticList *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageDiagnosticSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageDiagnosticRegistry *registry = umi_language_service_diagnostic(b->language);
    const uint64_t expected = umi_language_diagnostic_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageDiagnosticSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.diagnostic.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->message, sizeof(x->message), r->items[i].message, sizeof(r->items[i].message));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->source, sizeof(x->source), r->items[i].source, sizeof(r->items[i].source));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->code, sizeof(x->code), r->items[i].code, sizeof(r->items[i].code));
        if (status != UMI_STATUS_OK) goto done;
        x->severity = r->items[i].severity;
        x->line = r->items[i].range.start.line;
        x->column = r->items[i].range.start.character;
        x->end_line = r->items[i].range.end.line;
        x->end_column = r->items[i].range.end.character;
    }
    status = umi_language_diagnostic_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_code_actions(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const UmiLanguageRuntimeCodeActionList *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageCodeActionSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageCodeActionRegistry *registry = umi_language_service_code_action(b->language);
    const uint64_t expected = umi_language_code_action_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageCodeActionSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.code-action.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->title, sizeof(x->title), r->items[i].title, sizeof(r->items[i].title));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->kind, sizeof(x->kind), r->items[i].kind, sizeof(r->items[i].kind));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->command_id, sizeof(x->command_id), r->items[i].command, sizeof(r->items[i].command));
        if (status != UMI_STATUS_OK) goto done;
        x->preferred = r->items[i].preferred;
    }
    status = umi_language_code_action_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_inlay_hints(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const UmiLanguageRuntimeInlayHintList *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageInlayHintSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageInlayHintRegistry *registry = umi_language_service_inlay_hint(b->language);
    const uint64_t expected = umi_language_inlay_hint_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageInlayHintSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.inlay.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->label, sizeof(x->label), r->items[i].label, sizeof(r->items[i].label));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_text_format(x->kind, sizeof(x->kind), "%d", r->items[i].kind);
        if (status != UMI_STATUS_OK) goto done;
        x->line = r->items[i].position.line;
        x->column = r->items[i].position.character;
        x->visible = 1;
    }
    status = umi_language_inlay_hint_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

/* Convert the complete response before replacing this document's list. */
UmiStatus umi_language_runtime_publish_folding_ranges(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const UmiLanguageRuntimeFoldingRangeList *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageFoldingRangeSnapshot *items = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->items) / sizeof(r->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageFoldingRangeRegistry *registry = umi_language_service_folding_range(b->language);
    const uint64_t expected = umi_language_folding_range_registry_revision(registry);
    if (r->count != 0U) {
        items = calloc(r->count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < r->count; ++i) {
        UmiLanguageFoldingRangeSnapshot *x = &items[i];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.fold.%zu", d, i);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->kind, sizeof(x->kind), r->items[i].kind, sizeof(r->items[i].kind));
        if (status != UMI_STATUS_OK) goto done;
        x->start_line = r->items[i].start_line;
        x->end_line = r->items[i].end_line;
        x->collapsed = 0;
    }
    status = umi_language_folding_range_registry_replace_document(registry, d, expected, items, r->count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

UmiStatus umi_language_runtime_publish_hover(UmiLanguageRuntimeServiceBridge *b, const char *d,
    uint32_t line, uint32_t col, const UmiLanguageRuntimeHoverResult *r)
{
    UmiLanguageHoverSnapshot item = {0};
    UmiStatus status = publication_start(b, d);
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageHoverRegistry *registry = umi_language_service_hover(b->language);
    const uint64_t expected = umi_language_hover_registry_revision(registry);
    const size_t count = r->contents[0] != '\0' ? 1U : 0U;
    if (count != 0U) {
        item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
        status = umi_text_format(item.id, sizeof(item.id), "%s.hover", d);
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_publication_copy(item.document_id, sizeof(item.document_id), d, sizeof(item.document_id));
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_publication_copy(item.contents, sizeof(item.contents), r->contents, sizeof(r->contents));
        if (status != UMI_STATUS_OK) return status;
        item.line = line;
        item.column = col;
        if (r->has_range) {
            item.start_line = r->range.start.line; item.start_column = r->range.start.character;
            item.end_line = r->range.end.line; item.end_column = r->range.end.character;
        }
    }
    status = umi_language_hover_registry_replace_document(registry, d, expected, &item, count, NULL);
    /* Preserve the former empty-result bridge revision behavior. The registry
     * still records any successful document-only removal. */
    if (status == UMI_STATUS_OK && count != 0U) ++b->revision;
    return status;
}

UmiStatus umi_language_runtime_publish_signature(UmiLanguageRuntimeServiceBridge *b, const char *d,
    uint32_t line, uint32_t col, const UmiLanguageRuntimeSignatureResult *r)
{
    UmiLanguageSignatureSnapshot item = {0};
    UmiStatus status = publication_start(b, d);
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageSignatureRegistry *registry = umi_language_service_signature(b->language);
    const uint64_t expected = umi_language_signature_registry_revision(registry);
    const size_t count = r->available != 0 ? 1U : 0U;
    if (count != 0U) {
        item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
        status = umi_text_format(item.id, sizeof(item.id), "%s.signature", d);
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_publication_copy(item.document_id, sizeof(item.document_id), d, sizeof(item.document_id));
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_publication_copy(item.label, sizeof(item.label), r->label, sizeof(r->label));
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_publication_copy(item.documentation, sizeof(item.documentation), r->documentation, sizeof(r->documentation));
        if (status != UMI_STATUS_OK) return status;
        item.line = line;
        item.column = col;
        item.active_parameter = r->active_parameter;
    }
    status = umi_language_signature_registry_replace_document(registry, d, expected, &item, count, NULL);
    /* Preserve the former empty-result bridge revision behavior. The registry
     * still records any successful document-only removal. */
    if (status == UMI_STATUS_OK && count != 0U) ++b->revision;
    return status;
}

/* Incomplete groups and arithmetic overflow cannot become plausible tokens. */
UmiStatus umi_language_runtime_publish_semantic_tokens(UmiLanguageRuntimeServiceBridge *b,
    const char *d, const UmiLanguageRuntimeSemanticTokens *r)
{
    UmiStatus status = publication_start(b, d);
    UmiLanguageSemanticTokenSnapshot *items = NULL;
    uint32_t line = 0U, column = 0U;
    if (status != UMI_STATUS_OK) return status;
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (r->count > sizeof(r->data) / sizeof(r->data[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (r->count % 5U != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t count = r->count / 5U;
    UmiLanguageSemanticTokenRegistry *registry = umi_language_service_semantic_token(b->language);
    const uint64_t expected = umi_language_semantic_token_registry_revision(registry);
    if (count != 0U) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t row = 0U; row < count; ++row) {
        const size_t i = row * 5U;
        const uint32_t delta = r->data[i], start = r->data[i + 1U];
        UmiLanguageSemanticTokenSnapshot *x = &items[row];
        if (line > UINT32_MAX - delta || (delta == 0U && column > UINT32_MAX - start)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; goto done;
        }
        line += delta; column = delta != 0U ? start : column + start;
        if (r->data[i + 2U] > UINT32_MAX - column) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_text_format(x->id, sizeof(x->id), "%s.semantic.%zu", d, row);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), d, sizeof(x->document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_text_format(x->token_type, sizeof(x->token_type), "type-%u", r->data[i + 3U]);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_text_format(x->modifiers, sizeof(x->modifiers), "0x%08x", r->data[i + 4U]);
        if (status != UMI_STATUS_OK) goto done;
        x->line = line; x->column = column; x->length = r->data[i + 2U];
    }
    status = umi_language_semantic_token_registry_replace_document(registry, d, expected, items, count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items); return status;
}

UmiStatus umi_language_runtime_publish_formatting_available(UmiLanguageRuntimeServiceBridge *b,
    const char *d, const char *p, uint32_t tab, int spaces)
{
    UmiLanguageFormattingSnapshot item = {0};
    UmiStatus status = publication_start(b, d);
    if (status != UMI_STATUS_OK) return status;
    if (p == NULL || tab == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
    status = umi_text_format(item.id, sizeof(item.id), "%s.formatting", d);
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.document_id, sizeof(item.document_id), d, sizeof(item.document_id));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.provider_id, sizeof(item.provider_id), p, sizeof(item.provider_id));
    if (status != UMI_STATUS_OK) return status;
    memcpy(item.mode, "document", sizeof("document"));
    item.tab_size = tab; item.insert_spaces = spaces != 0; item.available = 1;
    status = umi_language_formatting_registry_upsert(umi_language_service_formatting(b->language), &item);
    if (status == UMI_STATUS_OK) ++b->revision;
    return status;
}

UmiStatus umi_language_runtime_publish_rename(UmiLanguageRuntimeServiceBridge *b, const char *d,
    const char *sid, const char *old, const char *newn, const UmiLanguageRuntimeWorkspaceEdit *e)
{
    UmiLanguageRenameSnapshot item = {0};
    UmiStatus status = publication_start(b, d);
    if (status != UMI_STATUS_OK) return status;
    if (sid == NULL || old == NULL || newn == NULL || e == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (e->count > sizeof(e->items) / sizeof(e->items[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
    status = umi_text_format(item.id, sizeof(item.id), "%s.rename", d);
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.document_id, sizeof(item.document_id), d, sizeof(item.document_id));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.symbol_id, sizeof(item.symbol_id), sid, sizeof(item.symbol_id));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.old_name, sizeof(item.old_name), old, sizeof(item.old_name));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.new_name, sizeof(item.new_name), newn, sizeof(item.new_name));
    if (status != UMI_STATUS_OK) return status;
    item.state = e->count != 0U ? 1 : 0; item.conflict_count = 0;
    status = umi_language_rename_registry_upsert(umi_language_service_rename(b->language), &item);
    if (status == UMI_STATUS_OK) ++b->revision;
    return status;
}
