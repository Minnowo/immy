
// If we have X11 available
// This contains code which uses X11 libs.
//
// We cannot import raylib here, since it has Font 
// which conflicts with the X11 font.
#ifdef X11_AVAILABLE

// we have to do this here without raylib,
// since Font part of X11 has a name conflict
#define GLFW_EXPOSE_NATIVE_X11

#include "external/glfw/include/GLFW/glfw3.h"
#include "external/glfw/include/GLFW/glfw3native.h"

#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdbool.h>

int x11GetWindowHandle(GLFWwindow* handle) {

    return glfwGetX11Window(handle);
}

int x11GrabKeyboard(GLFWwindow* handle) {

    Window   window  = glfwGetX11Window(handle);
    Display* display = glfwGetX11Display();

    return XGrabKeyboard(
        display, window, True, GrabModeAsync, GrabModeAsync, CurrentTime
    );
}

int x11UngrabKeyboard() {

    Display* display = glfwGetX11Display();

    return XUngrabKeyboard(display, CurrentTime);
}

void x11SetOverrideRedirect(GLFWwindow* handle) {

    Window   window  = glfwGetX11Window(handle);
    Display* display = glfwGetX11Display();

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;

    int r = XChangeWindowAttributes(
        display, window, CWOverrideRedirect, &attributes
    );

    XUnmapWindow(display, window);
    XMapWindow(display, window);

    // XRaiseWindow(display, window);

    // XFlush(display);
}

#endif
