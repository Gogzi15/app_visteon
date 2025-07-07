#include <iostream>
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

#include "tiny_gltf.h"

struct WindowGLContext 
{
    GLuint vertexArrayObject;
    GLint program;
};

struct WindowContext 
{
    WindowGLContext gl;
};


GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, buffer);

        std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                  << " shader compilation failed:\n" << buffer << std::endl;

        delete[] buffer;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetProgramInfoLog(program, length, nullptr, buffer);

        std::cerr << "Program linking failed:\n" << buffer << std::endl;

        delete[] buffer;
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

void loadShaders(WindowContext& windowContext) {
    const char* vertexShaderSource = R"(
        #version 300 es
        layout (location = 0) in vec3 position; 
        void main() {
            gl_Position = vec4(position, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 300 es
        precision mediump float;
        out vec4 fragcolour;
        void main() {
            fragcolour = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )";

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (!vertexShader) return;

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return;
    }

    windowContext.gl.program = linkProgram(vertexShader, fragmentShader);
    if (!windowContext.gl.program) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glUseProgram(windowContext.gl.program);
}


int main(void)
{

    WindowContext windowContext;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //nastroiki za opengl
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);    //izpolzvame opengl es
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //za versii
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);    //za da izpolzva egl

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current ================ v build papkata trqbvashe da slojim .dll failovete za da moje build-a da gi vidi kogato loadva .lib failovete*/
    glfwMakeContextCurrent(window); //za da se raboti na tozi prozorec

    std::string gltfFilename = "../example/gltf/01_triangle/export/triangle.gltf";

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if(!loader.LoadASCIIFromFile(&model, &err, &warn, gltfFilename)){
        std::cerr << "Failed to load gltf file" << gltfFilename << "/n";
        std::cerr << "Error: " << err << std::endl;
        std::cerr << "Warning: " << warn << std::endl;
        return 1;
    }



    // float vertex[6] = 
    // {
    //     0.0, 0.5,
    //     -0.5, 0.0,
    //     0.5, 0.0
    // };

    //============== S EDIN MASIV ==============
    // GLfloat vertexData[] = {
    // -0.5f, -0.5f, 0.0f, // positions[0]​
    // 0.5f, -0.5f, 0.0f, // positions[1]​
    // 0.0f, 0.5f, 0.0f, // positions[2]​
    // 0.0f, 0.0f, 1.0f, // normals[0]​
    // 0.0f, 0.0f, 1.0f, // normals[1]​
    // 0.0f, 0.0f, 1.0f, // normals[2]​
    // 0.25f, 0.25f, // texture coordinates[0]​
    // 0.75f, 0.25f, // texture coordinates[1]​
    // 0.5f, 0.75f // texture coordinates[2]​
    // };

    //============== S TRI MASIVA I TRI BUFERA
    GLfloat postitionData[] = {
        -0.5f, -0.5f, 0.0f, // positions[0]​
      0.5f, -0.5f, 0.0f, // positions[1]​
        0.0f, 0.5f, 0.0f, // positions[2]​
    };

    GLfloat normalData[] = {
        0.0f, 0.0f, 1.0f, // normals[0]​
        0.0f, 0.0f, 1.0f, // normals[1]​
        0.0f, 0.0f, 1.0f, // normals[2]​
    };

    GLfloat textData[] = {
        0.25f, 0.25f, // texture coordinates[0]​
        0.75f, 0.25f, // texture coordinates[1]​
        0.5f, 0.75f // texture coordinates[2]​
    };

    // ============= AKO SHTE CHETEM OT GLTF =====================
    // uint32_t gltfPostitionIndex = model.meshes[0].primitives[0].attributes["POSITION"];
    // uint32_t gltfBufferIndex = model.bufferViews[gltfPostitionIndex].buffer;
    // unsigned char* gltfBufferData = model.buffers[gltfBufferIndex].data.data();

    // float* verticies = (float*)gltfBufferData;

    // for(int i = 0; i < 9; i++){
    //     std::cout << verticies[i] << std::endl;
    // }


    // ============== SUZDAVANE NA TRI BUFERA ===================
    unsigned int bufferID = 0;
    unsigned int buffer2ID = 0;
    unsigned int buffer3ID = 0;
    glGenBuffers(1, &bufferID); //suzdavame bufer v GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);    //definirame tipa na bufera
    glBufferData(GL_ARRAY_BUFFER, sizeof(postitionData), postitionData, GL_STATIC_DRAW);

    glGenBuffers(1, &buffer2ID); //suzdavame bufer v GPU; kolko bufera, promenliva za da se populni id-to
    glBindBuffer(GL_ARRAY_BUFFER, buffer2ID);    //definirame tipa na bufera
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalData), normalData, GL_STATIC_DRAW);
    
    glGenBuffers(1, &buffer3ID); //suzdavame bufer v GPU
    glBindBuffer(GL_ARRAY_BUFFER, buffer3ID);    //definirame tipa na bufera
    glBufferData(GL_ARRAY_BUFFER, sizeof(textData), textData, GL_STATIC_DRAW);

    const int POSITION_INDEX = 0;
    const int NORMAL_INDEX = 1;
    const int TEXT_INDEX = 2;  
    


    // ======== NALGASA NA NASTROIKI S EDIN MASIV VARIANT 1 =========
    // glEnableVertexAttribArray(POSITION_INDEX); //enable attribute 0
    // //attribute zero[postitions], contains of three arguments/ aka shte chete tri floata na skok, the type is glfloat, misc, 
    // //offset to the next element (8 * glfloatsize) aka the next xyz's demek kolko trqbva da preskochi do sledvashtite xyz,
    // //where to start from
    // glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat))); 

    // glEnableVertexAttribArray(NORMAL_INDEX);
    // glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // glEnableVertexAttribArray(TEXT_INDEX);
    // glVertexAttribPointer(TEXT_INDEX, 2, GL_FLOAT, GL_FALSE,  8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    // //shte preskacha dokato ne stigne buffer overflow

    // ======== NALGASA NA NASTROIKI S EDIN MASIV VARIANT 2 =========
    // //kategorizirame veche postavenata informaciq v buferite
    // glEnableVertexAttribArray(POSITION_INDEX);
    // //ako slojim 0 na stride-a pak shte stane zashtoto sa edno sled drugo i opengl sam shte se opravi
    // glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat))); 
    
    // glEnableVertexAttribArray(NORMAL_INDEX);
    // glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));
    
    // glEnableVertexAttribArray(TEXT_INDEX);  //"zadai nov atribut na index 2"
    // glVertexAttribPointer(TEXT_INDEX, 2, GL_FLOAT, GL_FALSE,  2 * sizeof(GLfloat), (void*)(18 * sizeof(GLfloat)));


    // ========= IZPOLZVANE NA VERTEX ARRAY OBJECT ==========
    GLuint vertexArrayObject1 = 0;
    glGenVertexArrays(1, &vertexArrayObject1);
    glBindVertexArray(vertexArrayObject1);

    glBindBuffer(GL_ARRAY_BUFFER, bufferID); 
    glEnableVertexAttribArray(POSITION_INDEX);
    glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer2ID);
    glEnableVertexAttribArray(NORMAL_INDEX);
    glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer3ID);
    glEnableVertexAttribArray(TEXT_INDEX);  
    glVertexAttribPointer(TEXT_INDEX, 2, GL_FLOAT, GL_FALSE,  0, 0);

    GLuint vertexArrayObject2 = 0;
    glGenVertexArrays(1, &vertexArrayObject2);
    glBindVertexArray(vertexArrayObject2);

    glBindBuffer(GL_ARRAY_BUFFER, bufferID);    
    glEnableVertexAttribArray(TEXT_INDEX);
    glVertexAttribPointer(TEXT_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer2ID);    
    glEnableVertexAttribArray(NORMAL_INDEX);
    glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer3ID);    
    glEnableVertexAttribArray(POSITION_INDEX);  
    glVertexAttribPointer(POSITION_INDEX, 2, GL_FLOAT, GL_FALSE,  0, 0);

    loadShaders(windowContext);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(1.0F, 0.54F, 0.54F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        //glBegin(GL_TRIANGLES);
        // glVertex2d(0.5f, 0.0f);
        // glVertex2d(0.0f, 0.5f);
        // glVertex2d(0.0f, -0.5f);
        glBindVertexArray(vertexArrayObject1);
        glUseProgram(windowContext.gl.program);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(vertexArrayObject2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
} 