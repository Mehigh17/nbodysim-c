#include <stdio.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define PI 3.1415

int n = 60;
float accl[] = { 0.0f, 0.0f };
float pos[] = { 0.0f, 0.0f };

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
    int resx = 512, resy = 512;
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

	double time = 0.0f;
	float delta_t = 1.0f/1000.0f;
	glClearColor(1, 0, 1, 1);
    // main loop
    while (!glfwWindowShouldClose(window)) {
        // listen for events (keyboard, mouse, etc.). ignored for now, but useful later
        glfwPollEvents();

        // make it close on pressing escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);

		glClear(GL_COLOR_BUFFER_BIT);

		time = glfwGetTime();
		
		if(glfwGetKey(window, GLFW_KEY_I))
				n++;

		if(glfwGetKey(window, GLFW_KEY_U))
				n--;

		// ground collision
		if(pos[1] > -0.9)
		{
			accl[1] -= 0.1 * delta_t;
		}
		else
		{
			accl[1] = 0.0f * delta_t;
			//accl[1] -= abs(accl[1]);
		}

		// Impulsion
		if(glfwGetKey(window, GLFW_KEY_SPACE))
				accl[1] += 0.5f * delta_t;

		pos[0] += accl[0];
		pos[1] += accl[1];

		glBegin(GL_POLYGON);
		for(int i = 0; i < n; i++)
		{
				glVertex2f(cos(2*i*PI/n) * 0.1f + pos[0], sin(2*i*PI/n) * 0.1f + pos[1]);
		}
		glEnd();

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
