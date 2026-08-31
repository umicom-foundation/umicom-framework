/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_media_engine.c
 *
 * PURPOSE:
 *   Verify checked image access and deterministic animation interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/media/media.h"

/* Exercise image and animation ownership without a GUI or media codec. */
int main(void)
{
    UmiMediaImageSurface *surface = NULL;
    UmiMediaAnimationTrack *track = NULL;
    UmiMediaRgbaPixel blue = {0U, 0U, 255U, 255U};
    UmiMediaRgbaPixel red = {255U, 0U, 0U, 255U};
    UmiMediaRgbaPixel actual;
    UmiMediaAnimationKeyframe start = {
        0.0, 0.0, UMI_MEDIA_ANIMATION_LINEAR};
    UmiMediaAnimationKeyframe end = {
        2.0, 10.0, UMI_MEDIA_ANIMATION_LINEAR};
    double sampled = 0.0;

    /* Pixel reads must return exact copied RGBA channel values. */
    assert(umi_media_image_surface_create(4U, 3U, &surface) == UMI_STATUS_OK);
    assert(umi_media_image_surface_clear(surface, blue) == UMI_STATUS_OK);
    assert(umi_media_image_surface_set_pixel(surface, 1U, 1U, red) ==
           UMI_STATUS_OK);
    assert(umi_media_image_surface_get_pixel(surface, 1U, 1U, &actual) ==
           UMI_STATUS_OK);
    assert(actual.red == 255U && actual.blue == 0U && actual.alpha == 255U);

    /* Linear sampling halfway between zero and ten must produce five. */
    assert(umi_media_animation_track_create(4U, &track) == UMI_STATUS_OK);
    assert(umi_media_animation_track_add(track, &end) == UMI_STATUS_OK);
    assert(umi_media_animation_track_add(track, &start) == UMI_STATUS_OK);
    assert(umi_media_animation_track_sample(track, 1.0, &sampled) ==
           UMI_STATUS_OK);
    assert(sampled == 5.0);

    /* Destruction follows the separate ownership of track and image surface. */
    umi_media_animation_track_destroy(track);
    umi_media_image_surface_destroy(surface);
    return 0;
}
