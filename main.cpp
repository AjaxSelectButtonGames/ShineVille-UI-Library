#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include "sv_ui2.0.h"
#include "sv_ui_styles.h"

void initializeCustomStyles() {
    SV_UI::ButtonStyle customButtonStyle = {
        { 0.9f, 0.9f, 0.9f }, // Normal color
        { 1.0f, 1.0f, 1.0f }, // Hover color
        { 0.8f, 0.8f, 0.8f }  // Pressed color
    };
    SV_UI::styles.setButtonStyle(customButtonStyle);
}



void sv_uiExample()
{
    // Init SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("UI Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1800, 700, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GL_CreateContext(window);

    // After creating the OpenGL context
    glViewport(0, 0, 1800, 700); // Match your window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1800, 700, 0, -1, 1); // Left, Right, Bottom, Top, Near, Far
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    SV_UI::TextRenderer textRenderer("arial.ttf", 24);


    GLuint dummyTexture = 0;
    // Assuming you have a pointer to the widget you want to add the text component to
    
    initializeCustomStyles(); // Set custom styles if needed

    // Create widgets and components as usual
    SV_UI::createWidget(100, 100, 200, 100, 1, dummyTexture, SV_UI::WIDGET_DRAGGABLE);
    SV_UI::addTextComponent(&textRenderer, "Hello, World!", 10, 10, SV_UI::Alignment::Center);

    SV_UI::addButtonComponent(50, 50, 100, 30, []() { std::cout << "Button clicked!" << std::endl; });
    SV_UI::endWidget();
    

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            SV_UI::HandleEvents(&event);
        }

        // Clear the screen and draw the UI
        glClear(GL_COLOR_BUFFER_BIT);
        SV_UI::Draw();
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
