/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/surface_support.c
 *
 * PURPOSE:
 *   support scoring and blocker detection for a renderer against one semantic surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/surface_support.h"

/*
 * Initialise fc surface support from caller-provided values so later operations receive a
 * known state.
 */
void umi_fc_surface_support_init(UmiFcSurfaceSupport *self) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (self != NULL) *self = (UmiFcSurfaceSupport){0}; }
/*
 * Provide the fc surface support satisfied operation used by this module and its client
 * applications.
 */
bool umi_fc_surface_support_satisfied(const UmiFcSurfaceSupport *self) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (self == NULL) return false; return self->required == (self->supported & self->required); }
/*
 * Provide the fc surface support score operation used by this module and its client
 * applications.
 */
double umi_fc_surface_support_score(const UmiFcSurfaceSupport *self) { unsigned req, hit; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (self == NULL) return 0.0; req=(unsigned)__builtin_popcountll(self->required); hit=(unsigned)__builtin_popcountll(self->supported & self->required); return umi_fc_ratio((size_t)hit,(size_t)req); }
