# Create a native GTK project with shared branding

The C23 GTK4, C++ GTK4 and Umicom Thin Desktop Application recipes use the Framework application-branding helper. Choose the C23 recipe unless the project has a real C++ dependency. C++ is a consumer of the same C Framework contracts.

These recipes keep the existing small window content. The build stages canonical SVG assets and, on Windows, embeds the approved ICO identity. The generated main function requests native window identity before presentation and reports an unavailable identity as a warning.

## Prepare the source and SDK

1. Select the recipe in the existing project-generation workflow and review its rendered files. The recipe IDs are developer.template.c23-gtk4, developer.template.cpp-gtk4 and developer.template.thin-desktop-application.
2. Use an installed Framework SDK exporting Umicom::ui_gtk4. A core-only SDK is insufficient. Check that its platform, architecture and compiler environment match this application.
3. Alternatively, supply an explicitly selected Framework source checkout in the generated project's framework folder. That branch uses its own CMake helper and resources; do not mix assets from another checkout.
4. Inspect CMakeLists.txt. It should call umicom_apply_application_branding with the product name, application ID and selected resource root. It should link Umicom::ui_gtk4.
5. Keep the generated source headers and the existing project implementation when extending the application.

The recipes require current Framework interfaces. A package name alone does not prove that an older SDK exports the window-identity function. Link qualification is required.

## Build, install and inspect

For a generated target named umicom_example, run these commands from its root. The prefix is an example; use the actual qualified SDK location. Stop on the first error.

~~~powershell
$env:Path = "C:/msys64/ucrt64/bin;$env:Path"
cmake --preset windows-ucrt64-debug -DCMAKE_PREFIX_PATH=C:/umicom/installed/framework
cmake --build --preset windows-ucrt64-debug --parallel 2
Get-ChildItem .\build\windows-ucrt64-debug\bin\branding
.\build\windows-ucrt64-debug\bin\umicom_example.exe
cmake --install .\build\windows-ucrt64-debug --prefix .\install
cpack --config .\build\windows-ucrt64-debug\CPackConfig.cmake
~~~

The shared helper supplies resource compilation, native version identity, runtime deployment and desktop-entry integration for supported platforms. Inspect its actual output. Keep the branding directory beside the executable when moving the build. An archive being created is not proof that all runtime dependencies are present on another computer.

The operating system decides which window icon surfaces it displays. Check the executable's icon separately from the titlebar or task switcher. Missing artwork is reported; the recipe does not fabricate another logo.

## Fonts and content branding

The minimal generated window currently inherits GTK's font choices. This change does not install a managed appearance controller or a branded content header. A real product should compose the existing Framework appearance and workstation services, follow its approved design, preserve the user's profile and verify actual font fallback and DPI behaviour.

The shared native profiles already distinguish interface and code font roles. Reuse those roles instead of placing CSS font names in every product. SVG geometry and ICO identity are packaging contracts; fonts and visual layout require native observation.

## Focused recipe coverage

From the Framework checkout:

~~~powershell
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-gtk-starter-branding-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]developer_project[.]gtk-branding[.]"
~~~

The three authored cases render the real recipes into owned memory. They inspect the shared SDK/helper route, native call, headers, substituted identity and honest README limits. They do not configure generated files, link GTK, open a window or inspect Windows resources. All execution remains a separate owner qualification step.

The generated starter has no application-specific test suite yet. Add tests for its behaviour before presenting a successful compile as product readiness.

See [project recipe admission](project-template-admission.md) for ownership, capacity and refusal semantics, and the [worked branding lesson](../learning/create-a-branded-native-project.html) for a review exercise.
