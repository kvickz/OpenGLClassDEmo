#include <SDL.h>
#include <gl/glew.h>  // Needed before SDL_OpenGL.h!
#include <SDL_OpenGL.h>

void CheckGLError()
{
	GLenum error = glGetError();
	if (error != 0)
	{
		SDL_Log("GL Error: %d %s", error, glewGetErrorString(error));
		__debugbreak();
	}
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	// OpenGL 4.0!
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("OpenGL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Glew tends to generate GL_INVALID_ENUM (https://www.opengl.org/wiki/OpenGL_Loading_Library)

	const float triangle[] =
	{
		// X    Y    Z
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
	};

	GLuint triangleBuffer;
	glCreateBuffers(1, &triangleBuffer);
	 
	glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
	 
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	 
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	const int indices[] =
	{
		0, 1, 2
	};
	GLuint triangleIndexBuffer;
	glCreateBuffers(1, &triangleIndexBuffer);
	 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
	 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	 

	const char* vertexShaderCode[] =
	{
		"#version 400\n", // This is the only line that requires a newline, all others do not need it!
		"in vec3 vertex;",
		"void main() {",
		"  gl_Position = vec4(vertex, 1.0);",
		"}",
	};

    const char* fragmentShaderCode[] =
    {
        "#version 400\n", // This is the only line that requires a newline, all others do not need it!
        "out vec4 colorRGBA;",
        "void main() {",
        "  colorRGBA = vec4(1.0, 0.0, 0.0, 1.0);",
        "}",
    };

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, _ARRAYSIZE(vertexShaderCode), vertexShaderCode, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, _ARRAYSIZE(fragmentShaderCode), fragmentShaderCode, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	 

	GLuint vertArray;
	glGenVertexArrays(1, &vertArray);
	glBindVertexArray(vertArray);
	glEnableVertexAttribArray(0);
	 
	glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	 

	bool done = false;
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE || event.key.keysym.sym == SDLK_ESCAPE)
			{
				done = true;
				break;
			}
		}

		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		 
		glBindVertexArray(vertArray);
		 
        //glDrawArrays(GL_TRIANGLES, 0, _ARRAYSIZE(indices));
		glDrawElements(GL_TRIANGLES, _ARRAYSIZE(indices), GL_UNSIGNED_INT, 0);

		 

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}