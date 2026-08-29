<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/COMPONENT_MODEL_AND_COMPOSITION_MB61.md

PURPOSE:
Document the MB61 versioned component registry, frontend factories, lifecycle,
declarative selection and portable layout-composition contracts.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
----------------------------------------------------------------------------- -->

# Component Model and Composition — MB61

MB61 turns the existing 132-entry application-component catalogue into an
instantiable Framework platform. Applications continue to reference stable
component IDs, while Framework owns discovery, contract versions, frontend
factory selection, lifecycle transitions and portable layout recipes.

## Architecture

The model separates five responsibilities:

1. `UmiApplicationComponentContract` adds semantic version, supported frontend,
   factory identity, default region and minimum geometry to a catalogue record.
2. `UmiApplicationComponentRegistry` is the single discovery point for built-in
   and future contributed contracts.
3. `UmiApplicationComponentFactoryRegistry` binds a contract to headless, GTK4,
   Qt6 or web adapters without introducing toolkit types into application code.
4. `UmiApplicationComponentInstance` owns portable identity and enforces the
   created, active, suspended, closed and failed lifecycle.
5. `UmiApplicationComponentLayout` materialises component IDs into regional
   workspaces transactionally, cleaning up already-created instances on error.

The 132 catalogue entries are now separate include records under
`src/application/component_catalogue/<domain>/`. Their compiled order and public
metadata remain unchanged, but each Lego-like component can be reviewed and
governed independently.

## Reusable bundles

Framework provides common, Studio and Trader bundles as initial reference
recipes. They are not product-owned widget trees: each bundle is a list of
component IDs and regions that any supported frontend can materialise.

Run the headless proof with:

```powershell
.\build\windows-ucrt64-debug\bin\umicom-component-composer.exe studio
.\build\windows-ucrt64-debug\bin\umicom-component-composer.exe trader
```

Application repositories should select Framework component IDs, assign stable
instance IDs and provide presentation metadata. New reusable behavior belongs
in Framework; application-local code should contain only product-specific
orchestration and branding.
