#include <iostream>
#include "../es/esUtil.h"

typedef struct
{
    GLuint programObject;
} UserData;

GLuint LoadShader(GLenum type, const char* shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    glShaderSource(shader, 1, &shaderSrc, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            esLogMessage("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

int Init(ESContext* esContext)
{
    UserData* userData = (UserData*)esContext->userData;
    GLbyte vShaderStr[] =
        "attribute vec4 vPosition;    \n"
        "void main() {                \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";

    GLbyte fShaderStr[] =
        "precision mediump float;\n"\
        "void main() {                                \n"
        "  gl_FragColor = vec4 ( 1.0, 1.0, 0.0, 1.0 );\n"
        "}                                            \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, (const char*)vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*)fShaderStr);

    programObject = glCreateProgram();

    if (programObject == 0)
        return 0;

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glBindAttribLocation(programObject, 0, "vPosition");

    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            esLogMessage("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(programObject);
        return FALSE;
    }

    userData->programObject = programObject;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return TRUE;
}

void Draw(ESContext* esContext)
{
    UserData* userData = (UserData*)esContext->userData;
    GLfloat vVertices[] = {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    glViewport(0, 0, esContext->width, esContext->height);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(userData->programObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}

#if _DEBUG
int main(int argc, char* argv[])
#else
int WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
#endif
{
    ESContext esContext;
    UserData  userData;

    esInitContext(&esContext);
    esContext.userData = &userData;

    esCreateWindow(&esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB);

    if (!Init(&esContext))
        return 0;

    esRegisterDrawFunc(&esContext, Draw);

    esMainLoop(&esContext);
}
