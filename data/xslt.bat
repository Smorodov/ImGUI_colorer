@echo off
rem Calls XSL transform of %1 throught %2 into %3 with parameters %4=%5 %6=%7 etc.


rem SET PROC=XALAN
SET FIN=%1
SET FSS=%2
SET FOUT=%3

:loop
if "%4"=="" goto end

if "%PROC%"=="XALAN" goto xalan
SET PARS=%PARS% %4=%5
goto other
:xalan
SET PARS=%PARS% -PARAM %4 %5

:other
shift
shift

goto loop
:end


rem Instant saxon 6.5.2
rem saxon.exe -o %FOUT% %FIN% %FSS%  %PARS%

rem Java saxon 7.x
java net.sf.saxon.Transform -o %FOUT% %FIN% %FSS% %PARS%

rem XALAN
rem java org.apache.xalan.xslt.Process -OUT %FOUT% -IN %FIN% -XSL %FSS% %PARS%

rem XT
rem xt %FIN% %FSS% %FOUT% %PARS%

rem buggy MSXSL js call variant
rem msxslt %FIN% %FSS% %FOUT%  %PARS%
rem buggy MSXSL exe gate
rem msxsl %FIN% %FSS% -o %FOUT% %PARS%
