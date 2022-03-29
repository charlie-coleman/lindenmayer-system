@echo off

( endlocal
FOR /L %%x IN (1, 1, %3) DO (
  call %1 -c %2 -o %4\%%x.png
)
)