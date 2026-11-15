![Umicom](../../resources/brand/umicom-logo.svg)

# Build an independent native About application

This C23 example uses the installed Umicom Framework package. Its build does not
clone Framework or add a Framework source folder. The application supplies its
name, version and licence; Framework supplies the existing GTK About layout,
canonical artwork and window-lifetime checks.

## Prepare the package

1. Obtain an installed Framework SDK built with the GTK4 adapter. It must
   contain `include/umicom/ui/gtk4/workstation/about_dialog.h` and the
   corresponding compiled implementation. An older SDK with the same major
   version may not contain this new API.
2. Use the compiler, architecture, runtime and ABI described by that SDK's
   environment manifest. A header alone or a DLL alone does not supply all
   headers, import/static libraries, GTK dependencies and runtime resources.
3. Copy this example folder to your own application workspace if desired.
   The executable does not read the logo path used by this README; its About
   icon comes from Framework's embedded approved SVG.

The following PowerShell example assumes a compatible Windows UCRT64 SDK
installed at `C:\SDKs\Umicom`. Substitute your actual package prefix. Run each
command separately and stop on an error.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_SDK_ROOT=C:/SDKs/Umicom
cmake --build build --parallel 2
& ".\build\umicom-sdk-about.exe"
```

Configuration should find the exported `Umicom::ui_gtk4` and `Umicom::product`
targets in the selected prefix. The environment guard checks the package
contract; it is not a substitute for building and running your application.
The branding helper stages the canonical assets and applies Windows icon
resources through the same package service used by Umicom applications.

## Read the C ownership boundary

`UmiGtk4AboutDialogCreate` returns a caller-owned window reference without
presenting it. The example adds the window to GtkApplication, presents it,
checks that it still exists, and releases its creation reference. GtkApplication
then controls how long the application remains running. Closing the last
window ends the example.

The version `1.0.0` describes this example, not the Framework release. Production
applications should use their own public version contract. When adding an About
action to an existing application, call `UmiGtk4AboutDialogPresent` with its
live owner instead. That service reuses one live About window per owner and
creates a replacement after closure.

## Verify and recover

- Check the product name, version, licence and approved icon; close the window
  and confirm the process exits.
- Check light/dark contrast, fonts and display scaling. Close and reopen after
  changing the theme. The About icon is decoded at 96 pixels; high-DPI appearance
  and native taskbar ICO integration require desktop inspection.
- If configuration cannot find the component, use a package containing GTK4.
  Do not point an installed-package guard at an arbitrary source checkout.
- If linking reports a missing About symbol, verify that the selected library
  and headers came from the same new SDK package.
- If SVG decoding fails, verify the deployed GTK image loaders. The service
  reports failure rather than substituting an unrelated logo.
- If Windows reports a missing runtime DLL, inspect the package's deployment
  guidance and compiler identity before adding unrelated directories to PATH.

This example is authored source. Its installed-consumer build and launch must
be qualified with the actual SDK package. Configuration of the Framework source
tree does not establish that this independent consumer has passed.

Continue with the [native window lesson](../../docs/learning/owned-window-presentation.html)
for metadata limits, weak caching, callback cancellation and regression commands.
