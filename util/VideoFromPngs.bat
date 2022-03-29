set FRAMERATE=%1
set FILEPATH=%2
set OUTPUT=%3

cd %FILEPATH%

echo %CWD%

ffmpeg -framerate %FRAMERATE% -i %%d.png -c:v libx264 -pix_fmt yuv420p %OUTPUT%