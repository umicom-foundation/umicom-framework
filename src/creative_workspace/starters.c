/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/starters.c
 * PURPOSE: Offer inspectable starting documents without replacing existing creative work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus Add(UmiCreativeProject * p, const char * id, const char * label, UmiCreativeElementKind kind,
int32_t x, int32_t y, int32_t width, int32_t height, uint32_t colour)
{
    UmiCreativeElement element;
    UmiStatus status = UmiCreativeElementInit( & element, id, label, kind, x, y, width, height, colour);
    return status == UMI_STATUS_OK ? UmiCreativeProjectPutElement(p, & element, false) : status;
}

UmiStatus UmiCreativeProjectStarter(UmiCreativeProject * p, const char * profile)
{
    if (profile == NULL || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    if (p -> elementCount!= 0U || p -> noteCount!= 0U)return UMI_STATUS_INVALID_STATE;
    bool kitchen = strcmp(profile, "kitchen") == 0, cad = strcmp(profile, "cad") == 0, media = strcmp(profile, "media") == 0,
    music = strcmp(profile, "music") == 0, games = strcmp(profile, "games") == 0, web = strcmp(profile, "web-studio") == 0,
    mobile = strcmp(profile, "mobile-studio") == 0;
    if (!kitchen && !cad && !media && !music && !games && !web && !mobile)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeProject * next = malloc(sizeof( * next));
    if (next == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    * next = * p;
    next -> settings.unit = kitchen || cad ? UMI_CREATIVE_MILLIMETRES : UMI_CREATIVE_PIXELS;
    next -> settings.width = kitchen || cad ? 3000 : mobile ? 360 : 960;
    next -> settings.height = kitchen || cad ? 2400 : mobile ? 640 : 600;
    next -> settings.durationMs = 8000U;
    next -> settings.tempoBpm = 120U;
    UmiStatus status = UMI_STATUS_OK;
    if (kitchen || cad){
        status = Add(next, "cabinet", "Base cabinet", UMI_CREATIVE_CABINET, 100, 100, 600, 600, 0x96734BU);
        if (status == UMI_STATUS_OK)status = Add(next, "sink", "Sink unit", UMI_CREATIVE_CABINET, 800, 100, 800, 600, 0x7395A3U);
        if (status == UMI_STATUS_OK)status = Add(next, "table", "Workshop table", UMI_CREATIVE_RECTANGLE, 1000, 1300, 1200, 600, 0x87A88BU);
    }
    else if (games){
        status = Add(next, "player", "Player", UMI_CREATIVE_ACTOR, 40, 220, 40, 40, 0x438AC4U);
        if (status == UMI_STATUS_OK)status = Add(next, "wall", "Wall", UMI_CREATIVE_OBSTACLE, 260, 120, 60, 300, 0x94776BU);
    }
    else if (media){
        status = Add(next, "heading", "Workshop open day", UMI_CREATIVE_TEXT, 40, 40, 880, 70, 0x203A4DU);
        if (status == UMI_STATUS_OK)status = Add(next, "card", "Come and build", UMI_CREATIVE_PANEL, 40, 180, 240, 140, 0x609EB0U);
        const UmiCreativeKey keys[] = {
            {0U, 0, 0, UMI_MEDIA_ANIMATION_LINEAR}
            , {4000U, 500, 100, UMI_MEDIA_ANIMATION_STEP}
            , {8000U, 180, 0, UMI_MEDIA_ANIMATION_LINEAR}
        };
        for (size_t i = 0U;status == UMI_STATUS_OK && i < 3U; ++ i)status = UmiCreativeProjectPutKey(next, "card", & keys[i]);
    }
    else if (music){
        status = Add(next, "heading", "Four-note practice", UMI_CREATIVE_TEXT, 40, 40, 880, 70, 0x203A4DU);
        const char * ids[] = {"note-c", "note-e", "note-g", "note-high-c"};
        const uint8_t pitches[] = {60U, 64U, 67U, 72U};
        for (size_t i = 0U;status == UMI_STATUS_OK && i < 4U; ++ i){
            UmiCreativeNote note = {0};
            (void)UmiCreativeCopyText(note.id, sizeof(note.id), ids[i]);
            note.startTick = (uint32_t)i * 480U;
            note.durationTicks = 480U;
            note.pitch = pitches[i];
            note.velocity = 80U;
            status = UmiCreativeProjectPutNote(next, & note, false);
        }
    }
    else{
        int32_t x = mobile ? 20 : 40, width = mobile ? 320 : 880;
        status = Add(next, "header", "Workshop booking", UMI_CREATIVE_PANEL, x, 20, width, 80, 0x386B82U);
        if (status == UMI_STATUS_OK)status = Add(next, "information", "Choose a session", UMI_CREATIVE_PANEL, x, 120, width, mobile ? 360 : 300, 0x87ADB6U);
        if (status == UMI_STATUS_OK)status = Add(next, "footer", "A visual prototype - no booking is sent", UMI_CREATIVE_TEXT, x, mobile ? 500 : 450, width, 70, 0x203A4DU);
    }
    if (status == UMI_STATUS_OK)status = UmiCreativeProjectValidate(next);
    if (status == UMI_STATUS_OK) * p = * next;
    free(next);
    return status;
}

