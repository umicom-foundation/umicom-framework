/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_presentation.h
 *
 * PURPOSE:
 *   Materialise a short-lived toolkit-neutral view-model presentation from the
 *   canonical Framework view-factory registry. Frontend adapters can render the
 *   same immutable snapshot without owning product services or product widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A product registers a view factory. The factory creates a UmiUiViewModel.
 * This helper copies the view and its properties into a bounded snapshot and
 * immediately destroys the temporary model. GTK4, headless and future Qt/Wt
 * adapters can therefore consume exactly the same product-neutral data.
 */

#ifndef UMICOM_UI_VIEW_PRESENTATION_H
#define UMICOM_UI_VIEW_PRESENTATION_H

#include <stddef.h>

#include "umicom/ui/view_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * A presentation must be able to materialise every property accepted by the
 * canonical Framework property bag.  Keeping a smaller second limit caused
 * data-dependent failures when rich views (notably Source Control Changes)
 * legitimately exposed more than 64 properties.
 */
#define UMI_UI_VIEW_PRESENTATION_PROPERTY_MAX UMI_UI_PROPERTY_MAX

typedef struct UmiUiViewPresentation {
    UmiUiViewSnapshot view;
    UmiUiPropertySnapshot properties[UMI_UI_VIEW_PRESENTATION_PROPERTY_MAX];
    size_t property_count;
} UmiUiViewPresentation;

UmiStatus umi_ui_view_presentation_build(
    const UmiUiViewFactoryRegistry *registry,
    const char *view_type,
    const char *view_id,
    UmiUiViewPresentation *out_presentation
);

UmiStatus umi_ui_view_presentation_find_property(
    const UmiUiViewPresentation *presentation,
    const char *key,
    UmiUiPropertySnapshot *out_property
);

UmiStatus umi_ui_view_presentation_value_text(
    const UmiUiValue *value,
    char *out_text,
    size_t capacity
);

#ifdef __cplusplus
}
#endif

#endif
