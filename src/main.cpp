#include <iostream>
#include "sv_ui3.0.h"

const int SCREEN_WIDTH = 1800;
const int SCREEN_HEIGHT = 900;

bool initSDL(SDL_Window*& window, SDL_GLContext& context) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1)
    {
        std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("SV_UI Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return false;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cerr << "Failed to set VSync: " << SDL_GetError() << std::endl;
        return false;
    }

    glewExperimental = GL_TRUE; // Ensure this line is present
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization error: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void closeSDL(SDL_Window* window, SDL_GLContext context) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void debugOpenGL() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

GLuint loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path); // Use IMG_Load instead of SDL_LoadBMP
    if (!surface) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl; // Use IMG_GetError for error reporting
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Determine the format of the image
    GLenum format;
    if (surface->format->BytesPerPixel == 4) {
        format = surface->format->Rmask == 0x000000ff ? GL_RGBA : GL_BGRA;
    }
    else {
        format = surface->format->Rmask == 0x000000ff ? GL_RGB : GL_BGR;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format == GL_RGBA ? GL_RGBA : GL_RGB, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmaps for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture wrapping parameters (optional, but good to check)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Changed to GL_CLAMP_TO_EDGE for better edge handling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(surface);
    std::cout << "Texture loaded successfully from " << path << std::endl;
    return texture;
}


int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_GLContext context;

    if (!initSDL(window, context)) {
        std::cerr << "Failed to initialize SDL and OpenGL" << std::endl;
        return -1;
    }

    // Initialize the UI library
    SV_UI::initOpenGL();
    SV_UI::setProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Load texture
    GLuint texture = loadTexture("metalPanel_green.png");
    if (!texture) {
        std::cerr << "Failed to load texture" << std::endl;
        return -1;
    }
   
    auto buttonClickCallback = []() {
        std::cout << "Button Clicked!" << std::endl;
        };

    // Create some widgets
    SV_UI::createWidget(1, 0, 0, 400, 400, SV_UI::WidgetOptions::WIDGET_DRAGGABLE, "metalPanel_green.png");
    SV_UI::Text("Hello World",2.0f);
    SV_UI::Button("Click me", 2.0f,"", buttonClickCallback);
    
    SV_UI::endWidget();
    SV_UI::createWidget(2, 400, 100, 200, 150, SV_UI::WidgetOptions::WIDGET_DRAGGABLE, "metalPane3l_green.png");
    SV_UI::endWidget();
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            SV_UI::handleEvents(&event);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SV_UI::renderUI();

        SDL_GL_SwapWindow(window);
    }
   
    closeSDL(window, context);
    return 0;
}

