#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

float x = 0.0f;
float y = 0.0f;
float red = 1.0f;
float inc = 0.05f;

class IndexBuffer
{
private:
    unsigned int m_RendererID; // index buffer id
    unsigned int m_Count;      // number of indices

public:
    IndexBuffer(const unsigned int *data, unsigned int count)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    }
    ~IndexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    };
    void Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    inline unsigned int GetCount() const { return m_Count; }
};

class VertexBuffer
{
private:
    unsigned int m_RendererID; // vertex buffer id

public:
    VertexBuffer(const void *data, unsigned int size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
    ~VertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    };
    void Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };
};

std::string readFile(const char *path)
{
    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

unsigned int loadShader(const char *vPath, const char *fPath)
{
    unsigned int program = glCreateProgram();
    std::string vString = readFile(vPath);
    const char *vShader = vString.c_str();
    std::string fString = readFile(fPath);
    const char *fShader = fString.c_str();

    unsigned int vId = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fId = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vId, 1, &vShader, nullptr);
    glShaderSource(fId, 1, &fShader, nullptr);
    glAttachShader(program, vId);
    glAttachShader(program, fId);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vId);
    glDeleteShader(fId);
    int vRes;
    int fRes;
    glGetShaderiv(vId, GL_COMPILE_STATUS, &vRes);
    glGetShaderiv(fId, GL_COMPILE_STATUS, &fRes);
    if (!vRes)
    {
        int l;
        glGetShaderiv(vId, GL_INFO_LOG_LENGTH, &l);
        char *message = (char *)alloca(l * sizeof(char));
        glGetShaderInfoLog(vId, l, &l, message);
        std::cout << "failed to compile vertex shader\n"
                  << message << "\n";
    }
    if (!fRes)
    {
        int l;
        glGetShaderiv(fId, GL_INFO_LOG_LENGTH, &l);
        char *message = (char *)alloca(l * sizeof(char));
        glGetShaderInfoLog(fId, l, &l, message);
        std::cout << "failed to compile fragment shader\n"
                  << message << "\n";
    }
    glUseProgram(program);
    return program;
}

void keypressHandler(GLFWwindow *w, int k, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        if (scancode == glfwGetKeyScancode(GLFW_KEY_UP))
        {
            y += 0.05f;
        }
        else if (scancode == glfwGetKeyScancode(GLFW_KEY_DOWN))
        {
            y -= 0.05f;
        }
        else if (scancode == glfwGetKeyScancode(GLFW_KEY_RIGHT))
        {
            x += 0.05f;
        }
        else if (scancode == glfwGetKeyScancode(GLFW_KEY_LEFT))
        {
            x -= 0.05f;
        }
        else
        {
            std::exit(0);
        }
    }
}

void errorHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << "\n"
              << message << "\n";
}

int main(void)
{
    if (!glfwInit())
    {
        return -1;
    }
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    GLFWwindow *window;
    int monitorsCount;
    GLFWmonitor **monitors = glfwGetMonitors(&monitorsCount);
    window = glfwCreateWindow(1920, 1080, "OpenGL test app", monitors[0], NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error: failed to init glew\n";
        return -1;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, keypressHandler);
    glDebugMessageCallback(errorHandler, nullptr);

    float positions[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f};

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};

    VertexBuffer vb(positions, sizeof(positions));
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);
    IndexBuffer ib(indices, 6);

    unsigned int program = loadShader("C:/dev/ogl/src/res/shaders/vertex", "C:/dev/ogl/src/res/shaders/fragment");

    int u_Color = glGetUniformLocation(program, "u_Color");
    glUniform4f(u_Color, 1.0f, 0.0f, 0.0f, 1.0f);
    int u_Offset = glGetUniformLocation(program, "u_Offset");
    glUniform2f(u_Offset, x, y);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (red > 1.0f)
            inc = -0.05f;
        else if (red < 0.0f)
            inc = 0.05f;
        red += inc;
        glUniform4f(u_Color, red, 0.0f, 0.0f, 1.0f);
        glUniform2f(u_Offset, x, y);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}