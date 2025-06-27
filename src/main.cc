// App Entry Point

#include "app/application.hh"

int main(void)
{
    auto app = std::make_unique<Application>(APP_NAME);
    app->loop();

    return 0;
}