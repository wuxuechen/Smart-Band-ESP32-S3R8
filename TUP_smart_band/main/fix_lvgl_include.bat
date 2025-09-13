@echo off
setlocal enabledelayedexpansion

cd ui

for %%f in (*.h) do (
    echo Processing %%f
    powershell -Command "(Get-Content '%%f') -replace '#include <lvgl/lvgl.h>', '#include <lvgl.h>' | Set-Content '%%f'"
)

echo Done! Check your ui folder headers.
pause
