/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_playback_backend.c
 * PURPOSE: Check optional playback capabilities before constructing a native media player.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_audition.h"
#include <stdio.h>
#include <string.h>
#ifdef UMI_CREATIVE_HAS_GSTREAMER
#include <gst/gst.h>
#endif

/* Missing playback plugins must be an ordinary capability result. Some GTK
 * GStreamer backends terminate the process while constructing playbin3, before
 * GtkMediaStream can publish an error. Keep that construction behind this gate. */
UmiStatus UmiCreativeAuditionGtkBackendStatus(char *message, size_t capacity)
{
    if (message == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *requested = g_getenv("GTK_MEDIA");
    if (requested != NULL && requested[0] != '\0' && strcmp(requested, "gstreamer") != 0)
    {
        (void)snprintf(message, capacity,
                       "Audio preview requires the GStreamer GTK backend; GTK_MEDIA selects '%s'. "
                       "Editing and WAVE export remain available.",
                       requested);
        return UMI_STATUS_UNAVAILABLE;
    }
#ifdef UMI_CREATIVE_HAS_GSTREAMER
    GError *error = NULL;
    if (!gst_init_check(NULL, NULL, &error))
    {
        (void)snprintf(message, capacity, "GStreamer could not initialize: %s",
                       error != NULL ? error->message : "no diagnostic was supplied");
        g_clear_error(&error);
        return UMI_STATUS_UNAVAILABLE;
    }
    /* Query factories without constructing a pipeline, opening a device, or
     * playing sound. A successful preflight is not audio-device qualification. */
    static const char *required[] = {"playbin3",     "giostreamsrc",  "appsink",      "wavparse",
                                     "audioconvert", "audioresample", "autoaudiosink"};
    for (size_t index = 0U; index < sizeof(required) / sizeof(required[0]); ++index)
    {
        GstElementFactory *factory = gst_element_factory_find(required[index]);
        if (factory == NULL)
        {
            (void)snprintf(message, capacity,
                           "Audio preview is unavailable: GStreamer component '%s' is missing. "
                           "Install the matching playback plugins, then restart. Editing and WAVE "
                           "export remain available.",
                           required[index]);
            return UMI_STATUS_UNAVAILABLE;
        }
        gst_object_unref(factory);
    }
    (void)snprintf(message, capacity,
                   "Playback components are installed; an audio device is not yet verified.");
    return UMI_STATUS_OK;
#else
    (void)snprintf(message, capacity,
                   "Audio preview was built without GStreamer capability checks. "
                   "Enable the development package and rebuild to use preview. Editing and WAVE "
                   "export remain available.");
    return UMI_STATUS_UNAVAILABLE;
#endif
}
