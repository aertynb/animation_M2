#pragma once

#include <algorithm>
#include <glm/ext.hpp>
#include <memory>
#include <vector>
#include <iostream>

#include "../utils/uniform.hpp"
#include "Link.hpp"

struct Vertex_PMat {
  glm::vec3& _pos;
};

struct Vertex_Link {
  glm::vec3 start, end;
};

class Engine
{
public:
  Engine()
  {
    // Create PMats
    auto p1 = std::make_shared<PMat>(PMat::Type::FIXE, m, glm::vec3(0.5f, 0.f, 0.f), glm::vec3(0.f));
    auto p2 = std::make_shared<PMat>(PMat::Type::LEAP_FROG, m, glm::vec3(-0.5f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));

    _pmats.push_back(p1);
    _pmats.push_back(p2);

    // Create Link between them
    _links.push_back(std::make_shared<Link>(Link::Type::HOOK, p1, p2, k, z, s));

    // Create buffer to hold PMat positions
    glGenBuffers(1, &vbo_pmat);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);
    {
      std::vector<PMat> tmp_data;
      for (const auto& p : _pmats) tmp_data.push_back(*p);
      glBufferData(GL_ARRAY_BUFFER, tmp_data.size() * sizeof(PMat), tmp_data.data(), GL_DYNAMIC_DRAW);
    }

    // Setup VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PMat), (void*)offsetof(PMat, _pos));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void update() 
{ 
  // 0. Update steps
  h = 1.f / Fe;

  // 1. Clear all forces
  for (auto& pmat : _pmats) {    
    pmat->_frc += pmat->_m * g; // Apply gravity
  }

  // 2. Apply forces via links (springs, damping, etc.)
  for (auto& link : _links) {
    link->_update(); // Only apply spring forces!
  }

  // 3. Integrate motion
  for (auto& pmat : _pmats) {
    pmat->_update(h); // Now integrate with correct force
  }

  // 4. Push data to GPU
  glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);
  std::vector<PMat> tmp_data;
  for (const auto& p : _pmats) tmp_data.push_back(*p);
  glBufferData(GL_ARRAY_BUFFER, tmp_data.size() * sizeof(PMat), tmp_data.data(), GL_DYNAMIC_DRAW);
}


  void draw(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const UniformHandler& handler) 
  {
    update(); // make sure data is fresh

    glm::mat4 mvMatrix = viewMatrix * glm::mat4(1.f);
    glm::mat4 mvpMatrix = projMatrix * mvMatrix;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(mvMatrix));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);
    glBindVertexArray(vao);

    glUniformMatrix4fv(handler.uModelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(handler.uModelViewMatrix,     1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(handler.uNormalMatrix,        1, GL_FALSE, glm::value_ptr(normalMatrix));

    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(_pmats.size()));

    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(_pmats.size())); // Or draw links here later

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  
  float Fe = 100.f;
  float h = 1.f / Fe;
  float m = 1.f;
  float k = 100.f; 
  float z = 2.f * std::sqrt(k * m);
  float s = 1.2f;                // Allows for 20% stretch before ignoring the spring
  // float l0 = 0.2f;

private:
  std::vector<std::shared_ptr<PMat>> _pmats;
  std::vector<std::shared_ptr<Link>> _links;
  glm::vec3 g {0.f, -9.81f, 0.f}; // Realistic Earth gravity

  GLuint vbo_pmat = 0, vbo_link = 0, vao = 0;
};
