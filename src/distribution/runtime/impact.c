/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/impact.c
 * Purpose: Propagate changed-source impact through dependencies and explicit test ownership.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "impact_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct Edge { size_t from, to; } Edge;
typedef struct Changed { char *path; bool matched; } Changed;
typedef struct Work {
    const UmiReleaseInventory *inventory;
    size_t count, targetStart, targetCount, row;
    uint32_t *reasons;
    Edge *edges;
    size_t edgeCount, edgeCapacity;
    uint32_t fallback;
    bool completeSeen, complete, alwaysSeen, always;
    size_t bindings;
    char *headerPath;
} Work;

static char *Copy(const char *text)
{
    size_t size = strlen(text) + 1U;
    char *copy = malloc(size);
    if (copy != NULL) memcpy(copy, text, size);
    return copy;
}
static const UmiReleaseInventoryRecord *Row(const Work *work, size_t i)
{
    return UmiReleaseInventoryAt(work->inventory, i);
}
static size_t Target(const Work *work, const char *name)
{
    size_t low = work->targetStart, high = low + work->targetCount;
    while (low < high) {
        size_t mid = low + (high - low) / 2U;
        int order = strcmp(Row(work, mid)->identity, name);
        if (order < 0) low = mid + 1U;
        else high = mid;
    }
    return low < work->targetStart + work->targetCount &&
        strcmp(Row(work, low)->identity, name) == 0 ? low : SIZE_MAX;
}
static UmiStatus AddEdge(Work *work, size_t from, size_t to)
{
    if (work->edgeCount == UMI_RELEASE_IMPACT_EDGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (work->edgeCount == work->edgeCapacity) {
        size_t capacity = work->edgeCapacity == 0U ? 64U : work->edgeCapacity * 2U;
        if (capacity > UMI_RELEASE_IMPACT_EDGE_LIMIT) capacity = UMI_RELEASE_IMPACT_EDGE_LIMIT;
        Edge *edges = realloc(work->edges, capacity * sizeof(*edges));
        if (edges == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        work->edges = edges; work->edgeCapacity = capacity;
    }
    work->edges[work->edgeCount++] = (Edge){from, to};
    return UMI_STATUS_OK;
}
static UmiStatus Field(void *context, const char *key, const char *value)
{
    Work *work = context;
    UmiReleaseInventoryKind kind = Row(work, work->row)->kind;
    if (strcmp(key, "complete") == 0) {
        if (work->completeSeen || kind == UMI_RELEASE_INVENTORY_HEADER) return UMI_STATUS_PARSE_ERROR;
        work->completeSeen = true; work->complete = value[0] == '1';
        return UMI_STATUS_OK;
    }
    if (strcmp(key, "always") == 0) {
        if (work->alwaysSeen || kind != UMI_RELEASE_INVENTORY_TEST) return UMI_STATUS_PARSE_ERROR;
        work->alwaysSeen = true; work->always = value[0] == '1';
        return UMI_STATUS_OK;
    }
    if (strcmp(key, "path") == 0) {
        if (kind != UMI_RELEASE_INVENTORY_HEADER || work->headerPath != NULL) return UMI_STATUS_PARSE_ERROR;
        work->headerPath = Copy(value);
        return work->headerPath == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    }
    bool dependency = strcmp(key, "dependency") == 0;
    if ((dependency && kind != UMI_RELEASE_INVENTORY_TARGET) ||
        (!dependency && (strcmp(key, "target") != 0 || kind != UMI_RELEASE_INVENTORY_TEST)))
        return UMI_STATUS_PARSE_ERROR;
    size_t owner = Target(work, value);
    ++work->bindings;
    if (owner == SIZE_MAX) {
        work->fallback |= dependency ? UMI_RELEASE_IMPACT_INCOMPLETE_GRAPH : UMI_RELEASE_IMPACT_UNOWNED_TEST;
        return UMI_STATUS_OK;
    }
    return AddEdge(work, owner, work->row);
}
static int ChangedOrder(const void *left, const void *right)
{
    return strcmp(((const Changed *)left)->path, ((const Changed *)right)->path);
}
static int EdgeOrder(const void *left, const void *right)
{
    const Edge *a = left, *b = right;
    if (a->from != b->from) return a->from < b->from ? -1 : 1;
    return a->to < b->to ? -1 : a->to != b->to;
}
static Changed *FindChange(Changed *changes, size_t count, const char *path)
{
    Changed key = {(char *)path, false};
    return count == 0U ? NULL : bsearch(&key, changes, count, sizeof(*changes), ChangedOrder);
}
static UmiStatus MatchPath(Work *work, const char *path, const char *owner,
    Changed *changes, size_t changeCount)
{
    char *relative = NULL;
    UmiStatus status = UmiImpactRelativePath(UmiReleaseInventorySourceRoot(work->inventory), path, &relative);
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_INVALID_ARGUMENT) {
        work->fallback |= UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;
    Changed *changed = FindChange(changes, changeCount, relative);
    free(relative);
    if (changed == NULL) return UMI_STATUS_OK;
    changed->matched = true;
    size_t target = Target(work, owner);
    if (target == SIZE_MAX) work->fallback |= UMI_RELEASE_IMPACT_UNMAPPED_PATH;
    else work->reasons[target] |= UMI_RELEASE_IMPACT_CHANGED_SOURCE;
    return UMI_STATUS_OK;
}
static UmiStatus ReadGraph(Work *work, Changed *changes, size_t changeCount)
{
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < work->count && status == UMI_STATUS_OK; ++i) {
        const UmiReleaseInventoryRecord *row = Row(work, i);
        work->row = i; work->completeSeen = false; work->complete = false;
        work->alwaysSeen = false; work->always = false; work->bindings = 0U;
        bool present = false;
        if (row->kind != UMI_RELEASE_INVENTORY_SOURCE)
            status = UmiImpactMetadataRead(row->detail, Field, work, &present);
        if (status == UMI_STATUS_OK) {
            switch (row->kind) {
            case UMI_RELEASE_INVENTORY_TARGET:
                if (!present || !work->completeSeen || !work->complete)
                    work->fallback |= UMI_RELEASE_IMPACT_INCOMPLETE_GRAPH;
                break;
            case UMI_RELEASE_INVENTORY_TEST:
                if (!present || !work->completeSeen || !work->complete ||
                    (work->bindings == 0U && !work->always))
                    work->fallback |= UMI_RELEASE_IMPACT_UNOWNED_TEST;
                if (work->always) work->reasons[i] |= UMI_RELEASE_IMPACT_ALWAYS;
                break;
            case UMI_RELEASE_INVENTORY_SOURCE:
                if (strcmp(row->state, "unresolved") == 0)
                    work->fallback |= UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE;
                else status = MatchPath(work, row->identity, row->owner, changes, changeCount);
                break;
            case UMI_RELEASE_INVENTORY_HEADER:
                /* Old inventories lack the absolute header identity needed when
                 * Framework is nested in Applications. Unknown paths widen. */
                if (work->headerPath == NULL) work->fallback |= UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE;
                else status = MatchPath(work, work->headerPath, row->owner, changes, changeCount);
                break;
            }
        }
        free(work->headerPath); work->headerPath = NULL;
    }
    return status;
}
static UmiStatus Propagate(Work *work)
{
    /* Each target enters the queue once. Reverse edges preserve dependency
     * direction: a library edit selects its users, not unrelated libraries. */
    size_t *queue = malloc((work->count == 0U ? 1U : work->count) * sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (work->edgeCount > 1U) qsort(work->edges, work->edgeCount, sizeof(*work->edges), EdgeOrder);
    size_t head = 0U, tail = 0U;
    for (size_t i = 0U; i < work->count; ++i)
        if (Row(work, i)->kind == UMI_RELEASE_INVENTORY_TARGET && work->reasons[i] != 0U) queue[tail++] = i;
    while (head < tail) {
        size_t from = queue[head++], low = 0U, high = work->edgeCount;
        while (low < high) {
            size_t mid = low + (high - low) / 2U;
            if (work->edges[mid].from < from) low = mid + 1U;
            else high = mid;
        }
        for (size_t i = low; i < work->edgeCount && work->edges[i].from == from; ++i) {
            size_t to = work->edges[i].to;
            bool target = Row(work, to)->kind == UMI_RELEASE_INVENTORY_TARGET;
            if (target && work->reasons[to] == 0U) queue[tail++] = to;
            work->reasons[to] |= target ? UMI_RELEASE_IMPACT_DEPENDENT : UMI_RELEASE_IMPACT_TEST_OWNER;
        }
    }
    /* Selected tests may need more than the changed owner (for example a
     * fixture executable). Build every declared prerequisite, but do not
     * pretend that merely rebuilding that helper is a new source change. */
    for (size_t i = 0U; i < work->edgeCount; ++i) {
        size_t test = work->edges[i].to;
        if (Row(work, test)->kind == UMI_RELEASE_INVENTORY_TEST && work->reasons[test] != 0U)
            work->reasons[work->edges[i].from] |= UMI_RELEASE_IMPACT_TEST_PREREQUISITE;
    }
    free(queue);
    return UMI_STATUS_OK;
}
void UmiReleaseImpactDestroy(UmiReleaseImpactPlan *plan)
{
    if (plan == NULL) return;
    for (size_t i = 0U; i < plan->count; ++i) free((void *)plan->items[i].name);
    free(plan->items); free(plan);
}
UmiStatus UmiReleaseImpactDescribe(const UmiReleaseImpactPlan *plan, UmiReleaseImpactSummary *outSummary)
{
    if (plan == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = plan->summary;
    return UMI_STATUS_OK;
}
size_t UmiReleaseImpactCount(const UmiReleaseImpactPlan *plan)
{
    return plan == NULL ? 0U : plan->count;
}
const UmiReleaseImpactItem *UmiReleaseImpactAt(const UmiReleaseImpactPlan *plan, size_t index)
{
    return plan != NULL && index < plan->count ? &plan->items[index] : NULL;
}
UmiStatus UmiReleaseImpactCreate(const UmiReleaseInventory *inventory,
    const char *const *changedPaths, size_t changedCount, bool forceFull, UmiReleaseImpactPlan **outPlan)
{
    if (inventory == NULL || outPlan == NULL || *outPlan != NULL ||
        (changedCount != 0U && changedPaths == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (changedCount > UMI_RELEASE_IMPACT_CHANGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(UmiReleaseInventoryProducer(inventory), "cmake") != 0) return UMI_STATUS_INVALID_STATE;
    Work work = {0}; work.inventory = inventory; work.count = UmiReleaseInventoryCount(inventory);
    Changed *changes = calloc(changedCount == 0U ? 1U : changedCount, sizeof(*changes));
    work.reasons = calloc(work.count == 0U ? 1U : work.count, sizeof(*work.reasons));
    UmiReleaseImpactPlan *plan = calloc(1U, sizeof(*plan));
    UmiStatus status = changes == NULL || work.reasons == NULL || plan == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    size_t accepted = 0U;
    for (size_t i = 0U; i < changedCount && status == UMI_STATUS_OK; ++i) {
        status = UmiImpactChangedPath(changedPaths[i], &changes[accepted].path);
        if (status == UMI_STATUS_OK) ++accepted;
    }
    if (status == UMI_STATUS_OK) {
        if (accepted > 1U) qsort(changes, accepted, sizeof(*changes), ChangedOrder);
        size_t distinct = 0U;
        for (size_t i = 0U; i < accepted; ++i) {
            if (distinct != 0U && strcmp(changes[distinct - 1U].path, changes[i].path) == 0) free(changes[i].path);
            else changes[distinct++] = changes[i];
        }
        accepted = distinct;
        for (size_t i = 0U; i < work.count; ++i) {
            if (Row(&work, i)->kind == UMI_RELEASE_INVENTORY_TARGET) {
                if (work.targetCount == 0U) work.targetStart = i;
                ++work.targetCount;
            }
        }
        if (accepted != 0U || forceFull) {
            status = ReadGraph(&work, changes, accepted);
            if (forceFull) work.fallback |= UMI_RELEASE_IMPACT_REQUESTED_FULL;
            for (size_t i = 0U; i < accepted; ++i) {
                if (!changes[i].matched) work.fallback |= UMI_RELEASE_IMPACT_UNMAPPED_PATH;
                if (UmiImpactBuildConfiguration(changes[i].path)) work.fallback |= UMI_RELEASE_IMPACT_BUILD_CONFIGURATION;
            }
            if (status == UMI_STATUS_OK && work.fallback != 0U) {
                for (size_t i = 0U; i < work.count; ++i)
                    if (Row(&work, i)->kind == UMI_RELEASE_INVENTORY_TARGET || Row(&work, i)->kind == UMI_RELEASE_INVENTORY_TEST)
                        work.reasons[i] |= UMI_RELEASE_IMPACT_FULL;
            } else if (status == UMI_STATUS_OK) status = Propagate(&work);
        }
    }
    if (status == UMI_STATUS_OK) {
        plan->items = calloc(work.count == 0U ? 1U : work.count, sizeof(*plan->items));
        if (plan->items == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < work.count && status == UMI_STATUS_OK; ++i) {
        if (work.reasons[i] == 0U) continue;
        const UmiReleaseInventoryRecord *row = Row(&work, i);
        char *name = Copy(row->identity);
        if (name == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
        plan->items[plan->count++] = (UmiReleaseImpactItem){row->kind, name, work.reasons[i]};
        if (row->kind == UMI_RELEASE_INVENTORY_TARGET) ++plan->summary.targets;
        else ++plan->summary.tests;
    }
    if (status == UMI_STATUS_OK) {
        plan->summary.changedPaths = accepted;
        plan->summary.fallbackReasons = work.fallback;
        plan->summary.noWork = accepted == 0U && !forceFull;
        plan->summary.missingTestCoverage = !plan->summary.noWork && plan->summary.tests == 0U;
        *outPlan = plan;
    } else UmiReleaseImpactDestroy(plan);
    for (size_t i = 0U; i < accepted; ++i) free(changes[i].path);
    free(changes); free(work.reasons); free(work.edges); free(work.headerPath);
    return status;
}
