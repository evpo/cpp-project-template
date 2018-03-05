@echo off
setlocal

set test_pass=0
set test_fail=0
set fail_names=

for /d %%a in (*) do (
  if %%a neq CVS if %%a neq ide if not exist %%a\.norun call :test_case %%a
)

echo Tests passed: %test_pass%
echo Tests failed: %test_fail% (%fail_names% )
echo ##################################################

goto :EOF

:: Execute a test.
:: %1 is the name of the test folder.
:: The test executable and the folder must
:: have the same name.
::
:test_case
echo # testing %1
set original=%cd%
cd %1
%1
if ERRORLEVEL 1 (
  echo ERROR in %1.exe
  set /a test_fail=test_fail+1
  set fail_names=%fail_names% %1
) else (
  set /a test_pass=test_pass+1
)
echo ##################################################
cd /d %original%
goto :EOF
