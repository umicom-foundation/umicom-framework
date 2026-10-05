/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json_tree_compare.inc
 * PURPOSE: Compare immutable JSON values without treating object order or string escaping as identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct JsonCompareMember
{
    char *name;
    int value;
} JsonCompareMember;
static void JsonCompareMembersFree(JsonCompareMember *members, size_t count)
{
    if (members != NULL)
        for (size_t i = 0U; i < count; ++i)
            free(members[i].name);
    free(members);
}
/* Key sorting avoids a quadratic scan for every field in a large provider data
 * object. Sorting affects only these private tables, never the original tree. */
static int JsonCompareMemberOrder(const void *a, const void *b)
{
    return strcmp(((const JsonCompareMember *)a)->name, ((const JsonCompareMember *)b)->name);
}
static UmiStatus JsonCompareText(const UmiJsonTree *tree, int node, char **out)
{
    const JsonNode *value = &tree->nodes[node];
    size_t capacity = value->end - value->begin + 1U;
    char *text = malloc(capacity);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiJsonTreeText(tree, node, text, capacity);
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
static UmiStatus JsonCompareMembers(const UmiJsonTree *tree, int node, const UmiCancellationToken *cancel,
                                    JsonCompareMember **out)
{
    *out = NULL;
    size_t count = UmiJsonTreeCount(tree, node);
    if (count == 0U)
        return UMI_STATUS_OK;
    if (count > SIZE_MAX / sizeof(JsonCompareMember))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    JsonCompareMember *members = calloc(count, sizeof(*members));
    if (members == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    int key = UmiJsonTreeFirst(tree, node);
    for (size_t i = 0U; i < count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = JsonCompareText(tree, key, &members[i].name);
        if (status != UMI_STATUS_OK)
            break;
        members[i].value = UmiJsonTreeNext(tree, key);
        key = UmiJsonTreeNext(tree, members[i].value);
    }
    if (status == UMI_STATUS_OK)
    {
        qsort(members, count, sizeof(*members), JsonCompareMemberOrder);
        for (size_t i = 1U; i < count; ++i)
        {
            if (umi_cancellation_token_is_requested(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            if (strcmp(members[i - 1U].name, members[i].name) == 0)
            {
                status = UMI_STATUS_ALREADY_EXISTS;
                break;
            }
        }
    }
    if (status == UMI_STATUS_OK)
        *out = members;
    else
        JsonCompareMembersFree(members, count);
    return status;
}
/* Recursion follows the already validated container depth, capped by tree
 * creation at 256. Large strings and member tables remain on the heap. */
static UmiStatus JsonCompareValue(const UmiJsonTree *left, int a, const UmiJsonTree *right, int b,
                                  const UmiCancellationToken *cancel, int *equal)
{
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    const JsonNode *x = &left->nodes[a], *y = &right->nodes[b];
    *equal = 0;
    if (x->kind != y->kind)
        return UMI_STATUS_OK;
    if (x->kind == UMI_LANGUAGE_RUNTIME_JSON_STRING)
    {
        char *first = NULL, *second = NULL;
        UmiStatus status = JsonCompareText(left, a, &first);
        if (status == UMI_STATUS_OK)
            status = JsonCompareText(right, b, &second);
        if (status == UMI_STATUS_OK)
            *equal = strcmp(first, second) == 0;
        free(first);
        free(second);
        return status;
    }
    if (x->kind == UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
    {
        size_t length = x->end - x->begin;
        *equal =
            length == y->end - y->begin && memcmp(left->text + x->begin, right->text + y->begin, length) == 0;
        return UMI_STATUS_OK;
    }
    size_t count = UmiJsonTreeCount(left, a);
    if (count != UmiJsonTreeCount(right, b))
        return UMI_STATUS_OK;
    UmiStatus status = UMI_STATUS_OK;
    int matches = 1;
    if (x->kind == UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
    {
        int first = UmiJsonTreeFirst(left, a), second = UmiJsonTreeFirst(right, b);
        for (size_t i = 0U; status == UMI_STATUS_OK && matches && i < count; ++i)
        {
            status = JsonCompareValue(left, first, right, second, cancel, &matches);
            first = UmiJsonTreeNext(left, first);
            second = UmiJsonTreeNext(right, second);
        }
    }
    else
    {
        JsonCompareMember *first = NULL, *second = NULL;
        status = JsonCompareMembers(left, a, cancel, &first);
        if (status == UMI_STATUS_OK)
            status = JsonCompareMembers(right, b, cancel, &second);
        for (size_t i = 0U; status == UMI_STATUS_OK && matches && i < count; ++i)
        {
            matches = strcmp(first[i].name, second[i].name) == 0;
            if (matches)
                status = JsonCompareValue(left, first[i].value, right, second[i].value, cancel, &matches);
        }
        JsonCompareMembersFree(first, count);
        JsonCompareMembersFree(second, count);
    }
    if (status == UMI_STATUS_OK)
        *equal = matches;
    return status;
}
UmiStatus UmiJsonTreeValuesEqual(const UmiJsonTree *left, int left_node, const UmiJsonTree *right,
                                 int right_node, const UmiCancellationToken *cancel, int *out_equal)
{
    if (out_equal == NULL || !JsonValid(left, left_node) || !JsonValid(right, right_node))
        return UMI_STATUS_INVALID_ARGUMENT;
    int equal = 0;
    UmiStatus status = JsonCompareValue(left, left_node, right, right_node, cancel, &equal);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out_equal = equal;
    return status;
}
