/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/motion.c
 * PURPOSE: Keep collision-playground movement separate from saved design edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
static bool Solid(const UmiCreativeElement * e)
{
    return e -> visible && (e -> kind == UMI_CREATIVE_OBSTACLE || e -> kind == UMI_CREATIVE_CABINET || e -> kind == UMI_CREATIVE_ACTOR);
}

static bool Intersects(int64_t ax, int64_t ay, int64_t aw, int64_t ah, int64_t bx, int64_t by, int64_t bw, int64_t bh)
{
    return ax < bx + bw && bx < ax + aw && ay < by + bh && by < ay + ah;
}

static UmiStatus ValidateState(const UmiCreativeProject * p, const UmiCreativePlayState * s, size_t * actor)
{
    if (s == NULL || !UmiCreativeIdValid(s -> actorId) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    if (p -> revision!= s -> sourceRevision)return UMI_STATUS_BUSY;
    * actor = UmiCreativeFindElementIndex(p, s -> actorId);
    if ( * actor == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    const UmiCreativeElement * a = & p -> elements[ * actor];
    if (a -> kind!= UMI_CREATIVE_ACTOR || !a -> visible)return UMI_STATUS_INVALID_STATE;
    if (s -> x < 0 || s -> y < 0 || (int64_t)s -> x + a -> width > p -> settings.width || (int64_t)s -> y + a -> height > p -> settings.height)return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U;i < p -> elementCount; ++ i){
        const UmiCreativeElement * b = & p -> elements[i];
        if (b -> keyCount!= 0U)return UMI_STATUS_NOT_IMPLEMENTED;
        if (i!= * actor && Solid(b) && Intersects(s -> x, s -> y, a -> width, a -> height, b -> x, b -> y, b -> width, b -> height))return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativePlayBegin(const UmiCreativeProject * p, const char * id, UmiCreativePlayState * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativePlayState state = {0};
    if (!UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    (void)UmiCreativeCopyText(state.actorId, sizeof(state.actorId), id);
    state.x = p -> elements[index].x;
    state.y = p -> elements[index].y;
    state.sourceRevision = p -> revision;
    UmiStatus status = ValidateState(p, & state, & index);
    if (status == UMI_STATUS_OK) * out = state;
    return status;
}

UmiStatus UmiCreativePlayMove(const UmiCreativeProject * p, UmiCreativePlayState * state, int32_t dx, int32_t dy, UmiCreativeMotion * out)
{
    if (out == NULL || (dx!= 0 && dy!= 0))return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    UmiStatus status = ValidateState(p, state, & index);
    if (status!= UMI_STATUS_OK)return status;
    UmiCreativeMotion motion = {0};
    if (dx == 0 && dy == 0){
        * out = motion;
        return UMI_STATUS_OK;
    }
    const UmiCreativeElement * a = & p -> elements[index];
    bool horizontal = dx!= 0;
    int64_t requested = horizontal ? dx : dy;
    bool positive = requested > 0;
    int64_t origin = horizontal ? state -> x : state -> y;
    int64_t size = horizontal ? a -> width : a -> height;
    int64_t extent = horizontal ? p -> settings.width : p -> settings.height;
    int64_t distance = positive ? requested : - requested;
    int64_t allowed = positive ? extent - origin - size : origin;
    const char * blocker = "canvas";
    for (size_t i = 0U;i < p -> elementCount; ++ i){
        const UmiCreativeElement * b = & p -> elements[i];
        if (i == index || !Solid(b))continue;
        bool crosses = horizontal ? ((int64_t)state -> y < b -> y + (int64_t)b -> height && b -> y < (int64_t)state -> y + a -> height) :
        ((int64_t)state -> x < b -> x + (int64_t)b -> width && b -> x < (int64_t)state -> x + a -> width);
        if (!crosses)continue;
        int64_t start = horizontal ? b -> x : b -> y, end = start + (horizontal ? b -> width : b -> height), gap;
        if (positive){
            if (start < origin + size)continue;
            gap = start - origin - size;
        }
        else{
            if (end > origin)continue;
            gap = origin - end;
        }
        if (gap < allowed){
            allowed = gap;
            blocker = b -> id;
        }
    }
    if (distance > allowed){
        distance = allowed;
        motion.blocked = true;
        (void)UmiCreativeCopyText(motion.blockerId, sizeof(motion.blockerId), blocker);
    }
    int32_t moved = (int32_t)(positive ? distance : - distance);
    if (horizontal){
        state -> x = (int32_t)((int64_t)state -> x + moved);
        motion.movedX = moved;
    }
    else{
        state -> y = (int32_t)((int64_t)state -> y + moved);
        motion.movedY = moved;
    }
    * out = motion;
    return UMI_STATUS_OK;
}

