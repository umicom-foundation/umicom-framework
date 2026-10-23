/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/explorer_projection.inc
 * PURPOSE: Stage complete indexed Test Explorer projections before atomic publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct ExplorerTraversalFrame {
    size_t index, parent, depth;
} ExplorerTraversalFrame;

/* A bounded state history preserves expansion and selection across temporary
 * discovery removal. If the history fills, active IDs take precedence and only
 * absent IDs are evicted, in stable ID order. No live row loses its state. */
static UmiStatus ExplorerRetainStates(UmiTestExplorerModel *next,
    const UmiTestExplorerModel *previous)
{
    size_t capacity = next->row_count + previous->state_count;
    if (capacity > UMI_TEST_EXPLORER_MODEL_MAX_CAPACITY)
        capacity = UMI_TEST_EXPLORER_MODEL_MAX_CAPACITY;
    if (capacity == 0U) return UMI_STATUS_OK;
    next->states = (ExplorerStateRecord *)calloc(capacity, sizeof *next->states);
    if (next->states == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < next->row_count; ++i) {
        const char *id = ExplorerText(next, i, EXPLORER_TEXT_id);
        const size_t old = ExplorerStateFind(previous, id);
        ExplorerStateRecord *state = &next->states[next->state_count++];
        if (old != EXPLORER_NONE) *state = previous->states[old];
        else {
            memcpy(state->item_id, id, strlen(id) + 1U);
            state->expanded = next->records[i].child_count > 0U;
        }
        next->records[i].expanded = state->expanded;
        next->records[i].selected = state->selected;
    }
    for (size_t i = 0U; i < previous->state_count && next->state_count < capacity; ++i)
        if (ExplorerFind(next, previous->states[i].item_id) == EXPLORER_NONE)
            next->states[next->state_count++] = previous->states[i];
    qsort(next->states, next->state_count, sizeof *next->states, ExplorerStateCompare);
    return UMI_STATUS_OK;
}

/* Each source record has at most one declared parent. Marking a record before
 * stacking it cuts cyclic edges without recursion or duplicate rows. Rootless
 * components retain the former source-order fallback; siblings remain sorted. */
static void ExplorerEmit(UmiTestExplorerModel *next, size_t root,
    const size_t *last_child, const size_t *previous_sibling,
    unsigned char *queued, ExplorerTraversalFrame *stack, size_t *written)
{
    if (queued[root]) return;
    size_t pending = 1U;
    stack[0] = (ExplorerTraversalFrame){root, EXPLORER_NONE, 0U};
    queued[root] = 1U;
    while (pending > 0U) {
        const ExplorerTraversalFrame frame = stack[--pending];
        ExplorerRecord *record = &next->records[frame.index];
        record->tree_parent = frame.parent;
        record->depth = frame.depth;
        next->order[(*written)++] = frame.index;
        for (size_t child = last_child[frame.index]; child != EXPLORER_NONE;
             child = previous_sibling[child]) {
            if (queued[child]) continue;
            queued[child] = 1U;
            stack[pending++] = (ExplorerTraversalFrame){child, frame.index, frame.depth + 1U};
        }
    }
}

static UmiStatus ExplorerProject(UmiTestExplorerModel *next,
    const UmiTestExplorerModel *previous,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results)
{
    const size_t count = umi_test_platform_item_registry_count(items);
    if (count > UMI_TEST_EXPLORER_MODEL_MAX_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    next->filter = previous->filter; next->sort_mode = previous->sort_mode;
    next->group_mode = previous->group_mode;
    next->item_revision = umi_test_platform_item_registry_revision(items);
    next->result_revision = umi_test_platform_result_registry_revision(results);
    next->revision = previous->revision + 1U;
    next->row_count = count;
    next->result_count = umi_test_platform_result_registry_count(results);
    if (next->result_count > UMI_TEST_PLATFORM_RESULT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    next->text = (char *)calloc(1U, 1U);
    next->text_size = next->text_capacity = 1U;
    if (next->text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    const size_t slots = count == 0U ? 1U : count;
    next->records = (ExplorerRecord *)calloc(slots, sizeof *next->records);
    next->order = (size_t *)calloc(slots, sizeof *next->order);
    next->by_id = (size_t *)calloc(slots, sizeof *next->by_id);
    next->visible = (size_t *)calloc(slots, sizeof *next->visible);
    next->results = (UmiTestPlatformResultSnapshot *)calloc(
        next->result_count == 0U ? 1U : next->result_count, sizeof *next->results);
    size_t *sorted = (size_t *)calloc(slots, sizeof *sorted);
    size_t *scratch = (size_t *)calloc(slots, sizeof *scratch);
    size_t *last_child = (size_t *)calloc(slots, sizeof *last_child);
    size_t *previous_sibling = (size_t *)calloc(slots, sizeof *previous_sibling);
    unsigned char *queued = (unsigned char *)calloc(slots, sizeof *queued);
    ExplorerTraversalFrame *stack = (ExplorerTraversalFrame *)calloc(slots, sizeof *stack);
    UmiStatus status = UMI_STATUS_OK;
    if (next->records == NULL || next->order == NULL || next->by_id == NULL ||
        next->visible == NULL || next->results == NULL || sorted == NULL ||
        scratch == NULL || last_child == NULL || previous_sibling == NULL ||
        queued == NULL || stack == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    for (size_t i = 0U; i < count; ++i) {
        UmiTestPlatformItemSnapshot item;
        status = umi_test_platform_item_registry_at(items, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        status = ExplorerPack(next, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        next->by_id[i] = sorted[i] = i;
        last_child[i] = previous_sibling[i] = EXPLORER_NONE;
    }
    ExplorerSort(next, next->by_id, scratch, count, 1);
    for (size_t i = 0U; i < next->result_count; ++i) {
        UmiTestPlatformResultSnapshot *candidate = &next->results[i];
        status = umi_test_platform_result_registry_at(results, i, candidate);
        if (status != UMI_STATUS_OK) goto done;
        const size_t index = ExplorerFind(next, candidate->item_id);
        if (index == EXPLORER_NONE) continue;
        const UmiTestPlatformResultSnapshot *old = ExplorerResult(next, index);
        if (old == NULL || candidate->sequence > old->sequence ||
            (candidate->sequence == old->sequence && candidate->revision > old->revision))
            next->records[index].latest = i;
    }
    for (size_t i = 0U; i < count; ++i) {
        UmiTestPlatformItemSnapshot item;
        ExplorerUnpack(next, i, &item);
        ExplorerRecord *record = &next->records[i];
        record->direct_match = umi_test_platform_filter_matches(&item, ExplorerResult(next, i), &next->filter);
        record->parent = item.parent_id[0] == '\0' ? EXPLORER_NONE : ExplorerFind(next, item.parent_id);
        if (record->parent != EXPLORER_NONE) ++next->records[record->parent].child_count;
    }
    /* Every ancestor is marked once, including malformed cycles. This computes
     * the complete descendant closure independently of registry insertion order. */
    for (size_t i = 0U; i < count; ++i) {
        if (!next->records[i].direct_match) continue;
        for (size_t node = i; node != EXPLORER_NONE && !next->records[node].subtree_match;
             node = next->records[node].parent)
            next->records[node].subtree_match = 1;
    }
    status = ExplorerRetainStates(next, previous);
    if (status != UMI_STATUS_OK) goto done;
    ExplorerSort(next, sorted, scratch, count, 0);
    for (size_t i = 0U; i < count; ++i) {
        const size_t child = sorted[i];
        const size_t parent = next->records[child].parent;
        if (parent == EXPLORER_NONE) continue;
        previous_sibling[child] = last_child[parent];
        last_child[parent] = child;
    }
    size_t written = 0U;
    for (size_t i = 0U; i < count; ++i)
        if (next->records[sorted[i]].parent == EXPLORER_NONE)
            ExplorerEmit(next, sorted[i], last_child, previous_sibling, queued, stack, &written);
    for (size_t i = 0U; i < count; ++i)
        ExplorerEmit(next, i, last_child, previous_sibling, queued, stack, &written);
    if (written != count) { status = UMI_STATUS_INVALID_STATE; goto done; }
    ExplorerVisibility(next);
    if (next->item_revision != umi_test_platform_item_registry_revision(items) ||
        next->result_revision != umi_test_platform_result_registry_revision(results))
        status = UMI_STATUS_INVALID_STATE;
done:
    free(sorted); free(scratch); free(last_child); free(previous_sibling); free(queued); free(stack);
    return status;
}
