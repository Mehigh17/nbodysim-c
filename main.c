#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

// this function will be called internally by GLFW whenever an error occur.
void error_callback(int error, const char* description);

int main() {
    // tell GLFW to call error_callback if an internal error ever occur at some point inside GLFW functions.
    glfwSetErrorCallback(error_callback);

    // initialize all the internal state of GLFW
    if (!glfwInit()) {
        return -1;
    }

    // create the window
    int resx = 640, resy = 480;
    GLFWwindow* window = glfwCreateWindow(resx, resy, "GLFW: Creating a window.", NULL, NULL);

    // check if the opening of the window failed whatever reason and clean up
    if (!window) { 
        glfwTerminate();
        return -2;
    }

    // in principle we can have multiple windows, 
    // so we set the newly created on as "current"
    glfwMakeContextCurrent(window);

    // Enable v-sync for now, if possible
    glfwSwapInterval(1);

    // load opengl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Could not load OpenGL functions");
        return -3;
    }

	
	glClearColor(1, 0, 1, 1);
    // main loop
    while (!glfwWindowShouldClose(window)) {
        // listen for events (keyboard, mouse, etc.). ignored for now, but useful later
        glfwPollEvents();

        // make it close on pressing escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);
		
		glClear(GL_COLOR_BUFFER_BIT);
		
        // swap buffers (replace the old image with a new one)
        // this won't have any visible effect until we add actual drawing
        glfwSwapBuffers(window);
    }

    // clean up
    glfwTerminate();
    
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}
