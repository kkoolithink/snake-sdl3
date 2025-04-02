@echo off

if exist snake.exe (
    del snake.exe
    echo Snake deleted
)

gcc main.c -o snake.exe -Isrc/include -Lsrc/lib -lSDL3 -lSDL3_ttf

if %errorlevel% == 0 (
    echo Snake running
    snake.exe
) else (
    echo Compilation failed.
)