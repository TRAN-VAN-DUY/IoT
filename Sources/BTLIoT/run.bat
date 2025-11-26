@echo off
echo Building and running Gate Controller Application...

REM Check if Maven is installed
where mvn >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Maven is not installed or not in PATH!
    echo Please install Maven first.
    pause
    exit /b 1
)

echo Cleaning and building project...
mvn clean install

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Starting application...
    mvn spring-boot:run -Dspring-boot.run.jvmArguments="--enable-native-access=ALL-UNNAMED"
) else (
    echo Build failed!
    pause
)
