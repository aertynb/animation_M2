#include "ViewerApplication.hpp"

#include <iostream>
#include <numeric>
#include <chrono>
#include <thread>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "utils/cameras.hpp"
#include "utils/cube.hpp"
#include "utils/quad.hpp"
#include "utils/skybox.hpp"

#include "physics/Engine.hpp"

#include <stb_image.h>
#include <stb_image_write.h>
#include <tiny_gltf.h>

void keyCallback(
    GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
    glfwSetWindowShouldClose(window, 1);
  }
}

std::vector<GLuint> ViewerApplication::createBufferObjects(
    const tinygltf::Model &model)
{
  std::vector<GLuint> bufferObjects(
      model.buffers.size(), 0); // Assuming buffers is a std::vector of Buffer
  glGenBuffers(model.buffers.size(), bufferObjects.data());
  for (size_t i = 0; i < model.buffers.size(); ++i) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[i]);
    glBufferStorage(GL_ARRAY_BUFFER,
        model.buffers[i].data.size(), // Assume a Buffer has a data member
                                      // variable of type std::vector
        model.buffers[i].data.data(), 0);
  }
  glBindBuffer(
      GL_ARRAY_BUFFER, 0); // Cleanup the binding point after the loop only

  return bufferObjects;
}

int ViewerApplication::run()
{
  // Loader shaders
  auto glslProgram = compileProgram({m_ShadersRootPath / m_vertexShader,
      m_ShadersRootPath / m_fragmentShader});

  // Build projection matrix
  const auto diag = glm::vec3(1., 1., 1);
  auto maxDistance = glm::length(diag);
  const auto projMatrix =
      glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight,
          0.1f, 200.0f);

  std::unique_ptr<CameraController> cameraController =
      std::make_unique<FirstPersonCameraController>(
          m_GLFWHandle.window(), 0.5f * maxDistance);
  if (m_hasUserCamera) {
    cameraController->setCamera(m_userCamera);
  } else {
    const auto center = glm::vec3(0, 0, 0);
    const auto up = glm::vec3(0, 1, 0);
    const auto eye = glm::vec3(0, 0, -2);
    cameraController->setCamera(
        Camera{eye, center, up});
  }

  // Gen default texture for object
  float white[] = {1., 1., 1., 1.};
  GLuint whiteTexture;
  glGenTextures(1, &whiteTexture);
  glBindTexture(GL_TEXTURE_2D, whiteTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, white);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  // Setup OpenGL state for rendering
  glEnable(GL_DEPTH_TEST);
  glslProgram.use();

  UniformHandler uniforms(glslProgram);

  const auto pathToFaces = "assets/";

  const std::vector<std::string> faces{"assets/skybox/right.jpg",
      "assets/skybox/left.jpg", "assets/skybox/top.jpg",
      "assets/skybox/bottom.jpg", "assets/skybox/front.jpg",
      "assets/skybox/back.jpg"};

  QuadCustom quad(1, 1);
  CubeCustom cube(1, 1, 1);
  SphereCustom sphere(1, 32, 32);
  Skybox skybox(faces, m_ShadersRootPath);
  Engine engine;
  // cube.initObj(0, 1, 2);

  const auto drawScene = [&](const Camera &camera, double h) {
    glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const auto viewMatrix = camera.getViewMatrix();
    const auto modelMatrix = glm::mat4(1.0f);

    skybox.draw(modelMatrix, viewMatrix, projMatrix);

    glslProgram.use();

    engine.draw(h, viewMatrix, projMatrix, uniforms);
  };

  bool modifiedConstants = false;

  float Fe = engine.Fe, m = engine.m, k = engine.k, z = engine.z, s = engine.s;
  static int type = (int)engine.type;

  double previousTime = glfwGetTime();

  // Loop until the user closes the window
  for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose();
      ++iterationCount) {

    double currentTime = glfwGetTime();
    double dt = currentTime - previousTime;
    previousTime = currentTime;

    glfwSwapInterval(0); // <- V-Sync OFF (uncapped FPS)
    
    auto frameStart = std::chrono::high_resolution_clock::now();

    const auto seconds = glfwGetTime();

    const auto camera = cameraController->getCamera();
    // drawScene(camera, lightDirection, lightIntensity, lightCam,
    // occlusionState);
    drawScene(camera, dt);

    // GUI code:
    imguiNewFrame();

    {
      ImGui::Begin("GUI");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("position: %.3f %.3f %.3f", camera.eye().x, camera.eye().y,
            camera.eye().z);
      }

      if (ImGui::CollapsingHeader("Constants", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::SliderFloat("Fe", &Fe, 50.f, 4000.f)) engine.Fe = Fe;
        if (ImGui::SliderFloat("Masse", &m, 1.f, 20.f)) engine.m = m;
        if (ImGui::SliderFloat("Spring stiffness (k)", &k, 0.f, 500.f)) engine.k = k;
        if (ImGui::SliderFloat("Damping (z)", &z, 0.f, 500.f)) engine.z = z;
        if (ImGui::SliderFloat("Adhesion range (s)", &s, 0.f, 20.f)) engine.s = s;

        modifiedConstants = ImGui::RadioButton("Hook",             &type, 0)
                        ||  ImGui::RadioButton("Damped",           &type, 1)
                        ||  ImGui::RadioButton("Damped Hook",      &type, 2)
                        ||  ImGui::RadioButton("Cond Damped Hook", &type, 3);
        if (modifiedConstants) {
          engine.setType((Link::Type)type); // set new type
        }

      }

      ImGui::End();
    }

    imguiRenderFrame();

    glfwPollEvents(); // Poll for and process events

    auto ellapsedTime = glfwGetTime() - seconds;
    auto guiHasFocus =
        ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
    if (!guiHasFocus) {
      cameraController->update(float(ellapsedTime));
    }

    m_GLFWHandle.swapBuffers(); // Swap front and back buffers

  }

  // TODO clean up allocated GL data

  return 0;
}

ViewerApplication::ViewerApplication(const fs::path &appPath, uint32_t width,
    uint32_t height) :
    m_nWindowWidth(width),
    m_nWindowHeight(height),
    m_AppPath{appPath},
    m_AppName{m_AppPath.stem().string()},
    m_ImGuiIniFilename{m_AppName + ".imgui.ini"},
    m_ShadersRootPath{m_AppPath.parent_path() / "shaders"}
{
  ImGui::GetIO().IniFilename =
      m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows
                                  // positions in this file

  glfwSetKeyCallback(m_GLFWHandle.window(), keyCallback);

  // glfwSetInputMode(m_GLFWHandle.window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  printGLVersion();
}