/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/json_tree.h
 * PURPOSE: Own larger JSON documents under explicit byte, node and depth limits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_JSON_TREE_H
#define UMICOM_LANGUAGE_RUNTIME_JSON_TREE_H
#include <stdint.h>
#include "umicom/language_runtime/json.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiJsonTree UmiJsonTree;
    typedef struct UmiJsonTreeLimits
    {
        size_t bytes, nodes, depth;
    } UmiJsonTreeLimits;
    /* Defaults: 16 MiB, 1,048,576 nodes, 128 containers. Caller limits can be
 * smaller; hard ceilings are 64 MiB, 2,097,152 nodes and 256 containers. A node
 * is one value or object key. Memory is proportional to owned bytes and nodes.
 * The fixed-size JSON document/token interfaces remain available unchanged. */
    UmiJsonTreeLimits UmiJsonTreeDefaultLimits(void);
    /* Copy and validate exactly length bytes, including all uninterpreted fields.
 * No trailing NUL is required; an embedded NUL is rejected. Reject malformed
 * grammar, non-JSON whitespace, invalid UTF-8 and unpaired surrogate escapes.
 * Decoded NUL text is intentionally unsupported to match Framework C strings.
 * Cancellation is cooperative between tokens and during scanning; allocation,
 * copying and an individual string decode are not interruptible. Keep the
 * token alive for this call. Failure clears out; no partial tree is published. */
    UmiStatus UmiJsonTreeCreate(const void *bytes, size_t length, const UmiJsonTreeLimits *limits,
                                const UmiCancellationToken *cancel, UmiJsonTree **out);
    void UmiJsonTreeDestroy(UmiJsonTree *tree);
    /* Root is node 0. Handles are valid only in their owning immutable tree.
 * Invalid handles return UNDEFINED, -1 or zero. First/Next traverse direct
 * children in constant time; object children alternate key and value.
 * Count returns array elements or object members, not descendant nodes. */
    UmiLanguageRuntimeJsonTokenType UmiJsonTreeKind(const UmiJsonTree *tree, int node);
    int UmiJsonTreeFirst(const UmiJsonTree *tree, int node);
    int UmiJsonTreeNext(const UmiJsonTree *tree, int node);
    size_t UmiJsonTreeCount(const UmiJsonTree *tree, int node);
    /* Resolve one decoded object member name (up to 4096 UTF-8 bytes). Missing
 * gives NOT_FOUND, duplicate occurrences of that name give ALREADY_EXISTS.
 * Unrelated duplicates are retained, allowing consumers to choose policy.
 * Output remains unchanged on failure. Iterate First/Next to inspect all keys. */
    UmiStatus UmiJsonTreeMember(const UmiJsonTree *tree, int object, const char *name, int *out);
    UmiStatus UmiJsonTreeText(const UmiJsonTree *tree, int node, char *out, size_t capacity);
    /* Integer requires integer spelling and range; no float coercion or saturation.
 * Typed readers leave their caller-owned output unchanged on failure. */
    /* Wrong value kinds return PARSE_ERROR; invalid handles or NULL outputs
     * return INVALID_ARGUMENT. Integer overflow returns CAPACITY_EXCEEDED. */
    UmiStatus UmiJsonTreeInteger(const UmiJsonTree *tree, int node, int64_t *out);
    UmiStatus UmiJsonTreeBoolean(const UmiJsonTree *tree, int node, int *out);
    /* Distinguish a JSON null from a missing member without coercing numbers,
     * booleans or the string "null". Invalid tree/node handles return false. */
    int UmiJsonTreeIsNull(const UmiJsonTree *tree, int node);
    /* Borrow the original JSON spelling of a value, including quotes for a
     * string and brackets/braces for a container. The span is not NUL-terminated
     * and lives until tree destruction. This lets protocol consumers pass a
     * validated result to another bounded reader without rebuilding its JSON.
     * Failure leaves both outputs unchanged. Do not modify borrowed bytes. */
    UmiStatus UmiJsonTreeSourceSpan(const UmiJsonTree *tree, int node, const char **out_bytes,
                                    size_t *out_length);

    /* Compare two owned values without relying on object order or string escape
 * spelling. Arrays retain order; strings compare decoded UTF-8. Numbers compare
 * exact JSON spelling, so 1 and 1.0 differ: no floating-point coercion is used
 * for opaque provider identities. Duplicate keys in compared objects return
 * ALREADY_EXISTS. An unequal earlier value may short-circuit later traversal.
 *
 * Tree parse limits bound input size/depth. Temporary object-key tables and
 * decoded strings are allocated proportionally to the visited values. Checks
 * are cooperative between values and key decodes; qsort/string decoding are
 * not interruptible. Failure leaves out_equal unchanged; inputs are immutable. */
    UmiStatus UmiJsonTreeValuesEqual(const UmiJsonTree *left, int left_node, const UmiJsonTree *right,
                                     int right_node, const UmiCancellationToken *cancel, int *out_equal);
#ifdef __cplusplus
}
#endif
#endif
