/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json_tree.c
 * PURPOSE: Parse owned JSON with bounded heap growth and direct sibling traversal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_text.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
typedef struct JsonNode
{
    UmiLanguageRuntimeJsonTokenType kind;
    size_t begin, end, children;
    int first, last, next;
} JsonNode;
struct UmiJsonTree
{
    char *text;
    JsonNode *nodes;
    size_t count, capacity, length;
};
typedef struct JsonParse
{
    UmiJsonTree *tree;
    UmiJsonTreeLimits limits;
    const UmiCancellationToken *cancel;
    size_t at;
} JsonParse;
UmiJsonTreeLimits UmiJsonTreeDefaultLimits(void)
{
    return (UmiJsonTreeLimits){16U * 1024U * 1024U, 1024U * 1024U, 128U};
}
static bool JsonValid(const UmiJsonTree *tree, int node)
{
    return tree != NULL && node >= 0 && (size_t)node < tree->count;
}
static bool JsonSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
static bool JsonDigit(char c) { return c >= '0' && c <= '9'; }
static UmiStatus JsonWhitespace(JsonParse *parse)
{
    while (parse->at < parse->tree->length && JsonSpace(parse->tree->text[parse->at]))
    {
        if ((parse->at & 4095U) == 0U && umi_cancellation_token_is_requested(parse->cancel))
            return UMI_STATUS_CANCELLED;
        ++parse->at;
    }
    return UMI_STATUS_OK;
}
/* Node indices survive reallocations. Never retain a pointer to a node across
 * an insertion; that rule keeps parent/sibling links valid as the tree grows. */
static UmiStatus JsonAppend(JsonParse *parse, int parent, UmiLanguageRuntimeJsonTokenType kind, int *out)
{
    UmiJsonTree *tree = parse->tree;
    if (tree->count == parse->limits.nodes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (tree->count == tree->capacity)
    {
        size_t capacity = tree->capacity == 0U ? 64U : tree->capacity * 2U;
        if (capacity > parse->limits.nodes)
            capacity = parse->limits.nodes;
        if (capacity > SIZE_MAX / sizeof(*tree->nodes))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        JsonNode *nodes = realloc(tree->nodes, capacity * sizeof(*nodes));
        if (nodes == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        tree->nodes = nodes;
        tree->capacity = capacity;
    }
    int node = (int)tree->count++;
    tree->nodes[node] = (JsonNode){kind, parse->at, parse->at, 0U, -1, -1, -1};
    if (parent >= 0)
    {
        int last = tree->nodes[parent].last;
        if (last >= 0)
            tree->nodes[last].next = node;
        else
            tree->nodes[parent].first = node;
        tree->nodes[parent].last = node;
        ++tree->nodes[parent].children;
    }
    *out = node;
    return UMI_STATUS_OK;
}
static UmiStatus JsonString(JsonParse *parse, int parent)
{
    size_t begin = ++parse->at;
    bool escaped = false;
    while (parse->at < parse->tree->length)
    {
        if ((parse->at & 4095U) == 0U && umi_cancellation_token_is_requested(parse->cancel))
            return UMI_STATUS_CANCELLED;
        char c = parse->tree->text[parse->at];
        if (!escaped && c == '"')
        {
            size_t measured;
            UmiStatus status = UmiLanguageRuntimeJsonTextSpan(parse->tree->text + begin, parse->at - begin,
                                                              NULL, 0U, &measured);
            if (status != UMI_STATUS_OK)
                return status;
            int node;
            status = JsonAppend(parse, parent, UMI_LANGUAGE_RUNTIME_JSON_STRING, &node);
            if (status != UMI_STATUS_OK)
                return status;
            parse->tree->nodes[node].begin = begin;
            parse->tree->nodes[node].end = parse->at++;
            return UMI_STATUS_OK;
        }
        if (escaped)
            escaped = false;
        else if (c == '\\')
            escaped = true;
        ++parse->at;
    }
    return UMI_STATUS_PARSE_ERROR;
}
static bool JsonLiteral(const char *s, size_t length)
{
    if ((length == 4U && (memcmp(s, "true", 4U) == 0 || memcmp(s, "null", 4U) == 0)) ||
        (length == 5U && memcmp(s, "false", 5U) == 0))
        return true;
    size_t at = 0U;
    if (at < length && s[at] == '-')
        ++at;
    if (at == length)
        return false;
    if (s[at] == '0')
        ++at;
    else
    {
        if (s[at] < '1' || s[at] > '9')
            return false;
        do
        {
            ++at;
        } while (at < length && JsonDigit(s[at]));
    }
    if (at < length && s[at] == '.')
    {
        size_t first = ++at;
        while (at < length && JsonDigit(s[at]))
            ++at;
        if (at == first)
            return false;
    }
    if (at < length && (s[at] == 'e' || s[at] == 'E'))
    {
        ++at;
        if (at < length && (s[at] == '+' || s[at] == '-'))
            ++at;
        size_t first = at;
        while (at < length && JsonDigit(s[at]))
            ++at;
        if (at == first)
            return false;
    }
    return at == length;
}
static UmiStatus JsonPrimitive(JsonParse *parse, int parent)
{
    size_t begin = parse->at;
    while (parse->at < parse->tree->length)
    {
        char c = parse->tree->text[parse->at];
        if (JsonSpace(c) || c == ',' || c == ']' || c == '}')
            break;
        if ((parse->at & 4095U) == 0U && umi_cancellation_token_is_requested(parse->cancel))
            return UMI_STATUS_CANCELLED;
        ++parse->at;
    }
    if (!JsonLiteral(parse->tree->text + begin, parse->at - begin))
        return UMI_STATUS_PARSE_ERROR;
    int node;
    UmiStatus status = JsonAppend(parse, parent, UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE, &node);
    if (status == UMI_STATUS_OK)
    {
        parse->tree->nodes[node].begin = begin;
        parse->tree->nodes[node].end = parse->at;
    }
    return status;
}
static UmiStatus JsonValue(JsonParse *parse, int parent, size_t depth);
static UmiStatus JsonContainer(JsonParse *parse, int parent, size_t depth, bool object)
{
    if (depth >= parse->limits.depth)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int node;
    UmiStatus status = JsonAppend(
        parse, parent, object ? UMI_LANGUAGE_RUNTIME_JSON_OBJECT : UMI_LANGUAGE_RUNTIME_JSON_ARRAY, &node);
    if (status != UMI_STATUS_OK)
        return status;
    ++parse->at;
    status = JsonWhitespace(parse);
    if (status != UMI_STATUS_OK)
        return status;
    char close = object ? '}' : ']';
    if (parse->at < parse->tree->length && parse->tree->text[parse->at] == close)
    {
        parse->tree->nodes[node].end = ++parse->at;
        return UMI_STATUS_OK;
    }
    for (;;)
    {
        if (object)
        {
            if (parse->at == parse->tree->length || parse->tree->text[parse->at] != '"')
                return UMI_STATUS_PARSE_ERROR;
            status = JsonString(parse, node);
            if (status != UMI_STATUS_OK)
                return status;
            status = JsonWhitespace(parse);
            if (status != UMI_STATUS_OK)
                return status;
            if (parse->at == parse->tree->length || parse->tree->text[parse->at++] != ':')
                return UMI_STATUS_PARSE_ERROR;
        }
        status = JsonValue(parse, node, depth + 1U);
        if (status != UMI_STATUS_OK)
            return status;
        status = JsonWhitespace(parse);
        if (status != UMI_STATUS_OK)
            return status;
        if (parse->at == parse->tree->length)
            return UMI_STATUS_PARSE_ERROR;
        char separator = parse->tree->text[parse->at++];
        if (separator == close)
        {
            parse->tree->nodes[node].end = parse->at;
            return UMI_STATUS_OK;
        }
        if (separator != ',')
            return UMI_STATUS_PARSE_ERROR;
        status = JsonWhitespace(parse);
        if (status != UMI_STATUS_OK)
            return status;
        /* A following closing delimiter is not a value: trailing commas fail. */
    }
}
static UmiStatus JsonValue(JsonParse *parse, int parent, size_t depth)
{
    if (umi_cancellation_token_is_requested(parse->cancel))
        return UMI_STATUS_CANCELLED;
    UmiStatus status = JsonWhitespace(parse);
    if (status != UMI_STATUS_OK)
        return status;
    if (parse->at == parse->tree->length)
        return UMI_STATUS_PARSE_ERROR;
    switch (parse->tree->text[parse->at])
    {
    case '{':
        return JsonContainer(parse, parent, depth, true);
    case '[':
        return JsonContainer(parse, parent, depth, false);
    case '"':
        return JsonString(parse, parent);
    default:
        return JsonPrimitive(parse, parent);
    }
}
UmiStatus UmiJsonTreeCreate(const void *bytes, size_t length, const UmiJsonTreeLimits *limits,
                            const UmiCancellationToken *cancel, UmiJsonTree **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTreeLimits policy = limits != NULL ? *limits : UmiJsonTreeDefaultLimits();
    if (policy.bytes == 0U || policy.bytes > 64U * 1024U * 1024U || policy.nodes == 0U ||
        policy.nodes > 2U * 1024U * 1024U || policy.depth == 0U || policy.depth > 256U ||
        policy.nodes > (size_t)INT_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > policy.bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiJsonTree *tree = calloc(1U, sizeof(*tree));
    if (tree == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    tree->text = malloc(length + 1U);
    if (tree->text == NULL)
    {
        free(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(tree->text, bytes, length);
    tree->text[length] = '\0';
    tree->length = length;
    JsonParse parse = {tree, policy, cancel, 0U};
    UmiStatus status = JsonValue(&parse, -1, 0U);
    if (status == UMI_STATUS_OK)
        status = JsonWhitespace(&parse);
    if (status == UMI_STATUS_OK && parse.at != length)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = tree;
    else
        UmiJsonTreeDestroy(tree);
    return status;
}
void UmiJsonTreeDestroy(UmiJsonTree *tree)
{
    if (tree != NULL)
    {
        free(tree->nodes);
        free(tree->text);
        free(tree);
    }
}
UmiLanguageRuntimeJsonTokenType UmiJsonTreeKind(const UmiJsonTree *tree, int node)
{
    return JsonValid(tree, node) ? tree->nodes[node].kind : UMI_LANGUAGE_RUNTIME_JSON_UNDEFINED;
}
int UmiJsonTreeFirst(const UmiJsonTree *tree, int node)
{
    return JsonValid(tree, node) ? tree->nodes[node].first : -1;
}
int UmiJsonTreeNext(const UmiJsonTree *tree, int node)
{
    return JsonValid(tree, node) ? tree->nodes[node].next : -1;
}
size_t UmiJsonTreeCount(const UmiJsonTree *tree, int node)
{
    return !JsonValid(tree, node)
               ? 0U
               : tree->nodes[node].children /
                     (tree->nodes[node].kind == UMI_LANGUAGE_RUNTIME_JSON_OBJECT ? 2U : 1U);
}
UmiStatus UmiJsonTreeText(const UmiJsonTree *tree, int node, char *out, size_t capacity)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const JsonNode *value = &tree->nodes[node];
    size_t decoded;
    return UmiLanguageRuntimeJsonTextSpan(tree->text + value->begin, value->end - value->begin, out, capacity,
                                          &decoded);
}
UmiStatus UmiJsonTreeMember(const UmiJsonTree *tree, int object, const char *name, int *out)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT || name == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length <= 4096U && name[length] != '\0')
        ++length;
    if (length > 4096U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A bounded lookup key needs no allocation for every metadata field. */
    char decoded[4097];
    int found = -1;
    UmiStatus status = UMI_STATUS_NOT_FOUND;
    for (int key = UmiJsonTreeFirst(tree, object); key >= 0;)
    {
        int value = UmiJsonTreeNext(tree, key);
        UmiStatus decode = UmiJsonTreeText(tree, key, decoded, length + 1U);
        if (decode == UMI_STATUS_OK && strcmp(decoded, name) == 0)
        {
            if (found >= 0)
            {
                status = UMI_STATUS_ALREADY_EXISTS;
                break;
            }
            found = value;
            status = UMI_STATUS_OK;
        }
        else if (decode != UMI_STATUS_OK && decode != UMI_STATUS_CAPACITY_EXCEEDED)
        {
            status = decode;
            break;
        }
        key = UmiJsonTreeNext(tree, value);
    }
    if (status == UMI_STATUS_OK)
        *out = found;
    return status;
}
UmiStatus UmiJsonTreeInteger(const UmiJsonTree *tree, int node, int64_t *out)
{
/* The former guard conflated invalid arguments with a valid JSON value of the wrong type. The separate checks preserve caller diagnostics and protocol parse errors; retain the original guard for review. */
#if 0
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    /* Invalid handles are caller mistakes. A valid JSON value of the wrong
     * type is malformed protocol input, so report a parse error consistently
     * with fractional numbers and non-boolean primitives. Never change out. */
    if (!JsonValid(tree, node) || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
        return UMI_STATUS_PARSE_ERROR;
    const JsonNode *value = &tree->nodes[node];
    const char *text = tree->text + value->begin;
    size_t length = value->end - value->begin, at = 0U;
    bool negative = length != 0U && text[0] == '-';
    if (negative)
        ++at;
    if (at == length)
        return UMI_STATUS_PARSE_ERROR;
    uint64_t magnitude = 0U, limit = negative ? (uint64_t)INT64_MAX + 1U : (uint64_t)INT64_MAX;
    for (; at < length; ++at)
    {
        if (!JsonDigit(text[at]))
            return UMI_STATUS_PARSE_ERROR;
        unsigned digit = (unsigned)(text[at] - '0');
        if (magnitude > (limit - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        magnitude = magnitude * 10U + digit;
    }
    *out = negative ? (magnitude == (uint64_t)INT64_MAX + 1U ? INT64_MIN : -(int64_t)magnitude)
                    : (int64_t)magnitude;
    return UMI_STATUS_OK;
}
UmiStatus UmiJsonTreeBoolean(const UmiJsonTree *tree, int node, int *out)
{
/* The former guard conflated invalid arguments with a valid JSON value of the wrong type. The separate checks preserve caller diagnostics and protocol parse errors; retain the original guard for review. */
#if 0
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    /* Invalid handles are caller mistakes. A valid JSON value of the wrong
     * type is malformed protocol input, so report a parse error consistently
     * with fractional numbers and non-boolean primitives. Never change out. */
    if (!JsonValid(tree, node) || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
        return UMI_STATUS_PARSE_ERROR;
    const JsonNode *value = &tree->nodes[node];
    size_t length = value->end - value->begin;
    const char *text = tree->text + value->begin;
    if (length == 4U && memcmp(text, "true", 4U) == 0)
    {
        *out = 1;
        return UMI_STATUS_OK;
    }
    if (length == 5U && memcmp(text, "false", 5U) == 0)
    {
        *out = 0;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

int UmiJsonTreeIsNull(const UmiJsonTree *tree, int node)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
        return 0;
    const JsonNode *value = &tree->nodes[node];
    return value->end - value->begin == 4U && memcmp(tree->text + value->begin, "null", 4U) == 0;
}

UmiStatus UmiJsonTreeSourceSpan(const UmiJsonTree *tree, int node, const char **out_bytes, size_t *out_length)
{
    if (!JsonValid(tree, node) || out_bytes == NULL || out_length == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const JsonNode *value = &tree->nodes[node];
    size_t begin = value->begin, end = value->end;
    /* String nodes store only their escaped contents for decoded-text access.
     * Their validated surrounding quotes belong to the raw JSON value. */
    if (value->kind == UMI_LANGUAGE_RUNTIME_JSON_STRING)
    {
        --begin;
        ++end;
    }
    *out_bytes = tree->text + begin;
    *out_length = end - begin;
    return UMI_STATUS_OK;
}

#include "json_tree_compare.inc"
