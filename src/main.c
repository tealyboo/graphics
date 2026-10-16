#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "../libs/glad.h"
#include "../libs/cglm/cglm.h"
#include <GLFW/glfw3.h>
#include <GL/gl.h>

const char* vertex_shader_src = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;"
"layout (location = 1) in vec2 aTex;"
"uniform mat4 model;"
"out vec2 texCoord;"
"out vec2 oriCoord;"
"void main()"
"{"
  "gl_Position = model*vec4(aPos, 1.0);"
  "texCoord = aTex;"
  "oriCoord = aPos.xy;"
"}";
const char* fragment_shader_src = "#version 330 core\n"
"out vec4 FragColor;"
"in vec2 texCoord;"
"in vec2 oriCoord;"
"void main()"
"{"
"float minv = 0.5f;"
"float maxv = 1.0f-minv;"
"float color = minv+(oriCoord.x*maxv);"

"vec2 dist = abs(texCoord-vec2(0.5f))+0.5f;"
"float largest_dist = max(dist.x, dist.y);"
"float add = step(0.995f, largest_dist);"

"FragColor = vec4(add, 1.0f, color+add, 1.0f);"
"}";


float vertices[] = {
    // BACK face (z = -0.5)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 0: top left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 1: bottom left
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 2: bottom right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 3: top right

    // FRONT face (z = +0.5)
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 4
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 5
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 6
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 7

    // LEFT face (x = -0.5)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 8
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 9
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 10
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 11

    // RIGHT face (x = +0.5)
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 12
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 13
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 14
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 15

    // TOP face (y = +0.5)
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 16
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // 17
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, // 18
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 19

    // BOTTOM face (y = -0.5)
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 20
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 21
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 22
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f  // 23
};

unsigned int indices[] = {
    // Back
     0,  1,  2,    2,  3,  0,

    // Front
     4,  5,  6,    6,  7,  4,

    // Left
     8,  9, 10,   10, 11,  8,

    // Right
    12, 13, 14,   14, 15, 12,

    // Top
    16, 17, 18,   18, 19, 16,

    // Bottom
    20, 21, 22,   22, 23, 20
};

float get_t() {
  struct timespec ts;
  clock_gettime(TIME_UTC, &ts);

  return (float)(ts.tv_sec)+(float)(ts.tv_nsec/1000000.0f);
}

int main() {
  // GLFW BOILERPLATE
  if (!glfwInit()) { return -1; }
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(450, 450, "Quartz\'s super awesome app and stuff", NULL, NULL);

  if (!window) { return -1; }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // BIND BUFFER
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // BIND VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  // BIND EBO
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // CREATE SHADER PROGRAM
  unsigned int vertex_shader = 0;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);

  unsigned int fragment_shader = 0;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);

  // COMPILE SHADERS
  int vertex_success;
  int fragment_success;

  glCompileShader(vertex_shader);
  glCompileShader(fragment_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_success);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_success);

  if (!vertex_success) {
    printf("Failed to compile vertex shaders.");
    return -1;
  }

  if (!fragment_success) {
    printf("Failed to compile fragment shaders.");
    return -1;
  }

  // ATTACH SHADERS TO PROGRAM
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  
  // LINK PROGRAM
  int program_success;
  glLinkProgram(shader_program);

  glGetProgramiv(shader_program, GL_LINK_STATUS, &program_success);

  if (!program_success) {
    printf("Failed to link shader program.");
    return -1;
  }

  int modelLoc = glGetUniformLocation(shader_program, "model");

  // RENDER LOOP
  while (!glfwWindowShouldClose(window)) {
    // SHADERS
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);
    
    // RENDER
    float ct = (float)glfwGetTime();
    mat4 model;
    glm_mat4_identity(model);
    
    vec3 pos = {(float)cos(ct)*0.1f, (float)sin(ct)*0.1f, 0.0f};
    vec3 axis = {1.0f, 1.0f, -1.0f};
    vec3 scale = {1.0, 1.0, 1.0};

    glm_translate(model, pos);
    glm_rotate(model, ct+sin(ct)*cos(ct), axis);
    glm_scale(model, scale);
    
    printf("%f\n", ct);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
    // DRAW
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
