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
"uniform mat4 model;"
"void main()"
"{ gl_Position = model*vec4(aPos, 1.0); }";
const char* fragment_shader_src = "#version 330 core\nout vec4 FragColor; void main() { FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); }";

float vertices[] = {
  -0.5f, 0.5f, -0.5f, // BACK left top: 0
  -0.5f, -0.5f, -0.5f, // BACK left bottom: 1
  0.5f, -0.5f, -0.5f, // BACK right bottom: 2
  0.5f, 0.5f, -0.5f, // BACK right top: 3
  
  -0.5f, 0.5f, 0.5f, // FRONT left top: 4
  -0.5f, -0.5f, 0.5f, // FRONT left bottom: 5
  0.5f, -0.5f, 0.5f, // FRONT right bottom: 6
  0.5f, 0.5f, 0.5f, // FRONT right top: 7
};

unsigned int indices[] = {
  // back face
  0, 1, 2,  2, 3, 0,

  // front face
  4, 5, 6,  6, 7, 4,

  // left
  0, 1, 5,  5, 4, 0,

  // right
  3, 2, 6,  6, 7, 3,

  // top
  0, 3, 4,  4, 7, 0,

  // bottom
  1, 2, 5,  5, 6, 1
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
  GLFWwindow *window = glfwCreateWindow(900, 900, "Quartz\'s super awesome app and stuff", NULL, NULL);

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
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

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
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    // SHADERS
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(shader_program);
    
    // RENDER
    float ct = (float)glfwGetTime();
    mat4 model;
    glm_mat4_identity(model);
    
    vec3 pos = {0.0f, 0.0f, 0.0f};
    vec3 axis = {1.0f, 1.0f, 1.0f};
    vec3 scale = {1.0, 1.0, 1.0};

    glm_scale(model, scale);
    glm_rotate(model, ct, axis);
    glm_translate(model, pos);
    
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
