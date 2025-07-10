#include <iostream>
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include "tiny_gltf.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <string>
#include "basic_types.hpp"

// Structure to store OpenGL-related objects and state for the window
struct WindowGLContext 
{
    std::unordered_map<std::string, float> materialUniformFloats;
    std::unordered_map<std::string, int> materialUniformInts; 
    std::unordered_map<std::string, Vector3> materialUniformVector3;    
    std::unordered_map<std::string, Vector2> materialUniformVector2;
    std::unordered_map<std::string, Vector4> materialUniformVector4;

    GLuint indicesCount;           // Number of indices to render
    GLuint indexBuffer;            // OpenGL buffer object for indices
    GLuint vertexArrayObject;      // Vertex Array Object (VAO) handle
    GLint program;                 // Shader program handle

};

// Structure to store the window context, including OpenGL context
struct WindowContext 
{
    WindowGLContext gl;            // OpenGL context for this window
};

// Function to compile a shader (vertex or fragment) from source code
GLuint compileShader(GLenum type, const char* source) {
    // Create a new shader object of the given type
    GLuint shader = glCreateShader(type);
    // Attach the shader source code to the shader object
    glShaderSource(shader, 1, &source, nullptr);
    // Compile the shader source code
    glCompileShader(shader);

    // Check if the shader compiled successfully
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        // If compilation failed, get the error log
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, buffer);

        // Print the error log to standard error
        std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                  << " shader compilation failed:\n" << buffer << std::endl;

        // Clean up and delete the shader object
        delete[] buffer;
        glDeleteShader(shader);
        return 0;
    }

    // Return the compiled shader object handle
    return shader;
}

// Function to link a vertex and fragment shader into a shader program
GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    // Create a new program object
    GLuint program = glCreateProgram();
    // Attach the compiled vertex shader to the program
    glAttachShader(program, vertexShader);
    // Attach the compiled fragment shader to the program
    glAttachShader(program, fragmentShader);
    // Link the attached shaders into a complete program
    glLinkProgram(program);

    // Check if the program linked successfully
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        // If linking failed, get the error log
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetProgramInfoLog(program, length, nullptr, buffer);

        // Print the error log to standard error
        std::cerr << "Program linking failed:\n" << buffer << std::endl;

        // Clean up and delete the program object
        delete[] buffer;
        glDeleteProgram(program);
        return 0;
    }

    // Return the linked program object handle
    return program;
}

static float getcurrentTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
    return duration / 1000.0f; // Convert milliseconds to seconds
}

static void materialSetProperty(WindowGLContext& windowContext, std::string uniformName, int value)
{
    if (windowContext.materialUniformInts.find(uniformName) != windowContext.materialUniformInts.end())
    {
        windowContext.materialUniformInts[uniformName] = value;
    }
}

static void materialSetProperty(WindowGLContext& windowContext, std::string uniformName, Vector2 value)
{
    // Check if the uniform name exists in the materialUniformVector2 map
    // If it does, update the value; otherwise, add a new entry
    if (windowContext.materialUniformVector2.find(uniformName) != windowContext.materialUniformVector2.end())
    {
        windowContext.materialUniformVector2[uniformName] = value;
    }
}
static void materialSetProperty(WindowGLContext& windowContext, std::string uniformName, Vector3 value)
{
    // Check if the uniform name exists in the materialUniformVector3 map
    // If it does, update the value; otherwise, add a new entry
    if (windowContext.materialUniformVector3.find(uniformName) != windowContext.materialUniformVector3.end())
    {
        windowContext.materialUniformVector3[uniformName] = value;
    }
}

static void materialSetProperty(WindowGLContext& windowContext, std::string uniformName, float value)
{
    // Check if the uniform name exists in the materialUniformFloats map
    // If it does, update the value; otherwise, add a new entry
    if (windowContext.materialUniformFloats.find(uniformName) != windowContext.materialUniformFloats.end())
    {
        windowContext.materialUniformFloats[uniformName] = value;
    }
}

static void materialSetProperty(WindowGLContext& windowContext, std::string uniformName, Vector4 value)
{
    // Check if the uniform name exists in the materialUniformVector4 map
    // If it does, update the value; otherwise, add a new entry
    if (windowContext.materialUniformVector4.find(uniformName) != windowContext.materialUniformVector4.end())
    {
        windowContext.materialUniformVector4[uniformName] = value;
    }
}

static void materialUpdateProperties(WindowGLContext& windowContext)
{
    for(auto& uniform : windowContext.materialUniformInts)
    {
        GLint location = glGetUniformLocation(windowContext.program, uniform.first.c_str());
        if (location != -1)
        {
            glUniform1i(location, uniform.second);
        }
        std::cout << "Uniform: " << uniform.first << " = " << uniform.second << std::endl;
    }
    // Iterate through the material uniform floats and set them in the shader program
    for (auto& uniform : windowContext.materialUniformFloats)
    {
        GLint location = glGetUniformLocation(windowContext.program, uniform.first.c_str());
        if (location != -1)
        {
            glUniform1f(location, uniform.second);
        }
        std::cout << "Uniform: " << uniform.first << " = " << uniform.second << std::endl;
    }
    for (auto& uniform : windowContext.materialUniformVector4)
    {
        GLint location = glGetUniformLocation(windowContext.program, uniform.first.c_str());
        if (location != -1)
        {
            glUniform4f(location, uniform.second.x, uniform.second.y, uniform.second.z, uniform.second.w);
        }
        std::cout << "Uniform: " << uniform.first << " = (" 
                  << uniform.second.x << ", " 
                  << uniform.second.y << ", " 
                  << uniform.second.z << ", " 
                  << uniform.second.w << ")" << std::endl;
    }
    for (auto& uniform : windowContext.materialUniformVector3)
    {
        GLint location = glGetUniformLocation(windowContext.program, uniform.first.c_str());
        if (location != -1)
        {
            glUniform3f(location, uniform.second.x, uniform.second.y, uniform.second.z);
        }
        std::cout << "Uniform: " << uniform.first << " = (" 
                  << uniform.second.x << ", " 
                  << uniform.second.y << ", " 
                  << uniform.second.z << ")" << std::endl;
    }
    for (auto& uniform : windowContext.materialUniformVector2)
    {
        GLint location = glGetUniformLocation(windowContext.program, uniform.first.c_str());
        if (location != -1)
        {
            glUniform2f(location, uniform.second.x, uniform.second.y);
        }
        std::cout << "Uniform: " << uniform.first << " = (" 
                  << uniform.second.x << ", " 
                  << uniform.second.y << ")" << std::endl;
    }
}
// Function to load material (shaders) for a mesh, either from GLTF or use defaults
void loadMaterial(WindowContext& windowContext, tinygltf::Model& model, std::filesystem::path gltfDirectory, unsigned int materialId) {
    // Paths to vertex and fragment shader files
    std::filesystem::path vertexShaderPath;
    std::filesystem::path fragmentShaderPath;
    // Strings to hold shader source code
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    
    // Default vertex shader source code (GLSL)
    const char* defaultVertexShaderSource = R"(
        attribute vec2 position;
        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )";

    // Default fragment shader source code (GLSL)
    // This shader generates a simple color animation based on time
    const char* defaultFragmentShaderSource = R"(
    precision mediump float;
    uniform float time;
    void main() {
        float r = 0.5 + 0.5 * sin(time);
        float g = 0.5 + 0.5 * sin(time + 2.0);
        float b = 0.5 + 0.5 * sin(time + 4.0);
        gl_FragColor = vec4(r, g, b, 1.0);
    }
)";

    
    // Check if the material exists in the GLTF file
    if(materialId < model.materials.size())
    {
        // Try to get custom shader file names from the material's extras
        auto gltfMaterialExtras = model.materials[materialId].extras;
        if(gltfMaterialExtras.Has("shader"))
        {
            auto gltfMaterialShader = gltfMaterialExtras.Get("shader");
            if(gltfMaterialShader.Has("vertex"))
            {
                // Get the vertex shader file name from the GLTF material
                std::string gltfMaterialShaderVertex = gltfMaterialShader.Get("vertex").Get<std::string>();
                vertexShaderPath = gltfDirectory / gltfMaterialShaderVertex;
            }
            if(gltfMaterialShader.Has("fragment"))
            {
                // Get the fragment shader file name from the GLTF material
                std::string gltfMaterialShaderFragment = gltfMaterialShader.Get("fragment").Get<std::string>();
                fragmentShaderPath = gltfDirectory / gltfMaterialShaderFragment;
            }
            if (gltfMaterialShader.Has("uniforms"))
            {
                auto gltfUniforms = gltfMaterialShader.Get("uniforms");
                for (int uniformIdx = 0; uniformIdx < gltfUniforms.ArrayLen(); uniformIdx++)
                {
                    auto uniform = gltfUniforms.Get(uniformIdx);
                    std::string uniformName;
                    if (uniform.Has("name"))
                    {
                        uniformName = uniform.Get("name").Get<std::string>();
                    }
                    if (uniform.Has("type"))
                    {
                        std::string type = uniform.Get("type").Get<std::string>();
                        auto uniformValue = uniform.Get("value");
                        if(type == "Float")
                        {
                            double uniformValueFloat = uniformValue.Get(0).Get<double>();
                            windowContext.gl.materialUniformFloats[uniformName] = uniformValueFloat;
                            // Print the uniform name and value to standard output
                            std::cout << "Uniform " << uniformName << " = " << uniformValueFloat << std::endl;
                            
                        }
                        else if(type == "Vector4")
                        {
                            // Assuming the value is an array of 4 floats
                            if (uniformValue.ArrayLen() >= 4)
                            {
                                double x = uniformValue.Get(0).Get<double>();
                                double y = uniformValue.Get(1).Get<double>();
                                double z = uniformValue.Get(2).Get<double>();
                                double w = uniformValue.Get(3).Get<double>();
                                windowContext.gl.materialUniformVector4[uniformName] = Vector4(x, y, z, w);
                                // Print the uniform name and value to standard output
                                std::cout << "Uniform " << uniformName << " = (" 
                                          << x << ", " << y << ", " 
                                          << z << ", " << w << ")" << std::endl;
                            }
                        }
                        else if(
                            type == "Vector3")
                        {
                            // Assuming the value is an array of 3 floats
                            if (uniformValue.ArrayLen() >= 3)
                            {
                                double x = uniformValue.Get(0).Get<double>();
                                double y = uniformValue.Get(1).Get<double>();
                                double z = uniformValue.Get(2).Get<double>();
                                windowContext.gl.materialUniformVector3[uniformName] = Vector3(x, y, z);
                                // Print the uniform name and value to standard output
                                std::cout << "Uniform " << uniformName << " = (" 
                                          << x << ", " << y << ", " 
                                          << z << ")" << std::endl;
                            }
                        }
                        else if(type == "Vector2")
                        {
                            // Assuming the value is an array of 2 floats
                            if (uniformValue.ArrayLen() >= 2)
                            {
                                double x = uniformValue.Get(0).Get<double>();
                                double y = uniformValue.Get(1).Get<double>();
                                windowContext.gl.materialUniformVector2[uniformName] = Vector2(x, y);
                                // Print the uniform name and value to standard output
                                std::cout << "Uniform " << uniformName << " = (" 
                                          << x << ", " << y << ")" << std::endl;
                            }
                        }
                        // Check for integer type uniforms
                        else if(type == "Int")
                        {
                            int uniformValueInt = uniformValue.Get(0).Get<int>();
                            windowContext.gl.materialUniformInts[uniformName] = uniformValueInt;
                            // Print the uniform name and value to standard output
                            std::cout << "Uniform " << uniformName << " = " << uniformValueInt << std::endl;
                        }
                        else
                        {
                            std::cerr << "Unsupported uniform type: " << type << " for uniform: " << uniformName << std::endl;
                        }
                    }
                }
            }
        }
        // Try to read vertex shader source code from file if path is set
        std::ifstream vertexShaderFile(vertexShaderPath);
        if(vertexShaderFile.is_open())
        {
            std::stringstream buffer;
            buffer << vertexShaderFile.rdbuf();
            vertexShaderSource = buffer.str();
        }
        // Try to read fragment shader source code from file if path is set
        std::ifstream fragmentShaderFile(fragmentShaderPath);
        if(fragmentShaderFile.is_open())
        {
            std::stringstream buffer;
            buffer << fragmentShaderFile.rdbuf();
            fragmentShaderSource = buffer.str();
        }
    }
    else
    {
        // If no custom shaders, use the default shader source code
        vertexShaderSource = defaultVertexShaderSource;
        fragmentShaderSource = defaultFragmentShaderSource;
    }
    // If no vertex shader source code was loaded, use the default
    if (vertexShaderSource.empty()) {
        vertexShaderSource = defaultVertexShaderSource;
    }

    // Convert shader source code to    C-style strings for OpenGL
    const char *vertexShaderSourceCStr = vertexShaderSource.c_str();
    const char *fragmentShaderSourceCStr = fragmentShaderSource.c_str();

    // Compile the vertex shader from source code
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSourceCStr);
    if (!vertexShader) return;

    // Compile the fragment shader from source code
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceCStr);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return;
    } 

    // Link the compiled shaders into a shader program

    windowContext.gl.program = linkProgram(vertexShader, fragmentShader);

    // If linking failed, clean up shader objects
    if (!windowContext.gl.program) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    // Set the current shader program for rendering
    glUseProgram(windowContext.gl.program);
}

// Function to load mesh data from a GLTF model and upload it to GPU buffers
void loadMesh(WindowContext &windowContext, tinygltf::Model& model, unsigned int meshId)
{
    // Buffer handles for vertex, normal, and texture coordinate data
    GLuint vertexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint texCoordBuffer = 0;

    // Get accessor indices for position, normal, texcoord, and indices from the mesh
    uint32_t gltfAccessorPositionIndex = model.meshes[meshId].primitives[0].attributes["POSITION"];
    uint32_t gltfAccessorNormalIndex = model.meshes[meshId].primitives[0].attributes["NORMAL"];
    uint32_t gltfAccessorTexCoordIndex = model.meshes[meshId].primitives[0].attributes["TEXCOORD_0"];
    uint32_t gltfAccessorIndicesIndex = model.meshes[meshId].primitives[0].indices;

    // Get buffer view indices for each attribute
    uint32_t gltfBufferViewPositionIndex = model.accessors[gltfAccessorPositionIndex].bufferView;
    uint32_t gltfBufferViewNormalIndex = model.accessors[gltfAccessorNormalIndex].bufferView;
    uint32_t gltfBufferViewTexCoordIndex = model.accessors[gltfAccessorTexCoordIndex].bufferView;
    uint32_t gltfBufferIndicesIndex = model.accessors[gltfAccessorIndicesIndex].bufferView;

    // Get buffer indices for each buffer view
    uint32_t gltfBufferIndexPosition = model.bufferViews[gltfBufferViewPositionIndex].buffer;
    uint32_t gltfBufferIndexNormal = model.bufferViews[gltfBufferViewNormalIndex].buffer;
    uint32_t gltfBufferIndexTexCoord = model.bufferViews[gltfBufferViewTexCoordIndex].buffer;
    uint32_t gltfBufferIndexIndices = model.bufferViews[gltfBufferIndicesIndex].buffer;

    // Get raw data pointers for each buffer
    unsigned char* gltfBufferDataPosition = model.buffers[gltfBufferIndexPosition].data.data();
    unsigned char* gltfBufferDataNormal = model.buffers[gltfBufferIndexNormal].data.data();
    unsigned char* gltfBufferDataTexCoord = model.buffers[gltfBufferIndexTexCoord].data.data();
    unsigned char* gltfBufferDataIndices = model.buffers[gltfBufferIndexIndices].data.data();

    // Get byte offsets for each buffer view
    uint32_t gltfPositionByteOffset = model.bufferViews[gltfBufferViewPositionIndex].byteOffset;
    uint32_t gltfNormalByteOffset = model.bufferViews[gltfBufferViewNormalIndex].byteOffset;
    uint32_t gltfTexCoordByteOffset = model.bufferViews[gltfBufferViewTexCoordIndex].byteOffset;
    uint32_t gltfIndecesByteOffset = model.bufferViews[gltfBufferIndicesIndex].byteOffset;

    // Get byte lengths for each buffer view
    uint32_t gltfPositionByteLength = model.bufferViews[gltfBufferViewPositionIndex].byteLength;
    uint32_t gltfNormalByteLength = model.bufferViews[gltfBufferViewNormalIndex].byteLength;
    uint32_t gltfTexCoordByteLength = model.bufferViews[gltfBufferViewTexCoordIndex].byteLength;
    uint32_t gltfIndicesByteLength = model.bufferViews[gltfBufferIndicesIndex].byteLength;

    // Calculate the number of indices to draw (for glDrawElements)
    windowContext.gl.indicesCount = model.accessors[gltfAccessorIndicesIndex].count;

    // Create and upload index buffer to the GPU
    unsigned int indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gltfIndicesByteLength, gltfBufferDataIndices + gltfIndecesByteOffset, GL_STATIC_DRAW);

    // Store the index buffer handle in the window context
    windowContext.gl.indexBuffer = indexBuffer;

    // Create and upload vertex buffer (positions) to the GPU
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, gltfPositionByteLength, gltfBufferDataPosition + gltfPositionByteOffset, GL_STATIC_DRAW);

    // Create and upload normal buffer to the GPU
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, gltfNormalByteLength, gltfBufferDataNormal + gltfNormalByteOffset, GL_STATIC_DRAW);

    // Create and upload texture coordinate buffer to the GPU
    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, gltfTexCoordByteLength, gltfBufferDataTexCoord + gltfTexCoordByteOffset, GL_STATIC_DRAW);

    // Create and bind a Vertex Array Object (VAO) to store attribute/buffer bindings
    glGenVertexArrays(1, &windowContext.gl.vertexArrayObject);
    glBindVertexArray(windowContext.gl.vertexArrayObject);

    // Bind position buffer to attribute location 0 in the VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind normal buffer to attribute location 1 in the VAO
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind texture coordinate buffer to attribute location 2 in the VAO
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // TEXCOORD_0 is usually vec2

    // Unbind buffers and VAO to avoid accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main(void)
{
    // Create a window context structure to hold OpenGL state
    WindowContext windowContext;
    // Pointer to the GLFW window object
    GLFWwindow* window;

    // Path to the GLTF file to load (relative to the executable)
    std::string gltfFileName = R"(../example/06_shadertoy/export/shadertoy.gltf)";
    // Initialize the GLFW library (for window and OpenGL context management)
    if (!glfwInit())
        return -1;

    // Set GLFW window hints for OpenGL ES context creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    // Create a window and associated OpenGL context
    window = glfwCreateWindow(640, 480, "gogo", NULL, NULL);
    if (!window)
    {
        // If window creation failed, clean up and exit
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current for this thread
    glfwMakeContextCurrent(window);

    // Create a tinygltf model object to hold the loaded GLTF data
    tinygltf::Model model;
    // Create a tinygltf loader object to load the GLTF file
    tinygltf::TinyGLTF loader;
    // Strings to hold error and warning messages from the loader
    std::string err, warn;

    // Load the GLTF model from file (ASCII format)
    if(!loader.LoadASCIIFromFile(&model, &err, &warn, gltfFileName)){
        // If loading failed, print error and warning messages
        std::cerr << "Failed to load gltf file" << gltfFileName << "\n";    
        std::cerr << "Error: " << err << std::endl; 
        std::cerr << "Warning: " << warn << std::endl;
        return 1;
    }

    // Load mesh data from the GLTF model (using mesh index 0)
    unsigned int meshId = 0;
    loadMesh(windowContext, model, meshId);

    // Get the directory of the GLTF file (for loading shaders from the same folder)
    std::filesystem::path gltfPath = gltfFileName;
    std::filesystem::path gltfDirectory = gltfPath.parent_path();

    // Load material (shaders) for the mesh (using material index 0)
    loadMaterial(windowContext, model, gltfDirectory, 0);

    // Main render loop: runs until the window is closed
    while (!glfwWindowShouldClose(window))
    {
        // Set the clear color to white (RGBA)
        glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
        // Clear the color buffer (erase previous frame)
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program for rendering
        glUseProgram(windowContext.gl.program);
        // Bind the VAO (vertex array object) for the mesh
        glBindVertexArray(windowContext.gl.vertexArrayObject);
        // Bind the index buffer for drawing
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, windowContext.gl.indexBuffer);

        materialSetProperty(windowContext.gl, "iTime", getcurrentTime());   // Example of setting a uniform property (time)
        materialUpdateProperties(windowContext.gl);  // Update material properties (uniforms) before rendering

        // Draw the mesh using the index buffer (GL_TRIANGLES mode)
        glDrawElements(GL_TRIANGLES, windowContext.gl.indicesCount, GL_UNSIGNED_SHORT, nullptr);
    

        // Swap the front and back buffers (display the rendered image)
        glfwSwapBuffers(window);

        // Poll for window events (keyboard, mouse, etc.)
        glfwPollEvents();
    }

    // Clean up and close the window and OpenGL context
    glfwTerminate();
    return 0;
}
