#pragma once

//////////////////////////////////////////////////////////
////////////SV UI UTILITIES MUST BE INCLUDED IN SV_UI3.0//
//////////////////////////////////////////////////////////
#define APIENTRY GLAPIENTRY
#include <GL/glew.h>
#include <iostream>


namespace SV_UI {

    //////////////////////////////////////////////////
    ////////////OPENGL DEBUG STUFF////////////////////
    ///////////////////////////////////////////////////

    // Callback function for debugging
    void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {

        // Ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cerr << "---------------" << std::endl;
        std::cerr << "Debug message (" << id << "): " << message << std::endl;

        switch (source) {
        case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
        } std::cerr << std::endl;

        switch (type) {
        case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
        } std::cerr << std::endl;

        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;
        } std::cerr << std::endl;
        std::cerr << std::endl;
    }

    // Function to initialize OpenGL debugging
    void initOpenGLDebug() {
        if (glewIsSupported("GL_KHR_debug")) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(static_cast<GLDEBUGPROC>(glDebugOutput), nullptr); // Cast to GLDEBUGPROC
            // Other setup remains the same...
        }
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

    void calculatePositionForAlignment(float& x, float& y, int compWidth, int compHeight, int parentWidth, int parentHeight, Alignment alignment) {
        switch (alignment) {
        case Alignment::TopLeft:
            // x and y remain unchanged
            break;
        case Alignment::TopCenter:
            x = (parentWidth - compWidth) / 2;
            break;
        case Alignment::TopRight:
            x = parentWidth - compWidth;
            break;
        case Alignment::CenterLeft:
            y = (parentHeight - compHeight) / 2;
            break;
        case Alignment::Center:
            x = (parentWidth - compWidth) / 2;
            y = (parentHeight - compHeight) / 2;
            break;
        case Alignment::CenterRight:
            x = parentWidth - compWidth;
            y = (parentHeight - compHeight) / 2;
            break;
        case Alignment::BottomLeft:
            y = parentHeight - compHeight;
            break;
        case Alignment::BottomCenter:
            x = (parentWidth - compWidth) / 2;
            y = parentHeight - compHeight;
            break;
        case Alignment::BottomRight:
            x = parentWidth - compWidth;
            y = parentHeight - compHeight;
            break;
        }
    }
}
