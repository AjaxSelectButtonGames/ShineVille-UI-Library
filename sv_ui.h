/// <summary>
///	ShineVille UserInterFace Library designed for tools, client, server, and anything else SV releated
/// </summary>
#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <iostream>
#include <type_traits>

class DraggableComponent;


class UI
{
public:
	virtual void Draw() = 0;
	virtual void handleEvents(SDL_Event* event) = 0;
	virtual void updatePosition(float deltaX, float deltaY) = 0;
	virtual ~UI() {}
	
};

class UIComponent : public UI
{
protected:
	float x, y; // Position relative to the parent widget
  
public:
	UIComponent(float x = 0, float y = 0) : x(x), y(y) {}

	virtual void Draw() = 0;
	virtual void handleEvents(SDL_Event* event) = 0;
	virtual void updatePosition(float deltaX, float deltaY) override {
		x += deltaX;
		y += deltaY;
	}
	virtual ~UIComponent() {}
    float getX() const { return x; }
    float getY() const { return y; }
};


class DraggableComponent : public UIComponent
{
    bool isDragging = false;
    int lastMouseX, lastMouseY;
    widget* parentWidget;



public:
    DraggableComponent(widget* parent, float x = 0, float y = 0) : UIComponent(x, y), parentWidget(parent), lastMouseX(0), lastMouseY(0) {}


    void Draw() override {
        //this will remain null as the widget does not need to draw anything while it's being dragged.
    }

    void handleEvents(SDL_Event* event) override {
        // Use getters to access widget properties
        int widgetX = parentWidget->getX();
        int widgetY = parentWidget->getY();
        int width = parentWidget->getWidth();
        int height = parentWidget->getHeight();

        // Handle mouse button press
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            if (event->button.x > widgetX && event->button.x < widgetX + width && event->button.y > widgetY && event->button.y < widgetY + height) {
                isDragging = true; // Start dragging
                lastMouseX = event->button.x; // Record the starting mouse position
                lastMouseY = event->button.y;
            }
        }

        // Handle mouse button release
        if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
            isDragging = false; // Stop dragging
        }

        // Handle mouse movement
        if (event->type == SDL_MOUSEMOTION && isDragging) {
            // Calculate the difference between the current and last mouse positions
            int deltaX = event->motion.x - lastMouseX;
            int deltaY = event->motion.y - lastMouseY;

            // Update the widget's position
            parentWidget->setX(widgetX + deltaX);
            parentWidget->setY(widgetY + deltaY);

            // Update positions of all components in the widget
            for (auto& component : parentWidget->getComponents()) {
                component->updatePosition(deltaX, deltaY);
            }

            // Update the last mouse position
            lastMouseX = event->motion.x;
            lastMouseY = event->motion.y;
        }



    }



    void updatePosition(float deltaX, float deltaY) override {
        x += deltaX;
        y += deltaY;
    }
};






enum class WidgetFlags : uint32_t {
    None = 0,
    Draggable = 1 << 0,
    Resizable = 1 << 1,
    // Add more flags as needed
};

// Enable bitwise operations for WidgetFlags
inline WidgetFlags operator|(WidgetFlags a, WidgetFlags b) {
    return static_cast<WidgetFlags>(static_cast<std::underlying_type_t<WidgetFlags>>(a) | static_cast<std::underlying_type_t<WidgetFlags>>(b));
}

inline WidgetFlags operator&(WidgetFlags a, WidgetFlags b) {
    return static_cast<WidgetFlags>(static_cast<std::underlying_type_t<WidgetFlags>>(a) & static_cast<std::underlying_type_t<WidgetFlags>>(b));
}


//main widget class that allows us to make our UI it will contain all the UI elements 
class widget : public UI {
    std::vector<UIComponent*> components;
    int x, y, width, height, ID;
    GLuint texture;
    bool isResizing = false;
    bool resizingLeft = false, resizingRight = false, resizingTop = false, resizingBottom = false;
    WidgetFlags flags;
    
    SDL_Cursor* cursorArrow = nullptr;
    SDL_Cursor* cursorSizeWE = nullptr;
    SDL_Cursor* cursorSizeNS = nullptr;
    SDL_Cursor* currentCursor = nullptr;
    int lastMouseX = 0;
    int lastMouseY = 0;
 public:
    //getters for position and size
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const std::vector<UIComponent*>& getComponents() const {
        return components;
    }
    //Setters
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }


    widget(int x, int y, int width, int height, int ID, GLuint texture, WidgetFlags flags = WidgetFlags::None)
        : x(x), y(y), width(width), height(height), ID(ID), texture(texture), flags(flags)
    {
        if ((flags & WidgetFlags::Draggable) != WidgetFlags::None) {
            AddComponent(new DraggableComponent(this));
        }
        // Handle other flags as needed
    }

    void AddComponent(UIComponent* component) {
        components.push_back(component);
    }

    

    void initCursors() {
        cursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        cursorSizeWE = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        cursorSizeNS = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        currentCursor = cursorArrow; // Default cursor
    }

    void freeCursors() {
        SDL_FreeCursor(cursorArrow);
        SDL_FreeCursor(cursorSizeWE);
        SDL_FreeCursor(cursorSizeNS);
    }

    void updateCursor(SDL_Cursor* newCursor) {
        if (currentCursor != newCursor) {
            SDL_SetCursor(newCursor);
            currentCursor = newCursor;
        }
    }

    void Draw() override {
        // Draw the widget itself
        glPushMatrix();
        glTranslatef(x, y, 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(width, 0);
        glTexCoord2f(1, 1); glVertex2f(width, height);
        glTexCoord2f(0, 1); glVertex2f(0, height);
        glEnd();
        glPopMatrix();

        // Get window size
        int windowHeight;
        SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), NULL, &windowHeight);

        // Enable scissor test and set the scissor rectangle
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, windowHeight - (y + height), width, height);

        // Now, delegate to components to draw anything additional
        for (auto& component : components) {
            component->Draw();
        }

        // Disable scissor test after drawing components
        glDisable(GL_SCISSOR_TEST);
    }

    void handleEvents(SDL_Event* event) override {
        for (auto& component : components) {
            component->handleEvents(event);
        }

        int mouseX = event->button.x;
        int mouseY = event->button.y;
        int edgeThreshold = 10; // How close the mouse needs to be to the edge to start resizing
        bool shouldSaveConfig = false; // Flag to determine if we need to save the config

        // Handle mouse button press
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            // Reset resizing flags
            resizingLeft = resizingRight = resizingTop = resizingBottom = false;

            // Check if the mouse is near any of the widget's edges
            if (mouseX >= x + width - edgeThreshold && mouseX <= x + width) resizingRight = true;
            if (mouseX >= x && mouseX <= x + edgeThreshold) resizingLeft = true;
            if (mouseY >= y && mouseY <= y + edgeThreshold) resizingTop = true;
            if (mouseY >= y + height - edgeThreshold && mouseY <= y + height) resizingBottom = true;

            if (resizingLeft || resizingRight || resizingTop || resizingBottom) {
                isResizing = true;
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
        }

        // Handle mouse button release
        if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
            isResizing = false;
            shouldSaveConfig = true; // Save config on mouse release if there was a resize
        }

        // Handle mouse movement for resizing and cursor updates
        if (event->type == SDL_MOUSEMOTION) {
            mouseX = event->motion.x;
            mouseY = event->motion.y;
            int deltaX = mouseX - lastMouseX;
            int deltaY = mouseY - lastMouseY;

            // Update cursor type based on mouse position relative to widget edges
            if (!isResizing) {
                if (mouseX >= x + width - edgeThreshold && mouseX <= x + width) {
                    updateCursor(cursorSizeWE); // Right edge
                }
                else if (mouseX >= x && mouseX <= x + edgeThreshold) {
                    updateCursor(cursorSizeWE); // Left edge
                }
                else if (mouseY >= y && mouseY <= y + edgeThreshold) {
                    updateCursor(cursorSizeNS); // Top edge
                }
                else if (mouseY >= y + height - edgeThreshold && mouseY <= y + height) {
                    updateCursor(cursorSizeNS); // Bottom edge
                }
                else {
                    updateCursor(cursorArrow); // Default cursor
                }
            }

            // Handle resizing logic
            if (isResizing) {
                if (resizingRight) {
                    width += deltaX;
                    shouldSaveConfig = true;
                }
                if (resizingLeft) {
                    x += deltaX;
                    width -= deltaX;
                    shouldSaveConfig = true;
                }
                if (resizingTop) {
                    y += deltaY;
                    height -= deltaY;
                    shouldSaveConfig = true;
                }
                if (resizingBottom) {
                    height += deltaY;
                    shouldSaveConfig = true;
                }

                // Update the last mouse position
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
        }

    }
    void updatePosition(float deltaX, float deltaY) override {
        x += deltaX;
        y += deltaY;
        for (auto& component : components) {
            // Directly pass the delta values to the component's updatePosition method
            component->updatePosition(deltaX, deltaY);
        }
    }

  
};

/// <summary>
///	Manages all the UI elements
/// </summary>
class UIManager {
	std::vector<UI*> uiS;

public:

	void AddWidget(UI* widget) {
		uiS.push_back(widget);
	}

	void Draw()
	{
		for (auto element : uiS)
		{
			element->Draw();
		}
	}

	void handleEvents(SDL_Event* event)
	{
		for (auto element : uiS)
		{
			element->handleEvents(event);
		}
	}
};