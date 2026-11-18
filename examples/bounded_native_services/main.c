/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/bounded_native_services/main.c
 *
 * PURPOSE:
 *   Compose four allocation-free native services through installed SDK contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/game/grid_path.h"
#include "umicom/render3d/frustum.h"
#include "umicom/ai_workflow/euler_step.h"
#include "umicom/web/entity_tag.h"
#include <stdio.h>

int main(void)
{
    const uint8_t blocked[12] = {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
    size_t queue[12], parents[12], path[12], count = 0;
    UmiStatus status = UmiGameGridPath(blocked, 12, 4, 3, 0, 8, 12,
        queue, parents, 12, path, 12, &count);
    if (status != UMI_STATUS_OK || count != 9) return 1;

    const UmiRender3dBounds bounds = {{-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5}};
    const UmiRender3dPlane planes[6] = {
        {{1, 0, 0}, 1}, {{-1, 0, 0}, 1},
        {{0, 1, 0}, 1}, {{0, -1, 0}, 1},
        {{0, 0, 1}, 1}, {{0, 0, -1}, 1}
    };
    bool visible = false;
    status = UmiRender3dBoundsVisible(&bounds, planes, &visible);
    if (status != UMI_STATUS_OK || !visible) return 2;

    const float sample[2] = {4, -4}, denoised[2] = {2, 0};
    float next[2] = {0};
    status = UmiAiEulerDenoisedStep(sample, denoised, 2, 2, 1, next, 2);
    if (status != UMI_STATUS_OK || next[0] != 3 || next[1] != -2) return 3;

    bool unchanged = false;
    static const char condition[] = "W/\"asset-1\"";
    static const char current[] = "\"asset-1\"";
    status = UmiWebIfNoneMatch(condition, sizeof condition - 1,
        current, sizeof current - 1, &unchanged);
    if (status != UMI_STATUS_OK || !unchanged) return 4;

    /* Each result is checked before presentation. The application does not
     * duplicate the engine algorithms or allocate hidden working buffers. */
    printf("Path: %zu cells\nVisible: %d\nLatent: %.1f, %.1f\nUnchanged: %d\n",
        count, (int)visible, (double)next[0], (double)next[1], (int)unchanged);
    return 0;
}
