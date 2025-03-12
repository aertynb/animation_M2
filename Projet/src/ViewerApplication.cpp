#include "ViewerApplication.hpp"

#include <iostream>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "utils/cameras.hpp"

#include <stb_image.h>
#include <stb_image_write.h>
#include <tiny_gltf.h>

GLuint modelMatrixLocation, modelViewProjMatrixLocation,
    modelViewMatrixLocation, normalMatrixLocation, uLightDirection,
    uLightIntensity, uBaseColorTexture, uBaseColorFactor, uMetallicFactor,
    uMetallicRoughnessTexture, uRoughnessFactor, uEmissiveFactor,
    uEmissiveTexture, uApplyOcclusion, uOcclusionFactor, uOcclusionTexture,
    uNormalTexture;

void keyCallback(
    GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
    glfwSetWindowShouldClose(window, 1);
  }
}

bool ViewerApplication::loadGltfFile(tinygltf::Model &model)
{
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  bool ret =
      loader.LoadASCIIFromFile(&model, &err, &warn, m_gltfFilePath.string());

  if (ret) {
    printf("glTF has been loaded sucessfully\n");
  }

  if (!ret) {
    printf("Failed to parse glTF\n");
    return -1;
  }

  if (!warn.empty()) {
    printf("Warn: %s\n", warn.c_str());
  }

  if (!err.empty()) {
    printf("Err: %s\n", err.c_str());
  }

  return ret;
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

void getUniform(const GLProgram &glslProgram)
{
  modelMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelMatrix");
  modelViewProjMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelViewProjMatrix");
  modelViewMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uModelViewMatrix");
  normalMatrixLocation =
      glGetUniformLocation(glslProgram.glId(), "uNormalMatrix");

  uLightDirection = glGetUniformLocation(glslProgram.glId(), "uLightDirection");
  uLightIntensity = glGetUniformLocation(glslProgram.glId(), "uLightIntensity");
  uBaseColorTexture =
      glGetUniformLocation(glslProgram.glId(), "uBaseColorTexture");
  uBaseColorFactor =
      glGetUniformLocation(glslProgram.glId(), "uBaseColorFactor");
  uMetallicFactor = glGetUniformLocation(glslProgram.glId(), "uMetallicFactor");
  uRoughnessFactor =
      glGetUniformLocation(glslProgram.glId(), "uRoughnessFactor");
  uMetallicRoughnessTexture =
      glGetUniformLocation(glslProgram.glId(), "uMetallicRoughnessTexture");
  uEmissiveFactor = glGetUniformLocation(glslProgram.glId(), "uEmissiveFactor");
  uEmissiveTexture =
      glGetUniformLocation(glslProgram.glId(), "uEmissiveTexture");
  uApplyOcclusion = glGetUniformLocation(glslProgram.glId(), "uApplyOcclusion");
  uOcclusionFactor =
      glGetUniformLocation(glslProgram.glId(), "uOcclusionFactor");
  uOcclusionTexture =
      glGetUniformLocation(glslProgram.glId(), "uOcclusionTexture");
  uNormalTexture = glGetUniformLocation(glslProgram.glId(), "uNormalTexture");
}

int ViewerApplication::run()
{
  // Loader shaders
  auto glslProgram = compileProgram({m_ShadersRootPath / m_vertexShader,
      m_ShadersRootPath / m_fragmentShader});

  // tinygltf::Model model;
  // loadGltfFile(model);

  std::cout << "Model imported : " << m_gltfFilePath << std::endl;

  // glm::vec3 bboxMin, bboxMax;
  // computeSceneBounds(model, bboxMin, bboxMax);

  GLuint tangentVBO;
  glGenBuffers(1, &tangentVBO);

  // ComputeTangents(model);

  // Build projection matrix
  const auto diag = glm::vec3(1., 1., 1);
  auto maxDistance = glm::length(diag);
  const auto projMatrix =
      glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight,
          0.001f * maxDistance, 1.5f * maxDistance);

  std::unique_ptr<CameraController> cameraController =
      std::make_unique<TrackballCameraController>(
          m_GLFWHandle.window(), 0.5f * maxDistance);
  if (m_hasUserCamera) {
    cameraController->setCamera(m_userCamera);
  } else {
    const auto center = glm::vec3(0.f, 0.f, 0.f);
    const auto up = glm::vec3(0, 1, 0);
    const auto eye = glm::vec3(1.f, 1.f, 1.f);
    cameraController->setCamera(Camera{eye, center, up});
  }

  // const auto textureObjects = createTextureObjects(model);

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

  // const auto bufferObjects = createBufferObjects(model);
  // bool tangentAccessor = false;

  // std::vector<VaoRange> meshToVertexArrays;
  // const auto vertexArrayObjects = createVertexArrayObjects(
  //     model, bufferObjects, meshToVertexArrays, tangentAccessor);

  // Setup OpenGL state for rendering
  glEnable(GL_DEPTH_TEST);
  glslProgram.use();

  getUniform(glslProgram);

  // const auto bindMaterial = [&](const auto materialIndex) {
  //   if (materialIndex >= 0) {
  //     const auto &material = model.materials[materialIndex];
  //     const auto &pbrMetallicRoughness = material.pbrMetallicRoughness;
  //     if (uBaseColorFactor >= 0) {
  //       glUniform4f(uBaseColorFactor,
  //           (float)pbrMetallicRoughness.baseColorFactor[0],
  //           (float)pbrMetallicRoughness.baseColorFactor[1],
  //           (float)pbrMetallicRoughness.baseColorFactor[2],
  //           (float)pbrMetallicRoughness.baseColorFactor[3]);
  //     }
  //     if (uBaseColorTexture >= 0) {
  //       auto textureObject = whiteTexture;
  //       if (pbrMetallicRoughness.baseColorTexture.index >= 0) {
  //         const auto &texture =
  //             model.textures[pbrMetallicRoughness.baseColorTexture.index];
  //         if (texture.source >= 0) {
  //           textureObject = textureObjects[texture.source];
  //         }

  //         glActiveTexture(GL_TEXTURE0);
  //         glBindTexture(GL_TEXTURE_2D, textureObject);
  //         glUniform1i(uBaseColorTexture, 0);
  //       }
  //     }
  //     if (uMetallicFactor >= 0) {
  //       glUniform1f(
  //           uMetallicFactor, (float)pbrMetallicRoughness.metallicFactor);
  //     }
  //     if (uRoughnessFactor >= 0) {
  //       glUniform1f(
  //           uRoughnessFactor, (float)pbrMetallicRoughness.roughnessFactor);
  //     }
  //     if (uMetallicRoughnessTexture >= 0) {
  //       auto textureObject = whiteTexture;
  //       if (pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
  //         const auto &texture = model.textures[pbrMetallicRoughness
  //                 .metallicRoughnessTexture.index];
  //         if (texture.source >= 0) {
  //           textureObject = textureObjects[texture.source];
  //         }

  //         glActiveTexture(GL_TEXTURE1);
  //         glBindTexture(GL_TEXTURE_2D, textureObject);
  //         glUniform1i(uMetallicRoughnessTexture, 1);
  //       }
  //     }
  //     if (uEmissiveFactor >= 0) {
  //       glUniform3f(uEmissiveFactor, (float)material.emissiveFactor[0],
  //           (float)material.emissiveFactor[1],
  //           (float)material.emissiveFactor[2]);
  //     }
  //     if (uEmissiveTexture >= 0) {
  //       auto textureObject = 0u;
  //       if (material.emissiveTexture.index >= 0) {
  //         const auto &texture =
  //         model.textures[material.emissiveTexture.index]; if (texture.source
  //         >= 0) {
  //           textureObject = textureObjects[texture.source];
  //         }

  //         glActiveTexture(GL_TEXTURE2);
  //         glBindTexture(GL_TEXTURE_2D, textureObject);
  //         glUniform1i(uEmissiveTexture, 2);
  //       }
  //     }
  //     if (uOcclusionFactor >= 0) {
  //       glUniform1f(uOcclusionFactor, material.occlusionTexture.strength);
  //     }
  //     if (uOcclusionTexture >= 0) {
  //       auto textureObject = whiteTexture;
  //       if (material.emissiveTexture.index >= 0) {
  //         const auto &texture =
  //         model.textures[material.occlusionTexture.index]; if (texture.source
  //         >= 0) {
  //           textureObject = textureObjects[texture.source];
  //         }

  //         glActiveTexture(GL_TEXTURE3);
  //         glBindTexture(GL_TEXTURE_2D, textureObject);
  //         glUniform1i(uOcclusionTexture, 3);
  //       }
  //     }
  //     if (uNormalTexture >= 0) {
  //       auto textureObject = whiteTexture;
  //       if (material.normalTexture.index >= 0) {
  //         const auto &texture = model.textures[material.normalTexture.index];
  //         if (texture.source >= 0) {
  //           textureObject = textureObjects[texture.source];
  //         }

  //         glActiveTexture(GL_TEXTURE4);
  //         glBindTexture(GL_TEXTURE_2D, textureObject);
  //         glUniform1i(uNormalTexture, 4);
  //       }
  //     }
  //   } else {
  //     // Apply default material
  //     // Defined here:
  //     //
  //     https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#reference-material
  //     //
  //     https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#reference-pbrmetallicroughness3
  //     if (uBaseColorFactor >= 0) {
  //       glUniform4f(uBaseColorFactor, 1, 1, 1, 1);
  //     }
  //     if (uBaseColorTexture >= 0) {
  //       glActiveTexture(GL_TEXTURE0);
  //       glBindTexture(GL_TEXTURE_2D, whiteTexture);
  //       glUniform1i(uBaseColorTexture, 0);
  //     }
  //     if (uRoughnessFactor >= 0) {
  //       glUniform1f(uRoughnessFactor, 0.f);
  //     }
  //     if (uMetallicRoughnessTexture >= 0) {
  //       glActiveTexture(GL_TEXTURE1);
  //       glBindTexture(GL_TEXTURE_2D, 0);
  //       glUniform1i(uMetallicRoughnessTexture, 1);
  //     }
  //     if (uEmissiveFactor >= 0) {
  //       glUniform3f(uEmissiveFactor, 0.f, 0.f, 0.f);
  //     }
  //     if (uEmissiveTexture >= 0) {
  //       glActiveTexture(GL_TEXTURE2);
  //       glBindTexture(GL_TEXTURE_2D, 0);
  //       glUniform1i(uEmissiveTexture, 2);
  //     }
  //     if (uApplyOcclusion >= 0) {
  //       glUniform1i(uApplyOcclusion, 0);
  //     }
  //     if (uOcclusionFactor >= 0) {
  //       glUniform1f(uOcclusionFactor, 0.);
  //     }
  //     if (uOcclusionTexture >= 0) {
  //       glActiveTexture(GL_TEXTURE3);
  //       glBindTexture(GL_TEXTURE_2D, 0);
  //       glUniform1i(uOcclusionTexture, 3);
  //     }
  //     if (uNormalTexture >= 0) {
  //       glActiveTexture(GL_TEXTURE4);
  //       glBindTexture(GL_TEXTURE_2D, 0);
  //       glUniform1i(uNormalTexture, 4);
  //     }
  //   }
  // };

  // Lambda function to draw the scene
  // const auto drawScene = [&](const Camera &camera, glm::vec3 lightDirection,
  //                            glm::vec3 lightIntensity, bool lightCam,
  //                            bool occlusionState) {
  //   glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //   const auto viewMatrix = camera.getViewMatrix();

  //   if (uLightDirection * uLightDirection >= 0) {
  //     glUniform3fv(uLightIntensity, 1, glm::value_ptr(lightIntensity));
  //     if (lightCam)
  //       glUniform3fv(uLightDirection, 1, glm::value_ptr(glm::vec3(0, 0, 1)));
  //     else {
  //       lightDirection = glm::normalize(
  //           glm::vec3(viewMatrix * glm::vec4(lightDirection, 0.)));
  //       glUniform3fv(uLightDirection, 1, glm::value_ptr(lightDirection));
  //     }
  //   }

  //   if (uApplyOcclusion >= 0) {
  //     glUniform1i(uApplyOcclusion, occlusionState ? 1 : 0);
  //   }

  //   // The recursive function that should draw a node
  //   // We use a std::function because a simple lambda cannot be recursive
  //   const std::function<void(int, const glm::mat4 &)> drawNode =
  //       [&](int nodeIdx, const glm::mat4 &parentMatrix) {
  //         const auto &node = model.nodes[nodeIdx];
  //         const glm::mat4 modelMatrix =
  //             getLocalToWorldMatrix(node, parentMatrix);

  //         if (node.mesh >= 0) {
  //           const auto mvMatrix = viewMatrix * modelMatrix;
  //           const auto mvpMatrix = projMatrix * mvMatrix;
  //           const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

  //           glUniformMatrix4fv(
  //               modelMatrixLocation, 1, GL_FALSE,
  //               glm::value_ptr(modelMatrix));
  //           glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE,
  //               glm::value_ptr(mvpMatrix));
  //           glUniformMatrix4fv(
  //               modelViewMatrixLocation, 1, GL_FALSE,
  //               glm::value_ptr(mvMatrix));
  //           glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE,
  //               glm::value_ptr(normalMatrix));

  //           const auto &vaoRange = meshToVertexArrays[node.mesh];
  //           const auto &mesh = model.meshes[node.mesh];

  //           for (auto i = 0; i < mesh.primitives.size(); i++) {
  //             const auto &vaoPrim = vertexArrayObjects[vaoRange.begin + i];
  //             const auto &primitive = mesh.primitives[i];
  //             bindMaterial(primitive.material);
  //             glBindVertexArray(vaoPrim);
  //             if (primitive.indices >= 0) {
  //               const auto &accessor = model.accessors[primitive.indices];
  //               const auto &bufferView =
  //               model.bufferViews[accessor.bufferView]; const auto byteOffset
  //               =
  //                   accessor.byteOffset + bufferView.byteOffset;
  //               glDrawElements(primitive.mode, accessor.count,
  //                   accessor.componentType, (const GLvoid *)byteOffset);
  //             } else {
  //               const auto accessorIdx =
  //               (*begin(primitive.attributes)).second; const auto &accessor =
  //               model.accessors[accessorIdx]; glDrawArrays(primitive.mode, 0,
  //               accessor.count);
  //             }
  //           }
  //           glBindVertexArray(0);
  //         }
  //         for (const auto childNodeIdx : node.children) {
  //           drawNode(childNodeIdx, modelMatrix);
  //         }
  //       };

  //   // Draw the scene referenced by gltf file
  //   if (model.defaultScene >= 0) {
  //     for (const auto node : model.scenes[model.defaultScene].nodes) {
  //       drawNode(node, glm::mat4(1));
  //     }
  //   }
  // };

  // Uniform variable for light
  glm::vec3 lightDirection(1.f, 1.f, 1.f);
  glm::vec3 lightIntensity(1.f, 1.f, 1.f);
  glm::vec3 color = {1.f, 1.f, 1.f};
  float theta = 1.f;
  float phi = 1.f;
  bool lightCam = false;

  // Uniform variable for occlusion
  bool occlusionState = true;

  // if (!m_OutputPath.empty()) {
  //   const auto numComponents = 3;
  //   std::vector<unsigned char> pixels(
  //       m_nWindowWidth * m_nWindowHeight * numComponents);
  //   renderToImage(
  //       m_nWindowWidth, m_nWindowHeight, numComponents, pixels.data(), [&]()
  //       {
  //         const auto camera = cameraController->getCamera();
  //         drawScene(
  //             camera, lightDirection, lightIntensity, lightCam,
  //             occlusionState);
  //       });
  //   flipImageYAxis(
  //       m_nWindowWidth, m_nWindowHeight, numComponents, pixels.data());

  //   // Write png on disk
  //   const auto strPath = m_OutputPath.string();
  //   stbi_write_png(
  //       strPath.c_str(), m_nWindowWidth, m_nWindowHeight, 3, pixels.data(),
  //       0);

  //   return 0; // Exit, in that mode we don't want to run interactive
  //             // viewer
  // }

  // Loop until the user closes the window
  for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose();
      ++iterationCount) {
    const auto seconds = glfwGetTime();

    const auto camera = cameraController->getCamera();
    // drawScene(camera, lightDirection, lightIntensity, lightCam,
    // occlusionState);

    // GUI code:
    imguiNewFrame();

    {
      ImGui::Begin("GUI");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("eye: %.3f %.3f %.3f", camera.eye().x, camera.eye().y,
            camera.eye().z);
        ImGui::Text("center: %.3f %.3f %.3f", camera.center().x,
            camera.center().y, camera.center().z);
        ImGui::Text(
            "up: %.3f %.3f %.3f", camera.up().x, camera.up().y, camera.up().z);

        ImGui::Text("front: %.3f %.3f %.3f", camera.front().x, camera.front().y,
            camera.front().z);
        ImGui::Text("left: %.3f %.3f %.3f", camera.left().x, camera.left().y,
            camera.left().z);

        static int cameraControllerType = 0;
        const auto cameraControllerTypeChanged =
            ImGui::RadioButton("Trackball", &cameraControllerType, 0) ||
            ImGui::RadioButton("First Person", &cameraControllerType, 1);

        if (cameraControllerTypeChanged) {
          const auto currentCamera = cameraController->getCamera();
          if (cameraControllerType == 0) {
            cameraController = std::make_unique<TrackballCameraController>(
                m_GLFWHandle.window(), 0.5f * maxDistance);
          } else {
            cameraController = std::make_unique<FirstPersonCameraController>(
                m_GLFWHandle.window(), 0.5f * maxDistance);
          }
          cameraController->setCamera(currentCamera);
        }

        if (ImGui::Button("CLI camera args to clipboard")) {
          std::stringstream ss;
          ss << "--lookat " << camera.eye().x << "," << camera.eye().y << ","
             << camera.eye().z << "," << camera.center().x << ","
             << camera.center().y << "," << camera.center().z << ","
             << camera.up().x << "," << camera.up().y << "," << camera.up().z;
          const auto str = ss.str();
          glfwSetClipboardString(m_GLFWHandle.window(), str.c_str());
        }
      }

      ImGui::CollapsingHeader("Shader");
      static int shaderControllerType = 0;
      const auto shaderControllerTypeChanged =
          ImGui::RadioButton("Normal", &shaderControllerType, 0) ||
          ImGui::RadioButton("Diffuse Directional", &shaderControllerType, 1) ||
          ImGui::RadioButton("PBR Directional", &shaderControllerType, 2) ||
          ImGui::RadioButton("Metallic", &shaderControllerType, 3) ||
          ImGui::RadioButton("Normal Map", &shaderControllerType, 4);

      if (shaderControllerTypeChanged) {
        if (shaderControllerType == 0) {
          glslProgram = compileProgram({m_ShadersRootPath / "forward.vs.glsl",
              m_ShadersRootPath / "normals.fs.glsl"});
        } else if (shaderControllerType == 1) {
          glslProgram = compileProgram({m_ShadersRootPath / "forward.vs.glsl",
              m_ShadersRootPath / "diffuse_directional_light.fs.glsl"});
        } else if (shaderControllerType == 2) {
          glslProgram =
              compileProgram({m_ShadersRootPath / "forward_v2.vs.glsl",
                  m_ShadersRootPath / "pbr_directional_light.fs.glsl"});
        } else if (shaderControllerType == 3) {
          glslProgram = compileProgram({m_ShadersRootPath / "forward.vs.glsl",
              m_ShadersRootPath / "metallic_BRDF.fs.glsl"});
        } else if (shaderControllerType == 4) {
          glslProgram =
              compileProgram({m_ShadersRootPath / "forward_v2.vs.glsl",
                  m_ShadersRootPath / "normal_map.fs.glsl"});
        }
        getUniform(glslProgram);
        glslProgram.use();
      }

      ImGui::CollapsingHeader("Light");
      const auto lightDirChange =
          ImGui::SliderFloat("Theta", &theta, 0, M_PI) ||
          ImGui::SliderFloat("Phi", &phi, 0, M_PI * 2);
      if (lightDirChange) {
        lightDirection = glm::vec3(glm::sin(theta) * glm::cos(phi),
            glm::cos(phi), glm::sin(theta) * glm::sin(phi));
      }

      static glm::vec3 lightColor(1.f, 1.f, 1.f);
      static float lightIntensityFactor = 1.f;

      if (ImGui::ColorEdit3("Color", (float *)&lightColor) ||
          ImGui::InputFloat("Intensity", &lightIntensityFactor)) {
        lightIntensity = lightColor * lightIntensityFactor;
      }

      ImGui::Checkbox("Light from camera", &lightCam);

      ImGui::Checkbox("Occlusion", &occlusionState);

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
    uint32_t height, const std::string &vertexShader,
    const std::string &fragmentShader) :
    m_nWindowWidth(width),
    m_nWindowHeight(height),
    m_AppPath{appPath},
    m_AppName{m_AppPath.stem().string()},
    m_ImGuiIniFilename{m_AppName + ".imgui.ini"},
    m_ShadersRootPath{m_AppPath.parent_path() / "shaders"}
{
  if (!vertexShader.empty()) {
    m_vertexShader = vertexShader;
  }

  if (!fragmentShader.empty()) {
    m_fragmentShader = fragmentShader;
  }

  ImGui::GetIO().IniFilename =
      m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows
                                  // positions in this file

  glfwSetKeyCallback(m_GLFWHandle.window(), keyCallback);

  printGLVersion();
}