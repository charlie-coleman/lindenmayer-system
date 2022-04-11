set FILEPATH=%1
set FORMAT=%2
set FRAMERATE=%3
set OUTPUT=%4

cd %FILEPATH%

echo %CWD%

ffmpeg -framerate %FRAMERATE% -i %FORMAT%.png -c:v libx264 -pix_fmt yuv420p %OUTPUT%