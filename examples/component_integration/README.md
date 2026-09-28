# Build one application from three shared components

The example uses the market tape, historical strategy replay and read-only
broker policy in one C23 executable. It uses fictional observations and never
opens a broker connection. Its source is `main.c`; the three component libraries
remain responsible for their own state and validation.

Configure this folder in a new build directory, build, then run CTest. The
laboratory requests production component integration before canonical owner
targets are declared. This is deliberately different from the older standalone
examples that include their modules only after all owners exist.

```sh
cmake -S . -B /path/to/new-build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build /path/to/new-build --parallel 2
ctest --test-dir /path/to/new-build --parallel 2 --no-tests=error --output-on-failure
```

Ten configuration checks compare actual test names, exercise automatic
regeneration and install a focused SDK for the separate `client` project.
Fixtures never change the original source tree. Their logs are retained below
`component-integration-checks` in the build tree. This laboratory is not the
complete Framework or a qualified installed Windows application.
