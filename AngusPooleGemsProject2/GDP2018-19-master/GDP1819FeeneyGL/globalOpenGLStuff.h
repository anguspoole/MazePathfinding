#ifndef _globalOpenGLStuff_HG_
#define _globalOpenGLStuff_HG_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cFBO.h"

#include "Error/COpenGLError.h"
#include "cJoystickManager.h"

// GFLW keyboard callback
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods);
void ProcessAsyncKeys(GLFWwindow* window);
void ProcessAsyncJoysticks(GLFWwindow* window, cJoystickManager* pJoysticks);
// 
void z_ProcessAsyncJoysticksPS4(GLFWwindow* window, cJoystickManager* pJoysticks);
void z_ProcessAsyncJoysticksXboxOne(GLFWwindow* window, cJoystickManager* pJoysticks);

void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
void ProcessAsyncMouse(GLFWwindow* window);

void cursor_enter_callback(GLFWwindow* window, int entered);
extern bool g_MouseIsInsideWindow;	// 

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// This is to change the full screen FBO objects when the window changes size
// See: http://www.glfw.org/docs/latest/window_guide.html#window_size
void window_size_callback(GLFWwindow* window, int width, int height);

void joystick_callback(int joy, int event);

#endif

