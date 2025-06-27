#include "app/application.hh"

#include "window/window.hh"

Application::Application(const std::string& name) 
{
    main_window = std::make_unique<nwindow::Window>();
    main_window->init(INITIAL_WIDTH, INITIAL_HEIGHT, APP_NAME);
}

void Application::loop() 
{
    while (main_window->is_running()) 
    {
        main_window->render();
    }
}