#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <vector>
#include <iostream>
#include <functional>
#include <string>
#define GLT_IMPLEMENTATION
#include "gltext.h"
#include "sv_ui_styles.h"

namespace SV_UI {
    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;
    glm::mat4 projection; // Declaration without initialization

    void setProjectionMatrix(int screenWidth, int screenHeight) {
        projection = glm::ortho(0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, -1.0f, 1.0f);
    }
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
        int x = 0, y = 0, width = 0, height = 0, ID = 0;
        GLuint texture = 0;
        bool isResizing = false;
        bool resizingLeft = false, resizingRight = false, resizingTop = false, resizingBottom = false;
        std::vector<UIComponent*> components;
        DraggableComponent* draggableComponent = nullptr;
        TextComponent* textComponent = nullptr;

        ~Widget() {
            delete textComponent;
            // Remember to delete components in the vector to avoid memory leaks
            for (auto& component : components) {
                delete component;
            }
        }
    };

    struct UIManager {
        std::vector<Widget*> widgets;
        Widget* currentWidget = nullptr; // Track the current widget context
        bool isCreatingWidget = false;
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
            // Calculate the delta movement
            int deltaX = mouseX - draggable.offsetX - parent.x;
            int deltaY = mouseY - draggable.offsetY - parent.y;
            // Update widget position
            parent.x = mouseX - draggable.offsetX;
            parent.y = mouseY - draggable.offsetY;
            // Update positions of all components relative to the new widget position
            for (auto component : parent.components) {
                component->updatePosition(deltaX, deltaY);
            }
        }
    }


    // Shader compilation and linking utility functions
    GLuint compileShader(const char* source, GLenum shaderType) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
            return 0;
        }

        return shader;
    }

    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Shader linking failed: " << infoLog << std::endl;
            return 0;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    // Vertex and Fragment shader sources
    const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    uniform mat4 model;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;

    uniform sampler2D texture1;
    uniform int useTexture; // Uniform flag to indicate whether to use the texture
    uniform vec4 fallbackColor;

    void main() {
        if (useTexture != 0) {
            FragColor = texture(texture1, TexCoord);
        } else {
            FragColor = fallbackColor;
        }
    }
)";

    // Initialize shader program and VAO, VBO
   

    void initOpenGL() {
        glewInit();

        shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
        gltInit();
        float vertices[] = {
            // positions    // texture coords
            0.0f,  1.0f,    0.0f, 1.0f,
            1.0f,  1.0f,    1.0f, 1.0f,
            1.0f,  0.0f,    1.0f, 0.0f,
            0.0f,  0.0f,    0.0f, 0.0f
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
       
       
    
    }


    ///////////////////////////////////////////////////////////////////////////////
 /////////////// TEXT RENDERING STRUCTS////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////////
    struct TextComponent : public UIComponent {
        std::string text;
        float fontSize;
        GLTtext* gltText;

        TextComponent(const std::string& text, float fontSize)
            : text(text), fontSize(fontSize) {
            gltText = gltCreateText();
            if (gltText == nullptr) {
               throw std::runtime_error("Failed to create text");
            }
            
        }

        

        virtual void Draw() override {
            gltSetText(gltText, text.c_str());
            gltBeginDraw();

            // Determine text position within widget for centered alignment
            float textX = x + width / 2.0f; // Centered horizontally
            float textY = y + height / 2.0f; // Centered vertically

            gltColor(1.0f, 1.0f, 1.0f, 1.0f); // Example: White color
            gltDrawText2DAligned(gltText, textX, textY, fontSize, GLT_CENTER, GLT_CENTER);

            gltEndDraw();
        }

        virtual void handleEvents(SDL_Event* event) override {
            // Handle events for text component if needed
        }

        ~TextComponent() {
            gltDeleteText(gltText);
            gltTerminate();

        }
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    // /////////////////////////////BUTTON COMPONENT OF WIDGETS///////////////////////////////
    // //////////////////////////////////////////////////////////////////////////////////////
    struct ButtonComponent : public UIComponent {
        TextComponent* textComponent = nullptr;
        GLuint texture = 0;
        std::function<void()> onClick;
        bool hasTexture = false; // New flag to indicate if the button has a texture

        ButtonComponent(const std::string text, float fontSize, const std::string& texturePath = "", std::function<void()> onClick = nullptr)
            : onClick(onClick) {
            if (!text.empty()) {
                textComponent = new TextComponent(text, fontSize);
            }

            // Load the texture if a path is provided and it's not empty
            if (!texturePath.empty()) {
                SDL_Surface* surface = IMG_Load(texturePath.c_str());
                if (surface) {
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D, texture);

                    GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
                    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    SDL_FreeSurface(surface);
                    hasTexture = true; // Texture successfully loaded
                }
                else {
                    std::cerr << "Failed to load texture " << IMG_GetError() << std::endl;
                }
            }
        }

        virtual void Draw() override {
            //draw the button texture if it exists
            if (hasTexture) {
                glUseProgram(shaderProgram);
                glBindVertexArray(VAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
                glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);

                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
                model = glm::scale(model, glm::vec3(width, height, 1.0f));
                GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
            }
            else {
                // Draw a basic colored square
                glUseProgram(shaderProgram);
                glBindVertexArray(VAO);
                glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0); // Indicate not using texture
                glUniform4f(glGetUniformLocation(shaderProgram, "fallbackColor"), 0.5f, 0.5f, 0.5f, 1.0f); // Example: Gray color

                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
                model = glm::scale(model, glm::vec3(width, height, 1.0f));
                GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
            }

            //draw text component if it exists
            if (textComponent) {
                textComponent->Draw();
            }
        }
        virtual void handleEvents(SDL_Event* event) override {
            // Handle click events
            if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
                int mouseX = event->button.x;
                int mouseY = event->button.y;
                // Check if the click is within the button's bounds
                if (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height) {
                    if (onClick) {
                        onClick(); // Call the callback function
                    }
                }
            }
        }

        ~ButtonComponent() {
            delete textComponent; // Clean up the text component
            if (texture) {
                glDeleteTextures(1, &texture); // Clean up the texture
            }
        }


    };
   
    // Functions for Widget
    void drawWidget(const Widget& widget) {
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(widget.x, widget.y, 0.0f));
        model = glm::scale(model, glm::vec3(widget.width, widget.height, 1.0f));
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        int useTextureUniform = glGetUniformLocation(shaderProgram, "useTexture");
        if (widget.texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, widget.texture);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
            glUniform1i(useTextureUniform, 1);
        }
        else {
            glUniform1i(useTextureUniform, 0);
            glUniform4f(glGetUniformLocation(shaderProgram, "fallbackColor"), 1.0f, 0.0f, 0.0f, 1.0f); // Example: Red color
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        for (const auto component : widget.components) {
            component->Draw();
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }



    void handleWidgetEvents(Widget& widget, SDL_Event* event) {
        for (auto component : widget.components) {
            component->handleEvents(event);
        }
        if (widget.draggableComponent) {
            handleDrag(*widget.draggableComponent, event);
        }
    }

    // Public API functions
    void createWidget(int id, int x, int y, int width, int height, int options, const std::string& texturePath) {
        if (uiManager.isCreatingWidget) {
            throw std::runtime_error("EndWidget must be called before calling a new widget");
            return;
       }
        uiManager.isCreatingWidget = true;
        auto widget = new Widget();
        widget->ID = id;
        widget->x = x;
        widget->y = y;
        widget->width = width;
        widget->height = height;
        widget->texture = 0;

        SDL_Surface* surface = IMG_Load(texturePath.c_str());
        if (surface) {
            glGenTextures(1, &widget->texture);
            glBindTexture(GL_TEXTURE_2D, widget->texture);

            GLenum format;
            if (surface->format->BytesPerPixel == 4) {
                format = GL_RGBA;
            }
            else {
                format = GL_RGB;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            SDL_FreeSurface(surface);
        }
        else {
            std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        }

        if (hasFlag(options, WIDGET_DRAGGABLE)) {
            widget->draggableComponent = new DraggableComponent();
            widget->draggableComponent->parent = widget;
        }

        uiManager.widgets.push_back(widget);
        uiManager.currentWidget = widget;
    }

   

    void beginWidget(int id) {
        for (auto widget : uiManager.widgets) {
            if (widget->ID == id) {
                uiManager.currentWidget = widget;
                return;
            }
        }
        std::cerr << "Widget with ID " << id << " not found" << std::endl;
    }

    void Text(const std::string& text, float fontSize) {
		if (!uiManager.currentWidget) {
			std::cerr << "No widget selected" << std::endl;
			return;
		}
		auto textComponent = new TextComponent(text, fontSize);
        textComponent->y = static_cast<float>(uiManager.currentWidget->y);
        textComponent->x = static_cast<float>(uiManager.currentWidget->x);
		textComponent->width = uiManager.currentWidget->width;
		textComponent->height = uiManager.currentWidget->height;
		textComponent->parent = uiManager.currentWidget;
		uiManager.currentWidget->components.push_back(textComponent);
	}

    void Button(const std::string& text, float fontSize, const std::string& texturePath, std::function<void()> onClick) {
		if (!uiManager.currentWidget) {
			std::cerr << "No widget selected" << std::endl;
			return;
		}
		auto buttonComponent = new ButtonComponent(text, fontSize, texturePath, onClick);
		buttonComponent->y = static_cast<float>(uiManager.currentWidget->y);
		buttonComponent->x = static_cast<float>(uiManager.currentWidget->x);
		buttonComponent->width = uiManager.currentWidget->width;
		buttonComponent->height = uiManager.currentWidget->height;
		buttonComponent->parent = uiManager.currentWidget;
		uiManager.currentWidget->components.push_back(buttonComponent);
	}
    void endWidget() {
        uiManager.currentWidget = nullptr;
        uiManager.isCreatingWidget = false;
    }

    void renderUI() {
        for (const auto widget : uiManager.widgets) {
            drawWidget(*widget);
        }
    }

    void handleEvents(SDL_Event* event) {
        for (auto widget : uiManager.widgets) {
            handleWidgetEvents(*widget, event);
        }
    }

    // Additional utility functions and widget operations can be defined here...

} // namespace SV_UI
