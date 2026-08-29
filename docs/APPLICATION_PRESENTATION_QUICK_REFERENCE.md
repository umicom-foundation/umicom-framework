<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/APPLICATION_PRESENTATION_QUICK_REFERENCE.md

PURPOSE:
Provide a short beginner reference for inspecting and extending portable
application panels and windows.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
----------------------------------------------------------------------------- -->

# Application Presentation Quick Reference

## Important terms

| Term | Plain meaning |
| --- | --- |
| Component | A reusable application feature such as an editor or chart |
| Panel specification | Rules for presenting one component |
| Workspace recipe | An ordered list describing where components belong |
| Window specification | Rules for the outer application window |
| Presentation plan | The joined result a frontend can render |

## Public include

~~~c
#include "umicom/application/presentation/presentation.h"
~~~

## Project a layout

~~~c
UmiApplicationPresentationPlan plan;
UmiStatus status = umi_application_presentation_project(
    "org.umicom.workspace.trader.standard", &plan);
~~~

## Useful catalogue functions

~~~c
umi_application_presentation_panel_catalogue_count();
umi_application_presentation_panel_catalogue_find(component_id);
umi_application_presentation_window_catalogue_count();
umi_application_presentation_window_catalogue_find(recipe_id);
umi_application_presentation_catalogues_validate();
~~~

## Important rules

- Use stable component, recipe and window IDs.
- Keep presentation records in Framework.
- Keep toolkit widgets in frontend adapters.
- Add one panel record for every component.
- Add one window record for every recipe.
- Give every application Learning, Standard and Focus windows.
- Validate complete coverage before release.

## Inspection commands

~~~powershell
umicom-application-presentation validate
umicom-application-presentation list "org.umicom.studio"
umicom-application-presentation show "org.umicom.workspace.studio.standard"
~~~
