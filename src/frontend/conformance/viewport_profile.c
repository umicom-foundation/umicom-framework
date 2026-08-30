/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/viewport_profile.c
 *
 * PURPOSE:
 *   viewport aspect, pixel density and input-mode classification for frontend adaptation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/viewport_profile.h"

double umi_fc_viewport_profile_aspect(const UmiFcViewportProfile *profile){if(profile==NULL||profile->height<=0)return 0.0;return (double)profile->width/(double)profile->height;}
bool umi_fc_viewport_profile_is_portrait(const UmiFcViewportProfile *profile){return profile!=NULL&&profile->height>profile->width;}
