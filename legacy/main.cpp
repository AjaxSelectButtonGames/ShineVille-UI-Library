#define SDL_MAIN_HANDLED

#include "sv_ui2.0.h"
#include "sv_ui_styles.h"

void initializeCustomStyles() {
    // Custom button style
    SV_UI::ButtonStyle customButtonStyle = {
        { 0.9f, 0.9f, 0.9f }, // Normal color
        { 1.0f, 1.0f, 1.0f }, // Hover color
        { 0.8f, 0.8f, 0.8f }  // Pressed color
    };
    SV_UI::styles.setButtonStyle(customButtonStyle);

    // Custom widget style
    SV_UI::WidgetStyle customWidgetStyle = {
        { 0.5f, 0.0f, 0.5f }, // Background color (purple)
    };

    // Custom border style
    float borderColor[3] = { 0.5f, 0.0f, 0.0f }; // Border color (black)
    SV_UI::BorderStyle customBorderStyle(borderColor, 5, true);
    customWidgetStyle.borderStyle = customBorderStyle;

    SV_UI::styles.setWidgetStyle(customWidgetStyle);
}


void sv_uiExample() {
    // Init SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("UI Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1800, 700, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to init GLEW " << std::endl;
    }


    int windowWidth;
    int windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    // After creating the OpenGL context
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    SV_UI::TextRenderer textRenderer("arial.ttf", 24);

    GLuint dummyTexture = 0;

    initializeCustomStyles(); // Set custom styles if needed

    // Create widgets and components as usual
    SV_UI::createWidget(100, 100, 400, 400, 1, dummyTexture, SV_UI::WIDGET_NONE);
    SV_UI::addTextComponent(&textRenderer, "Hello, World!", 10, 10, SV_UI::Alignment::Center);
    SV_UI::addButtonComponent(0, 0, 100, 30, []() {
        // Button function in here
        SV_UI::TextRenderer textRenderer2("arial.ttf", 20);
        SV_UI::addTextComponent(&textRenderer2, "Button Pressed!", 10, 10, SV_UI::Alignment::Center);
        }, "Button Text", &textRenderer);

    

    SV_UI::endWidget();
    

    /**
    SV_UI::createWidget(0, 0, windowWidth, windowHeight, 1, dummyTexture, SV_UI::WIDGET_NONE);
    // Define menu items and their callbacks
    std::vector<std::string> menuItems = { "File", "Edit", "View", "Help" };
    std::vector<std::function<void()>> callbacks = {
        []() { std::cout << "File clicked\n"; },
        []() { std::cout << "Edit clicked\n"; },
        []() { std::cout << "View clicked\n"; },
        []() { std::cout << "Help clicked\n"; }
    };

    // Add the menu items to the current widget
    SV_UI::addMenuItems(menuItems, callbacks, &textRenderer);
    SV_UI::endWidget();
    **/
    
    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            SV_UI::handleUIManagerEvents(SV_UI::uiManager, &event);
        }

        // Clear the screen and draw the UI
        glClear(GL_COLOR_BUFFER_BIT);
        SV_UI::drawUIManager(SV_UI::uiManager);
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    sv_uiExample();
    return 0;
}
