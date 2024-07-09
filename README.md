# ShineVille UI Library (Legacy)

## Overview
The ShineVille UI Library is a set of reusable UI components designed to facilitate the development of SDL-based applications. This documentation covers the legacy version of the library.

## Installation
To use the ShineVille UI Library, include the necessary headers and link against SDL, SDL_ttf, and OpenGL libraries in your project.

## Usage
### Creating a Widget
```cpp
#include "sv_ui2.0.h"

int main(int argc, char* argv[]) {
    // Initialize SDL, create a window, etc.

    // Create a widget
    GLuint texture; // Load or generate texture
    SV_UI::Widget* widget = SV_UI::createWidget(50, 50, 200, 100, 1, texture, SV_UI::WIDGET_DRAGGABLE);

    // Add components to the widget
    SV_UI::TextRenderer textRenderer("path/to/font.ttf", 24);
    SV_UI::addTextComponent(&textRenderer, "Hello, World!", 10, 10, SV_UI::Alignment::TopLeft);

    SV_UI::addButtonComponent(10, 50, 180, 30, []() {
        std::cout << "Button clicked!" << std::endl;
    });

    SV_UI::endWidget();

    // Main loop
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            SV_UI::HandleEvents(&event);
        }

        // Clear screen
        // Draw UI
        SV_UI::Draw();
        // Update screen
    }

    return 0;
}
```

## API List

### Namespaces
- `SV_UI`

### Enums
- `Alignment`
  - `TopLeft`, `TopCenter`, `TopRight`
  - `CenterLeft`, `Center`, `CenterRight`
  - `BottomLeft`, `BottomCenter`, `BottomRight`

### Structures
- `Widget`
  - `int x, y, width, height, ID`
  - `GLuint texture`
  - `bool isResizing, resizingLeft, resizingRight, resizingTop, resizingBottom`
  - `std::vector<UIComponent*> components`
  - `DraggableComponent* draggableComponent`
  - `TextComponent* textComponent`
- `UIComponent`
  - `float x, y`
  - `int width, height`
  - `Widget* parent`
  - `virtual void Draw() = 0`
  - `virtual void handleEvents(SDL_Event* event) = 0`
  - `virtual void updatePosition(float deltaX, float deltaY)`
- `DraggableComponent`
  - `bool isDragging`
  - `int lastMouseX, lastMouseY`
  - `int offsetX, offsetY`
  - `Widget* parent`
- `UIManager`
  - `std::vector<Widget*> widgets`
  - `Widget* currentWidget`
- `TextRenderer`
  - `TTF_Font* font`
  - `float fontSize`
  - `TextRenderer(const char* fontPath, int size)`
  - `void RenderText(const std::string& text, float x, float y)`
  - `int getTextWidth(const std::string& text)`
  - `int getTextHeight(const std::string& text)`
- `TextComponent : public UIComponent`
  - `TextRenderer* textRenderer`
  - `std::string text`
  - `float offsetX, offsetY`
  - `Alignment alignment`
  - `TextComponent(TextRenderer* textRenderer, const std::string& text, float offsetX, float offsetY, Alignment alignment)`
  - `void Draw() override`
  - `void handleEvents(SDL_Event* event) override`
  - `void setText(const std::string& newText)`
  - `const std::string& getText() const`
  - `float getOffsetX() const`
  - `float getOffsetY() const`
- `ButtonComponent : public UIComponent`
  - `std::function<void()> onClick`
  - `bool isHovered, isPressed`
  - `float normalColor[3], hoverColor[3], pressedColor[3]`
  - `ButtonComponent(std::function<void()> onClickCallBack, float normal[3], float hover[3], float pressed[3])`
  - `void Draw() override`
  - `void handleEvents(SDL_Event* event) override`

### Functions
- `Widget* createWidget(int x, int y, int width, int height, int ID, GLuint texture, int options)`
- `void addTextComponent(TextRenderer* textRenderer, const std::string& text, float offsetX, float offsetY, Alignment alignment)`
- `void addButtonComponent(int x, int y, int width, int height, std::function<void()> onClickCallback)`
- `void endWidget()`
- `void Draw()`
- `void HandleEvents(SDL_Event* event)`
