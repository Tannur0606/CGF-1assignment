// Task 2 — Scene per spec (OpenGL 3.3 Core)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
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

// ===== geometry =====

// Круг/эллипс с изменением красного канала по углу (TRIANGLE_FAN, 1-я вершина — центр)
static std::vector<float> makeCircleFan(float cx, float cy, float r, float yScale,
                                        int segments, float redPhase){
    std::vector<float> v; v.reserve((segments+2)*5);
    v.insert(v.end(), {cx, cy, 0.18f, 0.f, 0.f}); // центр чуть темнее
    const float TAU = 6.28318530718f;
    for(int i=0;i<=segments;++i){
        float a = (float)i/segments * TAU;
        float x = cx + r * std::cos(a);
        float y = cy + (r * yScale) * std::sin(a);
        float red = 0.35f + 0.65f * (0.5f*(1.0f+std::cos(a+redPhase)));
        v.insert(v.end(), {x, y, red, 0.f, 0.f});
    }
    return v;
}

// Однотонный круг/эллипс (TRIANGLE_FAN, 1-я вершина — центр)
static std::vector<float> makeCircleFanSolid(float cx, float cy, float r, float yScale,
                                             int segments, float R, float G, float B){
    std::vector<float> v; v.reserve((segments+2)*5);
    v.insert(v.end(), {cx, cy, R, G, B});
    const float TAU = 6.28318530718f;
    for(int i=0;i<=segments;++i){
        float a = (float)i/segments * TAU;
        float x = cx + r * std::cos(a);
        float y = cy + (r * yScale) * std::sin(a);
        v.insert(v.end(), {x, y, R, G, B});
    }
    return v;
}

// Заполненный квадрат (TRIANGLE_STRIP), вершины рассчитаны по углам π/4, далее шаг π/2
static std::vector<float> makeSquareByAngleFilled(float cx, float cy, float radius, float gray){
    float c[4][2];
    const float start = 3.1415926535f/4.0f; // π/4
    for(int i=0;i<4;++i){
        float a = start + i*(3.1415926535f/2.0f);
        c[i][0] = cx + radius * std::cos(a);
        c[i][1] = cy + radius * std::sin(a);
    }
    // углы: TR, TL, BL, BR → переставим в BL, BR, TL, TR (для TRIANGLE_STRIP)
    std::vector<float> v = {
        c[2][0], c[2][1], gray,gray,gray, // BL
        c[3][0], c[3][1], gray,gray,gray, // BR
        c[1][0], c[1][1], gray,gray,gray, // TL
        c[0][0], c[0][1], gray,gray,gray  // TR
    };
    return v;
}

int main(){
    check(glfwInit(), "glfwInit failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(500,500,"Task 2 — Scene", nullptr, nullptr);
    check(win!=nullptr, "glfwCreateWindow failed");
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
    check(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "gladLoadGLLoader failed");
    glViewport(0,0,500,500);

    GLuint prog = makeProgram(VS,FS);

    // --- RGB-треугольник (интерполяция цвета)
    float tri[] = {
         0.00f,  0.65f,    1.f,0.f,0.f,
        -0.25f,  0.25f,    0.f,1.f,0.f,
         0.25f,  0.25f,    0.f,0.f,1.f
    };

    // --- Верх: левый однотонный эллипс (60% по Y), правый — круг с градиентом по углу
    auto leftEllipse  = makeCircleFanSolid(-0.55f, 0.60f, 0.17f, 0.60f, 96, 1.0f, 0.0f, 0.0f);
    auto rightCircle  = makeCircleFan(      0.55f, 0.60f, 0.17f, 1.00f, 96, 1.2f);

    // --- Низ: белые рамки (white→black→white→black→white→black), толщины подогнаны
    const float cx = 0.f, cy = -0.35f;
    auto sq1_white = makeSquareByAngleFilled(cx, cy, 0.57f, 1.00f); // внешний белый (чуть тоньше)
    auto gap1_blk  = makeSquareByAngleFilled(cx, cy, 0.47f, 0.00f); // чёрный зазор
    auto sq2_white = makeSquareByAngleFilled(cx, cy, 0.36f, 1.00f); // средний белый
    auto gap2_blk  = makeSquareByAngleFilled(cx, cy, 0.26f, 0.00f); // чёрный зазор
    auto sq3_white = makeSquareByAngleFilled(cx, cy, 0.165f,1.00f); // внутренний белый
    auto inner_blk = makeSquareByAngleFilled(cx, cy, 0.095f,0.00f); // чёрный центр

    // --- буферы
    GLuint vao[9], vbo[9];
    glGenVertexArrays(9, vao);
    glGenBuffers(9, vbo);

    auto upload = [&](int i, const float* data, size_t floats){
        glBindVertexArray(vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, floats*sizeof(float), data, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0); // vPosition
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(1); // vColor
    };

    upload(0, tri,             sizeof(tri)/sizeof(float));
    upload(1, leftEllipse.data(), leftEllipse.size());
    upload(2, rightCircle.data(), rightCircle.size());
    upload(3, sq1_white.data(),   sq1_white.size());
    upload(4, gap1_blk.data(),    gap1_blk.size());
    upload(5, sq2_white.data(),   sq2_white.size());
    upload(6, gap2_blk.data(),    gap2_blk.size());
    upload(7, sq3_white.data(),   sq3_white.size());
    upload(8, inner_blk.data(),   inner_blk.size());

    while(!glfwWindowShouldClose(win)){
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);

        // верх
        glBindVertexArray(vao[1]); glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)(leftEllipse.size()/5));
        glBindVertexArray(vao[2]); glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)(rightCircle.size()/5));
        glBindVertexArray(vao[0]); glDrawArrays(GL_TRIANGLES,    0, 3);

        // низ — рамки
        for(int i=3;i<=8;++i){
            glBindVertexArray(vao[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}