#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cstdio>
#include <cstdlib>

static const char* VS = R"(#version 330 core
layout(location=0) in vec2 vPosition;
layout(location=1) in vec3 vColor;
out vec3 color;
void main(){
    color = vColor;
    gl_Position = vec4(vPosition, 0.0, 1.0);
})";

static const char* FS = R"(#version 330 core
in vec3 color;
out vec4 FragColor;
void main(){
    FragColor = vec4(color, 1.0);
})";

static void framebuffer_size_callback(GLFWwindow*, int w, int h){ glViewport(0,0,w,h); }
static void check(bool ok, const char* msg){ if(!ok){ std::fprintf(stderr,"[!] %s\n", msg); std::exit(1);} }

static GLuint makeProgram(const char* vs, const char* fs){
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, nullptr); glCompileShader(v);
    GLint ok=0; glGetShaderiv(v, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(v,1024,nullptr,log); std::fprintf(stderr,"VS error:\n%s\n",log); std::exit(1); }

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, nullptr); glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(f,1024,nullptr,log); std::fprintf(stderr,"FS error:\n%s\n",log); std::exit(1); }
    
    GLuint p = glCreateProgram(); glAttachShader(p,v); glAttachShader(p,f); glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ char log[1024]; glGetProgramInfoLog(p,1024,nullptr,log); std::fprintf(stderr,"Link error:\n%s\n",log); std::exit(1); }
    glDeleteShader(v); glDeleteShader(f);
    return p;
}


//shapes

// Star (5-point) as TRIANGLE_FAN
static std::vector<float> makeStar(float cx,float cy,float rOut,float rIn){
    std::vector<float> v;
    v.insert(v.end(),{cx,cy,1,1,0}); // center yellow
    for(int i=0;i<=10;i++){
        float a=(M_PI/2)+(i*36.0f*M_PI/180.0f); // 36° per step
        float r=(i%2==0)?rOut:rIn;
        float x=cx+r*cos(a), y=cy+r*sin(a);
        v.insert(v.end(),{x,y,1,0.5f,0}); // orange
    }
    return v;
}

// Ring as TRIANGLE_STRIP
static std::vector<float> makeRing(float cx,float cy,float rOut,float rIn,int seg){
    std::vector<float> v;
    for(int i=0;i<=seg;i++){
        float a=i*2*M_PI/seg;
        float co=cos(a), si=sin(a);
        v.insert(v.end(),{cx+rOut*co,cy+rOut*si,0,1,1}); // outer cyan
        v.insert(v.end(),{cx+rIn*co, cy+rIn*si, 0,0.2f,0.5f}); // inner dark
    }
    return v;
}

// Hexagon as TRIANGLE_FAN
static std::vector<float> makeHexagon(float cx,float cy,float r){
    std::vector<float> v;
    v.insert(v.end(),{cx,cy,0.8f,0,0.8f}); // center purple
    for(int i=0;i<=6;i++){
        float a=M_PI/6+i*M_PI/3; // start 30°, step 60°
        float x=cx+r*cos(a), y=cy+r*sin(a);
        v.insert(v.end(),{x,y,0.3f+0.1f*i,0.1f*i,1.0f-0.1f*i}); // gradient
    }
    return v;
}

int main(){
    check(glfwInit(),"glfwInit failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win=glfwCreateWindow(500,500,"Part 2 Scene",nullptr,nullptr);
    check(win!=nullptr,"glfwCreateWindow failed");
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win,framebuffer_size_callback);
    check(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),"gladLoadGLLoader failed");

    GLuint prog=makeProgram(VS,FS);

    auto star=makeStar(-0.5f,0.5f,0.2f,0.08f);
    auto ring=makeRing(0.5f,0.5f,0.2f,0.14f,64);
    auto hex =makeHexagon(0.0f,-0.3f,0.3f);

    GLuint vao[3],vbo[3];
    glGenVertexArrays(3,vao); glGenBuffers(3,vbo);

    auto upload=[&](int i,const std::vector<float>&d){
        glBindVertexArray(vao[i]); glBindBuffer(GL_ARRAY_BUFFER,vbo[i]);
        glBufferData(GL_ARRAY_BUFFER,d.size()*sizeof(float),d.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(2*sizeof(float)));
        glEnableVertexAttribArray(1);
    };
    upload(0,star); upload(1,ring); upload(2,hex);
    while(!glfwWindowShouldClose(win)){
        glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);
        glBindVertexArray(vao[0]); glDrawArrays(GL_TRIANGLE_FAN,0,star.size()/5);
        glBindVertexArray(vao[1]); glDrawArrays(GL_TRIANGLE_STRIP,0,ring.size()/5);
        glBindVertexArray(vao[2]); glDrawArrays(GL_TRIANGLE_FAN,0,hex.size()/5);
        glfwSwapBuffers(win); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}