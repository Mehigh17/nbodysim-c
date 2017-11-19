#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

// Function Declaration
char* read_file(char* file_name);
GLuint get_shader(char* file_name, GLenum type);
void error_callback(int error, const char* description);
void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void compute_movement(); // bs method - temeporary
// --------------------

// Variable Declaration
GLfloat triangle[3][2] = {
		{-0.5, -0.5},
		{0.0, 0.5},
		{0.5, -0.5}};

GLfloat colors[3][3] = {
		{1.0, 1.0, 1.0},
		{0.5, 0.5, 0.5},
		{1.0, 1.0, 1.0}};

GLuint vao, vbo[2];
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

	glfwSetKeyCallback(window, window_key_callback);

	char* vertex_source = read_file("vertex_shader.glsl");
	char* fragment_source = read_file("fragment_shader.glsl");

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable V-Sync
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Load GLAD (library loader)

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);

	// Coordinates VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), triangle, GL_STATIC_DRAW); // Will have to use GL_DYANMIC_DRAW for moving shapes!
	glVertexAttribPointer(0 /*vbo[0]*/, 2 /*vec2*/, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0); // Tell the GPU it can use vbo[0]
	// ^ Finished transferring the vertices coordinates to the GPU memory

	// Colors VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1 /*vbo[1]*/, 3 /*vec3*/, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1); // Tell the GPU it can use vbo[1]
	// ^ Finished transferring the colors coordinates to the GPU memory

	// Loading & Compiling shaders (! Only the shader that comes first will load for some reason)
	GLuint fragment_shader = get_shader(fragment_source, GL_FRAGMENT_SHADER);
	GLuint vertex_shader = get_shader(vertex_source, GL_VERTEX_SHADER); 	

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	// 

	glBindAttribLocation(program, 0, "in_Position");
	glBindAttribLocation(program, 1, "in_Color");

	glLinkProgram(program);
	glUseProgram(program);

	while(!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		compute_movement();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}

void compute_movement()
{
	GLfloat shape[3][2];
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			shape[i][j] = triangle[i][j];

			if(j == 0)
				shape[i][j] += cos(glfwGetTime()) / 2.0;

			if(j == 1)
				shape[i][j] += sin(glfwGetTime()) / 2.0;
		}
	}

	// Update the coordinates vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), shape, GL_DYNAMIC_DRAW); 
}

GLuint get_shader(char* source_code, GLenum type)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, (const char**)&source_code, NULL);
	glCompileShader(shader);

	printf("Shader source code:\n%s", source_code);

	int is_compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

	if(is_compiled == GL_FALSE)
	{
		int max_len = 1000;
		int log_len;
		char* log = malloc(max_len); // max_len bytes

		glGetShaderInfoLog(shader, max_len, &log_len, log);

		free(log);
		return -1;
	}
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

void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}
