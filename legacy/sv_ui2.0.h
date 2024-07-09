
#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <vector>
#include <iostream>
#include <functional>
#include <string>
#include "sv_ui_styles.h"
namespace SV_UI {

    enum class Alignment {
        TopLeft,
        TopCenter,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomCenter,
        BottomRight
    };

    struct Widget; // Forward declaration
    struct TextRenderer; // Ensure this is forward-declared if its full definition comes later
    struct TextComponent;
    struct UIComponent {
        float x = 0.0f, y = 0.0f; // Initialized
        int width = 0, height = 0; // Initialized
        Widget* parent = nullptr;
        virtual void Draw() = 0;
        virtual void handleEvents(SDL_Event* event) = 0;
        virtual void updatePosition(float deltaX, float deltaY) {
            x += deltaX;
            y += deltaY;
        }
    };

    struct DraggableComponent {
        bool isDragging = false;
        int lastMouseX = 0, lastMouseY = 0;
        int offsetX = 0, offsetY = 0;
        Widget* parent = nullptr; // Correctly declared
    };

    struct Widget {
        int x, y, width, height, ID;
        GLuint texture;
        bool isResizing = false;
        bool resizingLeft = false, resizingRight = false, resizingTop = false, resizingBottom = false;
        std::vector<UIComponent*> components;
        DraggableComponent* draggableComponent = nullptr;
        TextComponent* textComponent = nullptr; // Ensure this is declared correctly
    };

    struct UIManager {
        std::vector<Widget*> widgets;
        Widget* currentWidget = nullptr; // Track the current widget context
    };

    // Global UIManager instance
    UIManager uiManager;

    // Flags for widget options
    enum WidgetOptions {
        WIDGET_NONE = 0,
        WIDGET_DRAGGABLE = 1 << 0,
        WIDGET_RESIZABLE_LEFT = 1 << 1,
        WIDGET_RESIZABLE_RIGHT = 1 << 2,
        WIDGET_RESIZABLE_TOP = 1 << 3,
        WIDGET_RESIZABLE_BOTTOM = 1 << 4
    };

    // Utility function to set flag
    bool hasFlag(int options, WidgetOptions flag) {
        return (options & flag) == flag;
    }

    // Functions for UIComponent
    void updatePosition(UIComponent& component, float deltaX, float deltaY) {
        component.x += deltaX;
        component.y += deltaY;
    }

    // Functions for DraggableComponent
    void handleDrag(DraggableComponent& draggable, SDL_Event* event) {
        Widget& parent = *draggable.parent;

        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            int mouseX = event->button.x;
            int mouseY = event->button.y;
            if (mouseX > parent.x && mouseX < parent.x + parent.width &&
                mouseY > parent.y && mouseY < parent.y + parent.height) {
                draggable.isDragging = true;
                draggable.offsetX = mouseX - parent.x; // Calculate offsets
                draggable.offsetY = mouseY - parent.y;
            }
        }
        else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
            draggable.isDragging = false;
        }
        else if (event->type == SDL_MOUSEMOTION && draggable.isDragging) {
            int mouseX = event->motion.x;
            int mouseY = event->motion.y;
            parent.x = mouseX - draggable.offsetX; // Apply offsets to keep the widget under the cursor correctly
            parent.y = mouseY - draggable.offsetY;
        }
    }

    // Functions for Widget
    void drawWidget(const Widget& widget) {
        glPushMatrix();
        glTranslatef(widget.x, widget.y, 0);
        glBindTexture(GL_TEXTURE_2D, widget.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(widget.width, 0);
        glTexCoord2f(1, 1); glVertex2f(widget.width, widget.height);
        glTexCoord2f(0, 1); glVertex2f(0, widget.height);
        glEnd();
        glPopMatrix();

        for (const auto component : widget.components) {
            component->Draw();
        }
    }

    void handleWidgetEvents(Widget& widget, SDL_Event* event) {
        for (auto component : widget.components) {
            component->handleEvents(event);
        }
        if (widget.draggableComponent) {
            handleDrag(*widget.draggableComponent, event);
        }
    }

    // Functions for UIManager
    void drawUIManager(const UIManager& uiManager) {
        for (const auto widget : uiManager.widgets) {
            drawWidget(*widget);
        }
    }

    void handleUIManagerEvents(UIManager& uiManager, SDL_Event* event) {
        for (auto widget : uiManager.widgets) {
            handleWidgetEvents(*widget, event);
        }
    }

    // Text Rendering Functions
    struct TextRenderer {
        TTF_Font* font = nullptr;
        float fontSize;

        TextRenderer(const char* fontPath, int size) : fontSize(static_cast<float>(size)) {
            if (TTF_Init() != 0) {
                throw std::runtime_error("Failed to initialize TTF: " + std::string(TTF_GetError()));
            }

            font = TTF_OpenFont(fontPath, size);
            if (!font) {
                TTF_Quit(); // Ensure TTF is cleaned up if font loading fails
                throw std::runtime_error("Failed to load font: " + std::string(TTF_GetError()));
            }
        }

        void RenderText(const std::string& text, float x, float y) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix();
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(0, 1800, 700, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if (!font) {
                std::cerr << "Font not loaded correctly!" << std::endl;
                return;
            }

            SDL_Color color = { 0, 0, 0 };
            SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
            if (surface == nullptr) {
                std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
                return;
            }

            SDL_Surface* glCompatibleSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#else
                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#endif
            );

            SDL_BlitSurface(surface, NULL, glCompatibleSurface, NULL);
            SDL_FreeSurface(surface); // Free the original surface

            GLuint textureId;
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glCompatibleSurface->w, glCompatibleSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, glCompatibleSurface->pixels);

            SDL_FreeSurface(glCompatibleSurface); // Free the compatible surface

            glBindTexture(GL_TEXTURE_2D, textureId);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(x, y);
            glTexCoord2f(1, 0); glVertex2f(x + glCompatibleSurface->w, y);
            glTexCoord2f(1, 1); glVertex2f(x + glCompatibleSurface->w, y + glCompatibleSurface->h);
            glTexCoord2f(0, 1); glVertex2f(x, y + glCompatibleSurface->h);

            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
            glDeleteTextures(1, &textureId);

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glPopAttrib();
        }

        int getTextWidth(const std::string& text) {
            int width, height;
            TTF_SizeText(font, text.c_str(), &width, &height);
            return width;
        }

        int getTextHeight(const std::string& text) {
            int width, height;
            TTF_SizeText(font, text.c_str(), &width, &height);
            return height;
        }

        ~TextRenderer() {
            if (font) {
                TTF_CloseFont(font);
            }
            TTF_Quit();
        }
    };

    struct TextComponent : public UIComponent {
        TextRenderer* textRenderer;
        std::string text;
        float offsetX, offsetY;
        Alignment alignment;

        TextComponent(TextRenderer* textRenderer, const std::string& text = "", float offsetX = 10, float offsetY = 10, Alignment alignment = Alignment::TopLeft)
            : textRenderer(textRenderer), text(text), offsetX(offsetX), offsetY(offsetY), alignment(alignment) {}

        void Draw() override {
            if (parent) {
                float textX = parent->x + offsetX;
                float textY = parent->y + offsetY;

                // Adjust text position based on widget dimensions and alignment
                switch (alignment) {
                case Alignment::TopLeft:
                    // Default alignment (no adjustment needed)
                    break;
                case Alignment::TopCenter:
                    textX += (parent->width - textRenderer->getTextWidth(text)) / 2;
                    break;
                case Alignment::TopRight:
                    textX += parent->width - textRenderer->getTextWidth(text);
                    break;
                case Alignment::CenterLeft:
                    textY += (parent->height - textRenderer->getTextHeight(text)) / 2;
                    break;
                case Alignment::Center:
                    textX += (parent->width - textRenderer->getTextWidth(text)) / 2;
                    textY += (parent->height - textRenderer->getTextHeight(text)) / 2;
                    break;
                case Alignment::CenterRight:
                    textX += parent->width - textRenderer->getTextWidth(text);
                    textY += (parent->height - textRenderer->getTextHeight(text)) / 2;
                    break;
                case Alignment::BottomLeft:
                    textY += parent->height - textRenderer->getTextHeight(text);
                    break;
                case Alignment::BottomCenter:
                    textX += (parent->width - textRenderer->getTextWidth(text)) / 2;
                    textY += parent->height - textRenderer->getTextHeight(text);
                    break;
                case Alignment::BottomRight:
                    textX += parent->width - textRenderer->getTextWidth(text);
                    textY += parent->height - textRenderer->getTextHeight(text);
                    break;
                }

                textRenderer->RenderText(text, textX, textY);
            }
            else {
                textRenderer->RenderText(text, offsetX, offsetY);
            }
        }

        void handleEvents(SDL_Event* event) override {}

        void setText(const std::string& newText) {
            text = newText;
        }

        const std::string& getText() const { return text; }
        float getOffsetX() const { return offsetX; }
        float getOffsetY() const { return offsetY; }

        void updatePosition(float deltaX, float deltaY) override {
            offsetX += deltaX;
            offsetY += deltaY;
        }
    };


    //button component
    struct ButtonComponent : public UIComponent {
        std::function<void()> onClick; // Callback function for when button is clicked
        bool isHovered = false;
        bool isPressed = false;

        // Color attributes for different states
        float normalColor[3];
        float hoverColor[3];
        float pressedColor[3];

        ButtonComponent(std::function<void()> onClickCallBack,
            float normal[3], float hover[3], float pressed[3])
            : onClick(onClickCallBack) {
            std::copy(normal, normal + 3, normalColor);
            std::copy(hover, hover + 3, hoverColor);
            std::copy(pressed, pressed + 3, pressedColor);
        }

        void Draw() override {
            float drawX = x, drawY = y;
            if (parent) {
                drawX += parent->x;
                drawY += parent->y;
            }

            // Set color based on button state
            if (isPressed) {
                glColor3f(pressedColor[0], pressedColor[1], pressedColor[2]);
            }
            else if (isHovered) {
                glColor3f(hoverColor[0], hoverColor[1], hoverColor[2]);
            }
            else {
                glColor3f(normalColor[0], normalColor[1], normalColor[2]);
            }

            // Draw button
            glBegin(GL_QUADS);
            glVertex2f(drawX, drawY);
            glVertex2f(drawX + width, drawY);
            glVertex2f(drawX + width, drawY + height);
            glVertex2f(drawX, drawY + height);
            glEnd();

            // Reset color to default after drawing
            glColor3f(1.0f, 1.0f, 1.0f); // Reset to white
        }

        void handleEvents(SDL_Event* event) override {
            int mouseX = event->motion.x;
            int mouseY = event->motion.y;
            float offsetX = x, offsetY = y;
            if (parent) {
                offsetX += parent->x;
                offsetY += parent->y;
            }

            if (event->type == SDL_MOUSEMOTION) {
                isHovered = (mouseX > offsetX && mouseX < offsetX + width && mouseY > offsetY && mouseY < offsetY + height);
            }
            else if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && isHovered) {
                isPressed = true;
            }
            else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
                if (isPressed && isHovered && onClick) {
                    onClick(); // Trigger the callback function
                }
                isPressed = false;
            }
        }
    };





    Widget* createWidget(int x, int y, int width, int height, int ID, GLuint texture, int options) {
        Widget* widget = new Widget{ x, y, width, height, ID, texture };

        // Initialize draggable component if needed
        if (hasFlag(options, WIDGET_DRAGGABLE)) {
            widget->draggableComponent = new DraggableComponent();
            widget->draggableComponent->parent = widget;
        }

        // Initialize resizable flags if needed
        if (hasFlag(options, WIDGET_RESIZABLE_LEFT)) widget->resizingLeft = true;
        if (hasFlag(options, WIDGET_RESIZABLE_RIGHT)) widget->resizingRight = true;
        if (hasFlag(options, WIDGET_RESIZABLE_TOP)) widget->resizingTop = true;
        if (hasFlag(options, WIDGET_RESIZABLE_BOTTOM)) widget->resizingBottom = true;

        uiManager.widgets.push_back(widget);
        uiManager.currentWidget = widget; // Set the current widget context
        return widget;
    }

    void addTextComponent(TextRenderer* textRenderer, const std::string& text, float offsetX, float offsetY, Alignment alignment) {
        if (uiManager.currentWidget) {
            TextComponent* textComponent = new TextComponent(textRenderer, text, offsetX, offsetY, alignment);
            textComponent->parent = uiManager.currentWidget;
            uiManager.currentWidget->components.push_back(textComponent);
        }
    }

    void addButtonComponent(int x, int y, int width, int height,
        std::function<void()> onClickCallback) {
        if (uiManager.currentWidget) {
            // Use styles from the global styles instance
            ButtonComponent* buttonComponent = new ButtonComponent(onClickCallback,
                styles.buttonStyle.normalColor,
                styles.buttonStyle.hoverColor,
                styles.buttonStyle.pressedColor);
            buttonComponent->x = x;
            buttonComponent->y = y;
            buttonComponent->width = width;
            buttonComponent->height = height;
            buttonComponent->parent = uiManager.currentWidget;
            uiManager.currentWidget->components.push_back(buttonComponent);
        }
    }

    void endWidget() {
        uiManager.currentWidget = nullptr;
    }

    void Draw() {
        drawUIManager(uiManager);
    }

    void HandleEvents(SDL_Event* event) {
        handleUIManagerEvents(uiManager, event);
    }

} // namespace SV_UI

