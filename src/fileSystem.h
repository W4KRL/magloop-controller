//! fileSystem.h
//! 2025-02-10

#include <LittleFS.h> // for index.html, styles.css, and script.js

// Place files in project \data folder
// Open PlatformIO Tasks by clicking on Alien Head
// Under Platform
// - Build Filesystem Image
// - Upload Filesystem Image

void littleFsBegin()
{
    // Initialize LittleFS for the web server files
    if (!LittleFS.begin())
    {
        Serial.println("Error mounting LittleFS");
        return;
    }
}