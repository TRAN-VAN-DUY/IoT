#!/bin/bash
echo "Building and running Gate Controller Application..."

# Check if Maven is installed
if ! command -v mvn &> /dev/null; then
    echo "Maven is not installed or not in PATH!"
    echo "Please install Maven first."
    exit 1
fi

echo "Cleaning and building project..."
mvn clean install

if [ $? -eq 0 ]; then
    echo "Build successful! Starting application..."
    mvn spring-boot:run -Dspring-boot.run.jvmArguments="--enable-native-access=ALL-UNNAMED"
else
    echo "Build failed!"
    exit 1
fi
