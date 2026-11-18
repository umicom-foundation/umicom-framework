/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/game/grid_path.h
 *
 * PURPOSE:
 *   Expose bounded, deterministic grid navigation for strategy-game movement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_GAME_GRID_PATH_H
#define UMICOM_GAME_GRID_PATH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_GAME_GRID_MAX_CELLS 1048576U
/* Row-major uniform-cost grid; nonzero cells are blocked. Caller storage stays
 * stable for the call. Paths contain start and goal, using up/right/down/left
 * tie order. Only cardinal moves are allowed. No allocation or global state.
 *
 * queue and parents each hold width*height size_t elements; scratch_capacity
 * is their element capacity. They may change on any result. All buffers must
 * be pairwise disjoint. The caller guarantees accessible backing storage.
 * max_expansions bounds visited cells (not the initial O(cell_count) reset).
 * BUSY means the search exhausted this budget, NOT that no path exists.
 * Refusal, NOT_FOUND, BUSY and insufficient path capacity preserve out_path
 * and out_count. Success writes exactly out_count indices; tail stays intact.
 * This is shortest-path search for a static unweighted grid, not multi-agent
 * collision avoidance or a complete strategy-game movement controller. */
UmiStatus UmiGameGridPath(const uint8_t *blocked, size_t blocked_count,
    size_t width, size_t height, size_t start, size_t goal,
    size_t max_expansions, size_t *queue, size_t *parents, size_t scratch_capacity,
    size_t *out_path, size_t path_capacity, size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
