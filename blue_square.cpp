
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

static const char* VS = R"(#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec3 aColor;
out vec3 vColor;
void main(){
    vColor = aColor;
    gl_Position = vec4(aPos, 0.0, 1.0);
})";

static const char* FS = R"(#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main(){
    FragColor = vec4(vColor, 1.0);
})";

static void framebuffer_size_callback(GLFWwindow*, int w, int h){ glViewport(0,0,w,h); }
static void check(bool ok, const char* msg){ if(!ok){ std::fprintf(stderr,"[!] %s\n", msg); std::exit(1);} }

static GLuint makeProgram(const char* vs, const char* fs){
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, nullptr); glCompileShader(v);
    GLint ok=0; glGetShaderiv(v, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(v,1024,nullptr,log); std::fprintf(stderr,"VS error: %s\n",log); std::exit(1); }
    
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, nullptr); glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(f,1024,nullptr,log); std::fprintf(stderr,"FS error: %s\n",log); std::exit(1); }

    GLuint p = glCreateProgram(); glAttachShader(p,v); glAttachShader(p,f); glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ char log[1024]; glGetProgramInfoLog(p,1024,nullptr,log); std::fprintf(stderr,"Link error: %s\n",log); std::exit(1); }
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

int main(){
    check(glfwInit(), "glfwInit failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(500,500,"Task 1 — Blue Square", nullptr, nullptr);
    check(win!=nullptr, "glfwCreateWindow failed");
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    check(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "gladLoadGLLoader failed");
    glViewport(0,0,500,500);

    GLuint prog = makeProgram(VS,FS);

    // Vertex data : Blue square 
float square[] = {
   -0.5f,-0.5f, 0,0,1,  // BL
    0.5f,-0.5f, 0,0,1,  // BR
   -0.5f, 0.5f, 0,0,1,  // TL
    0.5f, 0.5f, 0,0,1   // TR
};

// VAo/VBO setup
GLuint vao,vbo; 
glGenVertexArrays(1,&vao); 
glGenBuffers(1,&vbo);

glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);


//attributes: location 0 = vPosition (x,y), location 1= vColor (r,g,b)
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
glEnableVertexAttribArray(1);


// Render loop
while(!glfwWindowShouldClose(win)){
    glClearColor(0.05f,0.05f,0.05f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(prog);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glfwSwapBuffers(win);
    glfwPollEvents();
}


    glfwTerminate();
    return 0;
}
