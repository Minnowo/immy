
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
#include <X11/Xatom.h>

#include <string.h>
#include <stdbool.h>

int xGetWindowHandle(GLFWwindow* handle) {
    return glfwGetX11Window(handle);
}

int xGrabKeyboard(GLFWwindow* handle) {

    Window   window  = glfwGetX11Window(handle);
    Display* display = glfwGetX11Display();

    int r = XGrabKeyboard(display, window, false, GrabModeAsync, GrabModeAsync, CurrentTime);

    return  r;
}

int xUngrabKeyboard() {

    Display* display = glfwGetX11Display();

    return XUngrabKeyboard(display, CurrentTime);
}

void xSetOverrideRedirect(GLFWwindow* handle) {

    Window   window  = glfwGetX11Window(handle);
    Display* display = glfwGetX11Display();

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;

    int r = XChangeWindowAttributes(
        display, window, CWOverrideRedirect, &attributes
    );

    // makes window gone forever???
    // XUnmapWindow(display, window);
    // XMapWindow(display, window);
   
    glfwHideWindow(handle);
    glfwShowWindow(handle);
}

void xToggleFullscreen(GLFWwindow* handle) {

    Window   window  = glfwGetX11Window(handle);
    Display* display = glfwGetX11Display();

    Atom wm_state      = XInternAtom(display, "_NET_WM_STATE", true);
    Atom wm_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);

    XEvent               ev;
    XClientMessageEvent* cm;

    memset(&ev, 0, sizeof(ev));
    ev.type = ClientMessage;

    cm               = &ev.xclient;
    cm->window       = window;
    cm->message_type = wm_state;
    cm->format       = 32;
    cm->data.l[0]    = 2; // toggle
    // cm->data.l[0]    = 1; // yes
    cm->data.l[1]    = wm_fullscreen;

    XSendEvent(
        display, DefaultRootWindow(display), False, SubstructureNotifyMask | SubstructureRedirectMask, &ev
    );
}


#endif
