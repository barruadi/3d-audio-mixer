#pragma once

#include "../window/window.hh"

#define INITIAL_HEIGHT 720
#define INITIAL_WIDTH  1024
#define APP_NAME       "3D Audio Mixer"

class Application
{
    private:
        static Application* sInstance;

        // Main Window
        std::unique_ptr<nwindow::Window> main_window;

    public:
        Application(const std::string& name);

        void loop();
        
        static Application& Instance() { return *sInstance; }
};