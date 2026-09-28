# Financial close review laboratory

Build this directory with CMake and a C23 compiler. It compiles a focused subset of the canonical finance, Data Server, matching and read-only broker components; it is not the complete product build.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --parallel 2 --no-tests=error --output-on-failure
.\build\bin\umicom-finance-close-review.exe demo
```

Stop after any failed command. `UMICOM_CLOSE_REVIEW_SQLITE=OFF` disables only the optional canonical SQLite backend; those tests then skip. `UMICOM_CLOSE_REVIEW_SANITIZE=ON` instruments supported native builds. The GUI is built by the complete applications composition, not this headless example. The current serial service and actor-label simulation restrictions remain.

Read `docs/learning/finance-close-review.html` and `docs/development/finance-close-review.md` in the Framework checkout. The complete `lesson.c` uses public APIs only. Its expected observations are fixed fictional data, not external reconciliation evidence.
