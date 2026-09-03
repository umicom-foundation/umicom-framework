/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/providers/c_language_assistance.c
 *
 * PURPOSE:
 *   Implement deterministic, offline C completions with signatures, concise
 *   explanations and safer-use notes. The provider complements richer
 *   language servers instead of replacing them.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/providers/c_language_assistance.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorCLanguageAssistance {
    UmiEditorCompletionProviderRegistry *providers;
    UmiEditorCompletionOrchestration *completion;
    UmiEditorQuickDocumentation *documentation;
    UmiEditorSignatureHelpModel *signatures;
};

/* Keep the initial catalogue deliberately focused on common constructs and
 * safety-sensitive library calls. New entries can be added without changing
 * the provider API or Studio code. */
static const UmiEditorCLanguageAssistanceEntry ENTRIES[] = {
    {
        "c.keyword.if", "if", "if (condition)",
        "if (${1:condition}) {\n    ${2:/* work */}\n}", "if", "keyword",
        "Runs a block only when its condition is true. Keep the condition simple and use braces so later edits cannot accidentally escape the block.",
        "umicom://c-language/control-flow#if", "control-flow", 1, 0
    },
    {
        "c.keyword.for", "for", "for (initialise; condition; advance)",
        "for (${1:size_t index = 0U}; ${2:index < count}; ${3:++index}) {\n    ${4:/* work */}\n}",
        "for", "keyword",
        "Repeats a block while its condition is true. Check the upper bound before indexing an array and use a size-compatible counter.",
        "umicom://c-language/control-flow#for", "bounds", 1, 0
    },
    {
        "c.keyword.while", "while", "while (condition)",
        "while (${1:condition}) {\n    ${2:/* advance state */}\n}", "while", "keyword",
        "Repeats a block while its condition stays true. Make sure the body changes the state used by the condition so the loop can finish.",
        "umicom://c-language/control-flow#while", "control-flow", 1, 0
    },
    {
        "c.keyword.switch", "switch", "switch (value)",
        "switch (${1:value}) {\n    case ${2:0}:\n        ${3:break;}\n    default:\n        break;\n}",
        "switch", "keyword",
        "Chooses one branch from a value. Include a default branch and use an intentional break or return to prevent accidental fall-through.",
        "umicom://c-language/control-flow#switch", "control-flow", 1, 0
    },
    {
        "c.keyword.struct", "struct", "struct TypeName", "struct ${1:TypeName} {\n    ${2:int value;}\n};",
        "struct", "keyword",
        "Groups related values into one type. Document which code owns pointer fields and which functions initialise and release the structure.",
        "umicom://c-language/data#struct", "ownership", 1, 0
    },
    {
        "c.type.size_t", "size_t", "unsigned size and index type", "size_t", "size_t", "type",
        "Use size_t for object sizes, byte counts and array indexes. Check arithmetic before adding or multiplying values used for allocation.",
        "umicom://c-language/types#size-t", "integer-safety", 0, 0
    },
    {
        "c.operator.sizeof", "sizeof", "sizeof expression or type", "sizeof(${1:value})", "sizeof", "operator",
        "Reports an object's size in bytes. Prefer sizeof *pointer when allocating for a pointer because it stays correct if the pointed-to type changes.",
        "umicom://c-language/types#sizeof", "allocation", 1, 0
    },
    {
        "c.keyword.static-assert", "static_assert", "static_assert(condition, message)",
        "static_assert(${1:condition}, \"${2:explanation}\");", "static_assert", "keyword",
        "Checks an assumption while compiling. Use it for structure sizes, array relationships and other rules that must never depend on runtime input.",
        "umicom://c-language/contracts#static-assert", "contracts", 1, 0
    },
    {
        "c.function.snprintf", "snprintf", "int snprintf(char *buffer, size_t capacity, const char *format, ...)",
        "snprintf(${1:buffer}, sizeof(${1:buffer}), \"${2:%s}\", ${3:value})", "snprintf", "function",
        "Formats text within a stated capacity. Treat a negative return as an error and a return greater than or equal to the capacity as truncation.",
        "umicom://c-language/stdio#snprintf", "buffer-safety", 1, 0
    },
    {
        "c.function.memcpy", "memcpy", "void *memcpy(void *destination, const void *source, size_t count)",
        "memcpy(${1:destination}, ${2:source}, ${3:byte_count})", "memcpy", "function",
        "Copies a known number of bytes between non-overlapping regions. Prove that both regions contain at least count bytes before calling it.",
        "umicom://c-language/string#memcpy", "buffer-safety", 1, 0
    },
    {
        "c.function.memmove", "memmove", "void *memmove(void *destination, const void *source, size_t count)",
        "memmove(${1:destination}, ${2:source}, ${3:byte_count})", "memmove", "function",
        "Copies bytes safely when source and destination may overlap. The caller must still verify both capacities and the byte count.",
        "umicom://c-language/string#memmove", "buffer-safety", 1, 0
    },
    {
        "c.function.calloc", "calloc", "void *calloc(size_t count, size_t element_size)",
        "calloc(${1:count}, sizeof(${2:*pointer}))", "calloc", "function",
        "Allocates zero-initialised storage. Check that count times element size cannot overflow, test the returned pointer, and record who must free it.",
        "umicom://c-language/memory#calloc", "ownership", 1, 0
    },
    {
        "c.function.realloc", "realloc", "void *realloc(void *pointer, size_t new_size)",
        "realloc(${1:pointer}, ${2:new_size})", "realloc", "function",
        "Changes an allocation's size. Store the result in a temporary pointer so a failure does not lose the original allocation.",
        "umicom://c-language/memory#realloc", "ownership", 1, 0
    },
    {
        "c.function.free", "free", "void free(void *pointer)", "free(${1:pointer})", "free", "function",
        "Releases dynamically allocated storage exactly once. Clear the owning pointer when that makes later accidental reuse easier to detect.",
        "umicom://c-language/memory#free", "ownership", 1, 0
    },
    {
        "c.function.strtol", "strtol", "long strtol(const char *text, char **end, int base)",
        "strtol(${1:text}, &${2:end}, ${3:10})", "strtol", "function",
        "Converts text while reporting where parsing stopped. Clear errno first, inspect the end pointer and range, then convert to a narrower type only after bounds checks.",
        "umicom://c-language/conversion#strtol", "input-validation", 1, 0
    },
    {
        "c.function.fopen", "fopen", "FILE *fopen(const char *path, const char *mode)",
        "fopen(${1:path}, \"${2:r}\")", "fopen", "function",
        "Opens a file and returns NULL on failure. Validate untrusted paths, use the least-powerful mode, and close every successful handle on every exit path.",
        "umicom://c-language/files#fopen", "resource-lifetime", 1, 0
    },
    {
        "c.function.fclose", "fclose", "int fclose(FILE *stream)", "fclose(${1:stream})", "fclose", "function",
        "Flushes and closes a stream. Check the result when written data matters and never use the stream pointer after it has been closed.",
        "umicom://c-language/files#fclose", "resource-lifetime", 1, 0
    },
    {
        "c.function.assert", "assert", "void assert(scalar_expression)", "assert(${1:condition})", "assert", "function",
        "Documents and checks an internal programming assumption in debug builds. Do not use assert to validate user input or to perform work with side effects.",
        "umicom://c-language/testing#assert", "testing", 1, 0
    },
    {
        "c.header.stdio", "stdio.h", "standard input and output declarations", "<stdio.h>", "stdio.h", "header",
        "Declares FILE, formatted input and output, and stream operations. Prefer bounded formatting and always check operations that can fail.",
        "umicom://c-language/headers#stdio", "input-output", 0, 1
    },
    {
        "c.header.stdlib", "stdlib.h", "allocation, conversion and process declarations", "<stdlib.h>", "stdlib.h", "header",
        "Declares allocation, numeric conversion and process utilities. Pair every owned resource with a clear release path.",
        "umicom://c-language/headers#stdlib", "ownership", 0, 1
    },
    {
        "c.header.stdint", "stdint.h", "fixed-width integer declarations", "<stdint.h>", "stdint.h", "header",
        "Declares exact-width and minimum-width integer types. Choose types from the data contract and check conversions at boundaries.",
        "umicom://c-language/headers#stdint", "integer-safety", 0, 1
    },
    {
        "c.header.stddef", "stddef.h", "size, difference and null pointer declarations", "<stddef.h>", "stddef.h", "header",
        "Declares size_t, ptrdiff_t and other fundamental types used when working with object sizes and pointer differences.",
        "umicom://c-language/headers#stddef", "types", 0, 1
    },
    {
        "c.header.string", "string.h", "byte and string operation declarations", "<string.h>", "string.h", "header",
        "Declares byte and character-string operations. C strings require a terminating null byte, so track capacity separately from current length.",
        "umicom://c-language/headers#string", "buffer-safety", 0, 1
    }
};

/* Copy a trusted catalogue string into a bounded public field. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /* A missing buffer or source would make even a zero-length copy unsafe. */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Refuse truncation because a shortened symbol identifier can collide. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Join two trusted strings only when the complete result fits. */
static UmiStatus join_text(char *destination,
                           size_t capacity,
                           const char *left,
                           const char *right)
{
    size_t left_length;
    size_t right_length;

    /* Validate all inputs before measuring or writing them. */
    if (destination == NULL || capacity == 0U || left == NULL || right == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    left_length = strlen(left);
    right_length = strlen(right);
    /* Subtraction avoids overflowing while checking the combined length. */
    if (left_length >= capacity || right_length >= capacity - left_length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, left, left_length);
    (void)memcpy(destination + left_length, right, right_length + 1U);
    return UMI_STATUS_OK;
}

/* Decide whether the cursor is completing an include directive rather than C code. */
static int is_include_context(const char *line_prefix)
{
    const char *cursor = line_prefix;

    if (cursor == NULL) return 0;
    /* C permits spacing before the directive marker and after it. */
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    if (*cursor != '#') return 0;
    ++cursor;
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    if (strncmp(cursor, "include", 7U) != 0) return 0;
    cursor += 7U;
    /* A boundary check prevents another identifier such as included_file from
     * being mistaken for the include directive. */
    return *cursor == '\0' || *cursor == ' ' || *cursor == '\t' ||
           *cursor == '<' || *cursor == '"';
}

/* Match only the visible beginning of a label so results remain predictable. */
static int matches_prefix(const char *label, const char *prefix)
{
    size_t prefix_length;

    if (label == NULL || prefix == NULL) return 0;
    prefix_length = strlen(prefix);
    return prefix_length == 0U || strncmp(label, prefix, prefix_length) == 0;
}

/* Describe the fixed parameters shown by the first offline C signature set.
 * Variadic arguments are represented in the label but are not a fixed slot. */
static size_t signature_parameter_count(const char *entry_id)
{
    if (entry_id == NULL) return 0U;
    if (strcmp(entry_id, "c.function.snprintf") == 0) return 3U;
    if (strcmp(entry_id, "c.function.memcpy") == 0 ||
        strcmp(entry_id, "c.function.memmove") == 0 ||
        strcmp(entry_id, "c.function.strtol") == 0) {
        return 3U;
    }
    if (strcmp(entry_id, "c.function.calloc") == 0 ||
        strcmp(entry_id, "c.function.realloc") == 0 ||
        strcmp(entry_id, "c.function.fopen") == 0) {
        return 2U;
    }
    return 1U;
}

/* Supply the return type separately so a frontend can style it without
 * parsing the human-readable signature label. */
static const char *signature_return_type(const char *entry_id)
{
    if (entry_id == NULL) return "";
    if (strcmp(entry_id, "c.function.snprintf") == 0 ||
        strcmp(entry_id, "c.function.fclose") == 0) {
        return "int";
    }
    if (strcmp(entry_id, "c.function.memcpy") == 0 ||
        strcmp(entry_id, "c.function.memmove") == 0 ||
        strcmp(entry_id, "c.function.calloc") == 0 ||
        strcmp(entry_id, "c.function.realloc") == 0) {
        return "void *";
    }
    if (strcmp(entry_id, "c.function.strtol") == 0) return "long";
    if (strcmp(entry_id, "c.function.fopen") == 0) return "FILE *";
    return "void";
}

/* Convert one immutable catalogue record into the public completion value. */
static UmiStatus build_candidate(
    const UmiEditorCLanguageAssistanceEntry *entry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidate *out_candidate)
{
    UmiStatus status;
    size_t prefix_length;

    if (entry == NULL || provider_id == NULL || request == NULL ||
        out_candidate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_candidate, 0, sizeof(*out_candidate));
    out_candidate->struct_size = (uint32_t)sizeof(*out_candidate);
    out_candidate->api_version = UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION;
    out_candidate->item.struct_size = (uint32_t)sizeof(out_candidate->item);
    out_candidate->item.api_version = 1U;

    status = copy_text(out_candidate->item.id,
                       sizeof(out_candidate->item.id), entry->id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->item.document_id,
                           sizeof(out_candidate->item.document_id),
                           request->document_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->item.label,
                           sizeof(out_candidate->item.label), entry->label);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->item.detail,
                           sizeof(out_candidate->item.detail), entry->detail);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_candidate->item.insert_text,
            sizeof(out_candidate->item.insert_text),
            entry->snippet && !request->allow_snippets
                ? entry->plain_insert_text : entry->insert_text);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->item.kind,
                           sizeof(out_candidate->item.kind), entry->kind);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->item.sort_text,
                           sizeof(out_candidate->item.sort_text), entry->label);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->item.filter_text,
                           sizeof(out_candidate->item.filter_text), entry->label);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->provider_id,
                           sizeof(out_candidate->provider_id), provider_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->documentation,
                           sizeof(out_candidate->documentation),
                           entry->documentation);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(out_candidate->commit_characters,
                           sizeof(out_candidate->commit_characters), "();, ");
    }
    if (status != UMI_STATUS_OK) return status;

    prefix_length = strlen(request->prefix);
    /* Avoid unsigned underflow if a caller supplies inconsistent cursor data. */
    out_candidate->replace_start_offset = request->cursor_offset >= prefix_length
        ? request->cursor_offset - prefix_length : request->cursor_offset;
    out_candidate->replace_end_offset = request->cursor_offset;
    out_candidate->document_revision = request->document_revision;
    out_candidate->request_id = request->request_id;
    out_candidate->locality_score = 100;
    out_candidate->usage_score = entry->header ? 20 : 40;
    out_candidate->insert_format = entry->snippet && request->allow_snippets
        ? UMI_EDITOR_COMPLETION_INSERT_SNIPPET
        : UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT;
    out_candidate->source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
    /* Leave selection to the common ranking layer so several entries cannot
     * claim to be the preferred result merely because their names match. */
    out_candidate->flags = 0U;
    return UMI_STATUS_OK;
}

/* Supply prefix-filtered C records through the common provider callback. */
static UmiStatus provide_completions(
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidateSink sink,
    void *sink_user_data,
    UmiEditorCompletionProviderResponse *out_response,
    void *provider_user_data)
{
    const char *provider_id = (const char *)provider_user_data;
    int include_context;
    size_t index;

    /* Validate the request before reading its line prefix. Provider callbacks
     * may also be exercised directly by tests and extension hosts. */
    if (request == NULL || sink == NULL || out_response == NULL ||
        provider_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    include_context = is_include_context(request->line_prefix);
    /* Visit a bounded static catalogue, stopping when the caller's limit is reached. */
    for (index = 0U; index < umi_editor_c_language_assistance_count(); ++index) {
        const UmiEditorCLanguageAssistanceEntry *entry = &ENTRIES[index];
        UmiEditorCompletionCandidate candidate;
        UmiStatus status;

        /* Cancellation is checked between records so automatic completion stays responsive. */
        if (umi_editor_completion_request_is_cancelled(request)) {
            return UMI_STATUS_CANCELLED;
        }
        /* Include lines receive headers; ordinary code receives language constructs. */
        if (entry->header != include_context ||
            !matches_prefix(entry->label, request->prefix)) {
            continue;
        }
        if (out_response->emitted_candidate_count >= request->maximum_candidates) {
            out_response->incomplete = 1;
            break;
        }
        status = build_candidate(entry, provider_id, request, &candidate);
        if (status != UMI_STATUS_OK) return status;
        status = sink(&candidate, sink_user_data);
        if (status != UMI_STATUS_OK) return status;
        ++out_response->emitted_candidate_count;
    }
    out_response->provider_revision = 1U;
    out_response->cacheable = 1;
    return UMI_STATUS_OK;
}

/* Register one language identifier with the same stateless provider logic. */
static UmiStatus register_language(
    UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    const char *language_id)
{
    UmiEditorCompletionProviderDescriptor descriptor;
    UmiEditorCompletionProviderCallbacks callbacks;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    (void)copy_text(descriptor.id, sizeof(descriptor.id), provider_id);
    (void)copy_text(descriptor.label, sizeof(descriptor.label),
                    "Built-in C language assistance");
    (void)copy_text(descriptor.language_id, sizeof(descriptor.language_id),
                    language_id);
    descriptor.priority = 40;
    descriptor.source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
    descriptor.supports_manual_invocation = 1;
    descriptor.supports_automatic_invocation = 1;
    descriptor.supports_incomplete_refresh = 1;
    descriptor.supports_commit_characters = 1;
    descriptor.supports_snippets = 1;
    descriptor.enabled = 1;

    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.struct_size = (uint32_t)sizeof(callbacks);
    callbacks.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    callbacks.provide = provide_completions;
    return umi_editor_completion_provider_registry_register(
        registry, &descriptor, &callbacks, (void *)provider_id);
}

/* Return the immutable catalogue size without exposing its storage. */
size_t umi_editor_c_language_assistance_count(void)
{
    return sizeof(ENTRIES) / sizeof(ENTRIES[0]);
}

/* Return one immutable record only when the requested position exists. */
const UmiEditorCLanguageAssistanceEntry *
umi_editor_c_language_assistance_at(size_t index)
{
    return index < umi_editor_c_language_assistance_count()
        ? &ENTRIES[index] : NULL;
}

/* Find an immutable record by stable identifier. */
const UmiEditorCLanguageAssistanceEntry *
umi_editor_c_language_assistance_find(const char *entry_id)
{
    size_t index;

    if (entry_id == NULL || entry_id[0] == '\0') return NULL;
    for (index = 0U; index < umi_editor_c_language_assistance_count(); ++index) {
        if (strcmp(ENTRIES[index].id, entry_id) == 0) return &ENTRIES[index];
    }
    return NULL;
}

/* Make both common C document identifiers available to the shared orchestrator. */
UmiStatus umi_editor_c_language_assistance_register(
    UmiEditorCompletionProviderRegistry *registry)
{
    UmiStatus status;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = register_language(registry, UMI_EDITOR_C_COMPLETION_PROVIDER_ID, "c");
    if (status != UMI_STATUS_OK) return status;
    status = register_language(
        registry, UMI_EDITOR_C23_COMPLETION_PROVIDER_ID, "c23");
    /* Roll back the first registration so callers never receive half a provider pair. */
    if (status != UMI_STATUS_OK) {
        (void)umi_editor_completion_provider_registry_unregister(
            registry, UMI_EDITOR_C_COMPLETION_PROVIDER_ID);
    }
    return status;
}

/* Seed trusted quick documentation from the exact same records as completion. */
UmiStatus umi_editor_c_language_assistance_populate_documentation(
    UmiEditorQuickDocumentation *documentation)
{
    size_t index;

    if (documentation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < umi_editor_c_language_assistance_count(); ++index) {
        const UmiEditorCLanguageAssistanceEntry *entry = &ENTRIES[index];
        UmiEditorQuickDocumentationSection section;
        UmiStatus status;

        (void)memset(&section, 0, sizeof(section));
        section.struct_size = (uint32_t)sizeof(section);
        section.api_version = UMI_EDITOR_QUICK_DOCUMENTATION_API_VERSION;
        status = join_text(section.id, sizeof(section.id), "c.documentation.",
                           entry->id);
        if (status == UMI_STATUS_OK) {
            status = copy_text(section.symbol_id, sizeof(section.symbol_id),
                               entry->label);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(section.provider_id,
                               sizeof(section.provider_id),
                               UMI_EDITOR_C_COMPLETION_PROVIDER_ID);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(section.title, sizeof(section.title),
                               entry->detail);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(section.language_id,
                               sizeof(section.language_id), "c");
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(section.content, sizeof(section.content),
                               entry->documentation);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(section.source_uri,
                               sizeof(section.source_uri), entry->source_uri);
        }
        if (status == UMI_STATUS_OK) {
            status = umi_editor_source_location_initialize(
                &section.location, entry->source_uri, 0U, 0U);
        }
        if (status != UMI_STATUS_OK) return status;
        section.location.kind = UMI_EDITOR_SOURCE_LOCATION_SYMBOL;
        (void)copy_text(section.location.symbol_id,
                        sizeof(section.location.symbol_id), entry->label);
        section.section_kind = UMI_EDITOR_DOCUMENTATION_SUMMARY;
        section.content_kind = UMI_EDITOR_DOCUMENTATION_PLAIN_TEXT;
        section.order = 10;
        section.trusted = 1;
        section.visible = 1;
        status = umi_editor_quick_documentation_upsert(
            documentation, &section);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_editor_quick_documentation_sort(documentation);
}

/* Seed callable signatures from the same catalogue used by completion so the
 * displayed parameters and safety notes cannot drift into separate copies. */
UmiStatus umi_editor_c_language_assistance_populate_signatures(
    UmiEditorSignatureHelpModel *signatures)
{
    size_t index;

    if (signatures == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < umi_editor_c_language_assistance_count(); ++index) {
        const UmiEditorCLanguageAssistanceEntry *entry = &ENTRIES[index];
        UmiEditorSignatureHelpItem item;
        UmiStatus status;

        /* Keywords, operators, types and headers are not callable signatures. */
        if (strcmp(entry->kind, "function") != 0) continue;
        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION;
        status = copy_text(item.id, sizeof(item.id), entry->id);
        if (status == UMI_STATUS_OK) {
            status = copy_text(item.provider_id, sizeof(item.provider_id),
                               UMI_EDITOR_C_COMPLETION_PROVIDER_ID);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(item.label, sizeof(item.label), entry->detail);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(item.documentation,
                               sizeof(item.documentation),
                               entry->documentation);
        }
        if (status == UMI_STATUS_OK) {
            status = copy_text(item.return_type, sizeof(item.return_type),
                               signature_return_type(entry->id));
        }
        if (status == UMI_STATUS_OK) {
            status = umi_editor_source_location_initialize(
                &item.location, entry->source_uri, 0U, 0U);
        }
        if (status != UMI_STATUS_OK) return status;
        item.location.kind = UMI_EDITOR_SOURCE_LOCATION_SYMBOL;
        status = copy_text(item.location.symbol_id,
                           sizeof(item.location.symbol_id), entry->label);
        if (status != UMI_STATUS_OK) return status;
        item.parameter_count = signature_parameter_count(entry->id);
        item.score = 100;
        item.visible = 1;
        status = umi_editor_signature_help_model_upsert(signatures, &item);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_editor_signature_help_model_sort(signatures);
}

/* Build the complete offline feature through one owner so applications cannot
 * accidentally activate completion while omitting its matching explanations. */
UmiStatus umi_editor_c_language_assistance_create(
    UmiEditorCLanguageAssistance **out_assistance)
{
    UmiEditorCLanguageAssistance *assistance;
    UmiStatus status;

    if (out_assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_assistance = NULL;
    assistance = (UmiEditorCLanguageAssistance *)calloc(
        1U, sizeof(*assistance));
    if (assistance == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_completion_provider_registry_create(
        &assistance->providers);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_c_language_assistance_register(
            assistance->providers);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_completion_orchestration_create(
            assistance->providers, &assistance->completion);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_quick_documentation_create(
            &assistance->documentation);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_c_language_assistance_populate_documentation(
            assistance->documentation);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_signature_help_model_create(
            &assistance->signatures);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_c_language_assistance_populate_signatures(
            assistance->signatures);
    }
    /* A single failure path makes partial creation safe for every client. */
    if (status != UMI_STATUS_OK) {
        umi_editor_c_language_assistance_destroy(assistance);
        return status;
    }
    *out_assistance = assistance;
    return UMI_STATUS_OK;
}

/* Destroy children in reverse creation order; every destroy accepts NULL so
 * this also handles a partially constructed owner. */
void umi_editor_c_language_assistance_destroy(
    UmiEditorCLanguageAssistance *assistance)
{
    if (assistance == NULL) return;
    umi_editor_signature_help_model_destroy(assistance->signatures);
    umi_editor_quick_documentation_destroy(assistance->documentation);
    umi_editor_completion_orchestration_destroy(assistance->completion);
    umi_editor_completion_provider_registry_destroy(assistance->providers);
    free(assistance);
}

/* Gather independent value snapshots so a panel never borrows internal arrays
 * while another provider is being coordinated by its owner. */
UmiStatus umi_editor_c_language_assistance_snapshot(
    const UmiEditorCLanguageAssistance *assistance,
    UmiEditorCLanguageAssistanceSnapshot *out_snapshot)
{
    UmiStatus status;

    if (assistance == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_C_LANGUAGE_ASSISTANCE_API_VERSION;
    status = umi_editor_completion_provider_registry_snapshot(
        assistance->providers, &out_snapshot->completion_providers);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_quick_documentation_snapshot(
            assistance->documentation, &out_snapshot->documentation);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_signature_help_model_snapshot(
            assistance->signatures, &out_snapshot->signatures);
    }
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->available = 1;
    return UMI_STATUS_OK;
}

/* Borrow the registry while keeping its lifetime tied to the assistance owner. */
UmiEditorCompletionProviderRegistry *
umi_editor_c_language_assistance_providers(
    UmiEditorCLanguageAssistance *assistance)
{
    return assistance != NULL ? assistance->providers : NULL;
}

/* Borrow the completion coordinator without transferring ownership. */
UmiEditorCompletionOrchestration *
umi_editor_c_language_assistance_completion(
    UmiEditorCLanguageAssistance *assistance)
{
    return assistance != NULL ? assistance->completion : NULL;
}

/* Borrow quick documentation without transferring ownership. */
UmiEditorQuickDocumentation *
umi_editor_c_language_assistance_documentation(
    UmiEditorCLanguageAssistance *assistance)
{
    return assistance != NULL ? assistance->documentation : NULL;
}

/* Borrow signature help without transferring ownership. */
UmiEditorSignatureHelpModel *
umi_editor_c_language_assistance_signatures(
    UmiEditorCLanguageAssistance *assistance)
{
    return assistance != NULL ? assistance->signatures : NULL;
}
