@echo off

set folder=%1

cd %folder%
set folder=%cd%

echo Changing BMPs in %folder% to PNGs

for %%A IN (*.bmp) DO (
	echo Converting %%A to a PNG
	magick convert "%%A" "%%~nA.png"
	del /f "%%A"
)