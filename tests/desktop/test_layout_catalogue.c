/* Umicom Framework Tests | Desktop layout catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/desktop/layout_catalogue.h"

int main(void)
{
    UmiDesktopLayoutCatalogue catalogue;
    const UmiDesktopLayout *layout;
    umi_desktop_layout_catalogue_init(&catalogue);
    assert(umi_desktop_layout_catalogue_seed_professional(
        &catalogue, "primary") == UMI_STATUS_OK);
    assert(catalogue.count == 16U);
    assert(strcmp(catalogue.active_layout_id, "develop") == 0);
    layout = umi_desktop_layout_catalogue_find(&catalogue, "trading");
    assert(layout != NULL && layout->window_count == 5U && layout->locked);
    assert(strcmp(layout->windows[1].component_id, "umicom.trading.chart") == 0);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "treasury") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "media") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "music") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "creator") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "knowledge") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "model-lab") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "games") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "cad") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "kitchen") != NULL);
    assert(umi_desktop_layout_catalogue_find(&catalogue, "author") != NULL);
    assert(umi_desktop_layout_catalogue_clone(
        &catalogue, "trading", "trading-custom", "My Trading") ==
        UMI_STATUS_OK);
    assert(catalogue.count == 17U);
    assert(!umi_desktop_layout_catalogue_find(
        &catalogue, "trading-custom")->locked);
    return 0;
}
