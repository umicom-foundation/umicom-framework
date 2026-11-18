/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/game/grid_path.c
 *
 * PURPOSE:
 *   Find shortest cardinal paths with caller-owned scratch memory and explicit work limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/game/grid_path.h"
#include <stdint.h>
UmiStatus UmiGameGridPath(const uint8_t *blocked, size_t blocked_count,
    size_t width, size_t height, size_t start, size_t goal,
    size_t max_expansions, size_t *queue, size_t *parents, size_t scratch_capacity,
    size_t *out_path, size_t path_capacity, size_t *out_count)
{
    if (!blocked || !queue || !parents || !out_path || !out_count ||
        !width || !height || !max_expansions || width > UMI_GAME_GRID_MAX_CELLS / height)
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t cells = width * height;
    if (blocked_count < cells || scratch_capacity < cells)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (start >= cells || goal >= cells) return UMI_STATUS_INVALID_ARGUMENT;
    if (blocked[start] || blocked[goal]) return UMI_STATUS_NOT_FOUND;
    for (size_t i = 0; i < cells; ++i) parents[i] = SIZE_MAX;
    size_t head = 0, tail = 1, expanded = 0;
    queue[0] = start;
    parents[start] = start;
    while (head < tail) {
        if (expanded == max_expansions) return UMI_STATUS_BUSY;
        const size_t current = queue[head++];
        ++expanded;
        if (current == goal) {
            size_t count = 1;
            for (size_t node = goal; node != start; node = parents[node]) ++count;
            if (count > path_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            /* Every predecessor points to an earlier discovered cell, so the
             * admitted chain cannot cycle. Publish only after sizing the path. */
            size_t node = goal;
            for (size_t i = count; i > 0; --i) {
                out_path[i - 1] = node;
                node = parents[node];
            }
            *out_count = count;
            return UMI_STATUS_OK;
        }
        size_t next[4], n = 0;
        if (current >= width) next[n++] = current - width;
        if (current % width + 1 < width) next[n++] = current + 1;
        if (current < cells - width) next[n++] = current + width;
        if (current % width != 0) next[n++] = current - 1;
        for (size_t i = 0; i < n; ++i) {
            size_t node = next[i];
            if (!blocked[node] && parents[node] == SIZE_MAX) {
                parents[node] = current;
                queue[tail++] = node;
            }
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
