// CompileShaderBinary.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "src/opengl.h"

using namespace std;

//====COLOR Term======
const char RED[] = "\033[1;31m";
const char END[] = "\033[0m";
const char GREEN[] = "\033[1;32m";
const char BLUE[] = "\033[1;34m";
const char YELLOW[] = "\033[1;33m";
const char MAGENTA[] = "\033[1;35m";
//====================

//}}}

//{{{OpenGL Debug Message (after OpenGL 4.3)
//OpenGL Debug Level
enum OpenGL_Debug {
    OpenGL_Debug_NONE = 0,
    OpenGL_Debug_ERROR = 0x01,
    OpenGL_Debug_DEPRECATED = 0x01 << 1,
    OpenGL_Debug_UNDEFINED = 0x01 << 2,
    OpenGL_Debug_PORTABILITY = 0x01 << 3,
    OpenGL_Debug_PERFORMANCE = 0x01 << 4,
    OpenGL_Debug_MARKER = 0x01 << 5,
    OpenGL_Debug_PUSHGROUP = 0x01 << 6,
    OpenGL_Debug_POPGROUP = 0x01 << 7,
    OpenGL_Debug_OTHER = 0x01 << 8,
};
OpenGL_Debug OpenGL_Msg_Type = OpenGL_Debug_ERROR;

static void errorcallback_gl(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void const *userParam){
    OpenGL_Debug opengl_msg_type = *(OpenGL_Debug*)userParam;

    //{{{Output Type
    if(type == GL_DEBUG_TYPE_ERROR){
        if((opengl_msg_type & OpenGL_Debug_ERROR) == 0) return;
        cerr << RED << "Type : [ERROR] " << END;
    }
    else if(type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_DEPRECATED) == 0) return;
        cerr << YELLOW << "Type : [Deprecatred] " << END;
    }
    else if(type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_UNDEFINED) == 0) return;
        cerr << YELLOW << "Type : [Undefined] " << END;
    }
    else if(type == GL_DEBUG_TYPE_PORTABILITY){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_PORTABILITY) == 0) return;
        cerr << YELLOW << "Type : [Portability] " << END;
    }
    else if(type == GL_DEBUG_TYPE_PERFORMANCE){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_PERFORMANCE) == 0) return;
        cerr << YELLOW << "Type : [Performance] " << END;
    }
    else if(type == GL_DEBUG_TYPE_MARKER){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_MARKER) == 0) return;
        cerr << YELLOW << "Type : [Marker] " << END;
    }
    else if(type == GL_DEBUG_TYPE_PUSH_GROUP){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_PUSHGROUP) == 0) return;
        cerr << YELLOW << "Type : [Group Pushing] " << END;
    }
    else if(type == GL_DEBUG_TYPE_POP_GROUP){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_POPGROUP) == 0) return;
        cerr << YELLOW << "Type : [??] " << END;
    }
    else if(type == GL_DEBUG_TYPE_OTHER){
        if((opengl_msg_type & OpenGL_Debug::OpenGL_Debug_OTHER) == 0) return;
        cerr << YELLOW << "Type : [Other] " << END;
    }
    //}}}

    //{{{Output Source
    if(source == GL_DEBUG_SOURCE_API){
        cerr << GREEN << "Source : [OpenGL API] " << END;
    }
    else if(source == GL_DEBUG_SOURCE_WINDOW_SYSTEM){
        cerr << GREEN << "Source : [Window System API] " << END;
    }
    else if(source == GL_DEBUG_SOURCE_SHADER_COMPILER){
        cerr << GREEN << "Source : [Shader Compiler] " << END;
    }
    else if(source == GL_DEBUG_SOURCE_THIRD_PARTY){
        cerr << GREEN << "Source : [Third Party] " << END;
    }
    else if(source == GL_DEBUG_SOURCE_APPLICATION){
        cerr << GREEN << "Source : [User Application] " << END;
    }
    else if(source == GL_DEBUG_SOURCE_OTHER){
        cerr << GREEN << "Source : [Other] " << END;
    }
    //}}}

    //{{{Output Severity
    if(severity == GL_DEBUG_SEVERITY_HIGH){
        cerr << RED <<  "Severity : [High] " << END;
    }
    else if(severity == GL_DEBUG_SEVERITY_MEDIUM){
        cerr << YELLOW <<  "Severity : [Medium] " << END;
    }
    else if(severity == GL_DEBUG_SEVERITY_LOW){
        cerr << MAGENTA <<  "Severity : [Low] " << END;
    }
    else if(severity == GL_DEBUG_SEVERITY_NOTIFICATION){
        cerr << GREEN <<  "Severity : [Notification] " << END;
    }
    //}}}

    cerr << string(message) << endl;
}
//}}}
//}}}

//{{{void assertOpenGLError(const std::string &msg)
void assertOpenGLError(const std::string& msg) {
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        stringstream s;
        s << "OpenGL error 0x" << std::hex << error << " at " << msg;
        throw runtime_error(s.str());
    }
}
//}}}
std::string loadShaderAsString(const char* path) 
{
    std::ifstream f(path);
    std::string str;

    f.seekg(0, std::ios::end);   
    str.reserve(f.tellg());
    f.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());
    return str;
}
int load_shader(GLenum shader_type, const char* shader_path, GLuint& shaderid) 
{
    GLuint shader_id = glCreateShader( shader_type ); 
    if ( 0 == shader_id ) { 
        printf("Failed to create shader %s\n", shader_path);
        return 1;
    } 
    std::string shaderCode = loadShaderAsString(shader_path); 
    const GLchar * codeArray[] = { shaderCode.c_str() }; 
    glShaderSource( shader_id, 1, codeArray, NULL ); 
    glCompileShader( shader_id );
    GLint result; 
    glGetShaderiv( shader_id, GL_COMPILE_STATUS, &result ); 
    if( GL_FALSE == result ) { 
        printf("Failed to compile %s!\n", shader_path);
        // Get and print the info log
        GLint logLen; 
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLen); 
        if( logLen > 0 ) { 
            std::string log(logLen, ' '); 
            GLsizei written; 
            glGetShaderInfoLog(shader_id, logLen, &written, &log[0]); 
            printf("[ERROR] %s\n", log.c_str());
        } 
        return 1;
    } 
    shaderid = shader_id;
    return 0;
}
void precompile_shader(const char* vertex_shader, const char* fragment_shader, const char* output_path)
{
    GLuint vert_shader;
    GLuint frag_shader;
    GLuint programHandle;
    GLint status; 
    if (load_shader(GL_VERTEX_SHADER, vertex_shader, vert_shader))
        return;
    if (load_shader(GL_FRAGMENT_SHADER, fragment_shader, frag_shader))
        return;

    programHandle = glCreateProgram(); 
    if( 0 == programHandle ) 
    { 
        printf("Failed to create program.\n");
        return;
    } 
    glAttachShader( programHandle, vert_shader); 
    glAttachShader( programHandle, frag_shader); 
    glLinkProgram( programHandle );
    glGetProgramiv( programHandle, GL_LINK_STATUS, &status ); 
    if( GL_FALSE == status ) {
        std::cerr << "Failed to link shader program!" << std::endl;
        GLint logLen; 
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen); 
        if( logLen > 0 ) { 
            std::string log(logLen, ' ');
            GLsizei written;
            glGetProgramInfoLog(programHandle, logLen, &written, &log[0]); 
            std::cerr << "Program log: " << std::endl << log;
        } 
    }else {
        glUseProgram( programHandle );
    }

    GLint formats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
    if( formats < 1 ) {
        printf("Driver does not support any binary formats.\n");
        return;
    }
    // Get the binary length
    GLint length = 0;
    glGetProgramiv(programHandle, GL_PROGRAM_BINARY_LENGTH, &length);

    // Retrieve the binary code
    std::vector<GLubyte> buffer(length);
    GLenum format = 0;
    glGetProgramBinary(programHandle, length, NULL, &format, buffer.data());

    // Write the binary to a file.
    std::string fName(output_path);
    printf("Compiled %s & %s -> %s in format(%d)\n.", vertex_shader, fragment_shader, output_path, format);
    std::ofstream out(fName.c_str(), std::ios::binary);
    out.write( reinterpret_cast<char *>(buffer.data()), length );
    out.close();

    // Detach and delete shader objects
    glDetachShader(programHandle, vert_shader);
    glDetachShader(programHandle, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}
int main(int argc, char** argv)
{
    if (argc !=4) {
        printf("%s ${vertex_shader} ${fragment_shader} ${Binary_Shader_File_Output}", argv[0]);
        return EXIT_FAILURE;   
    }
    const char* vertex_shader_src = argv[1];
    const char* fragment_shader_src = argv[2];
    const char* output_binary = argv[3];

    if (glfwInit() != GLFW_TRUE) {
        printf("failed to init glfw.\n");
        exit(EXIT_FAILURE);
    }
    printf("glfw init.\n");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "CompileShader", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() < 0) {
        printf("failed to init glew.\n");
        exit(EXIT_FAILURE);
    }
    printf("glew init.\n");
   
    precompile_shader(vertex_shader_src, fragment_shader_src, output_binary);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
