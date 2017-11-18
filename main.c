#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

// Function Declaration
char* read_file(char* file_name);
void error_callback(int error, const char* description);
// --------------------

// Variable Declaration

// -------------------- 

int main()
{
	glfwSetErrorCallback(error_callback);

	int result = glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if(result == GLFW_FALSE)
	{
		printf("GLFW did not initialize correctly!\n");
		return -1;
	}

	int width = 500, height = 500;
	GLFWwindow* window = glfwCreateWindow(width, height, "N-Body Simulation", NULL, NULL);

	if(window == NULL)
	{
		glfwTerminate();
		printf("The window did not initialize correctly!\n");
		return -1;
	}

	char* vertex_source = read_file("vertex_shader.glsl");
	char* fragment_source = read_file("fragment_shader.glsl");

//	printf("Vertex source: %s\n", vertex_source);
//	printf("Fragment source: %s\n", fragment_source);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable V-Sync
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Load GLAD (library loader)

	// Loading & Compiling shaders
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const char **)&vertex_source, NULL);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const char **)&fragment_source, NULL); // why cast to (const char **) ????
	glCompileShader(fragment_shader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	//  

	while(!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

char* read_file(char* file_name)
{
	FILE* file = fopen(file_name, "r");

	if(file == NULL)
	{
		//printf("Couldn't open the file %s.\n", *file_name);
		return NULL;
	}

	while(1)
	{
		char  c = fgetc(file);
		if(feof(file))
			break;
	}

	long size = ftell(file);

	char* source = (char*) malloc(size);

	rewind(file); // file stream goes back to first position

	for(int i = 0; i < size; i++)
	{
		*(source + i) = fgetc(file);
	}

	//printf("File %s has a size of %d bytes.\n", file_name, size);

	fclose(file);

	return source;
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}
