# Umicom assembly learning project

Start with the matching practicum in Education Studio or Studio's Guided Learning page.
These are complete, inspectable reference files. Read them, run the baseline tests,
then make the lesson's small change without removing previous tests or comments.

The source is a learning example, not a replacement for a production Framework service.
The framework project uses the real Data Server. The notes project is a bounded C
ownership exercise. The assembly project explicitly labels its native or fallback path.

## Build and check

Open PowerShell in this exported folder. Keep source files outside the build folder.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw "Configure failed" }
cmake --build build --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Build failed" }
ctest --test-dir build --no-tests=error --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Tests failed" }
```

The framework project requires an installed UmicomFramework SDK. Pass its prefix with
`-DCMAKE_PREFIX_PATH="C:/umicom/Umicom-SDK"` when configuring. Its optional GTK view is
selected with `-DUMICOM_LESSON_WITH_GTK=ON`; this requires the real GTK4 development files.
Do not interpret headless test success as graphical validation.

In Studio use its existing folder/workspace opening workflow to open this directory;
read the header, implementation and test side by side. No compiler or student program
is launched merely by exporting or opening a lesson. Building a project executes its
build rules: inspect projects before building them and use your own trusted practice folder.

## Evidence

Keep your actual CTest output with your project. The Education quiz record is separate
and never claims that these tests ran. Export is a one-way copy: edits here do not alter
the Framework's built-in teaching sources. No learner files are silently overwritten.

Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
