//! fileSystem.h
//! 2025-02-10

#include <LittleFS.h> // for index.html, styles.css, and script.js

// Place files in project \data folder
// Open PlatformIO Tasks by clicking on Alien Head
// Under Platform
//   1. Build Filesystem Image, wait until complete
//   2. Upload Filesystem Image, wait until complete

void littleFsBegin()
{
    // Initialize LittleFS for the web server files
    if (!LittleFS.begin())
    {
        Serial.println("Error mounting LittleFS");
        return;
    }
}