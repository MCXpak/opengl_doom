#ifndef CONTROLS
#define CONTROLS

#include "Camera_c.h"

class Controls {
public:

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos, bool firstMouse, float lastX, float lastY, Camera camera)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }

};

#endif
