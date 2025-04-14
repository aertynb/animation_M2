#pragma once

#include <glm/ext.hpp>

#include "../utils/uniform.hpp"
#include "Link.hpp"

struct Vertex {
  glm::vec3 _pos;
  // Add missing attributes required by your shader
  glm::vec3 _normal;
  glm::vec2 _texCoords;
};

class Engine
{
public:
  Engine() :
      _pmat1{glm::vec3(-1, 0, 1)},
      _pmat2{glm::vec3(1, 0, 1)},
      _l{_pmat1, _pmat2, 1.f}
  {
    // Create vertices with all required attributes
    std::vector<Vertex> vertices {
      {_pmat1.position(), glm::vec3(0, 1, 0), glm::vec2(0, 0)}, 
      {_pmat2.position(), glm::vec3(0, 1, 0), glm::vec2(1, 0)}
    };
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position attribute (layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, _pos));
    glEnableVertexAttribArray(0);
    
    // Normal attribute (layout 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, _normal));
    glEnableVertexAttribArray(1);
    
    // TexCoord attribute (layout 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, _texCoords));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void update() { _l.update(); }

  void draw(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const UniformHandler& handler) 
  {
    update();
    const auto mvMatrix = viewMatrix * glm::mat4(1.f);
    const auto mvpMatrix = projMatrix * mvMatrix;
    const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));
    
    glBindVertexArray(vao);
    
    glUniformMatrix4fv(
        handler.uModelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(
        handler.uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(
        handler.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    
    // Fix 1: Use GL_POINTS instead of GL_POINT
    // Fix 2: Set the point size in OpenGL
    glPointSize(10.0f); // Make points larger and visible
    
    glDrawArrays(GL_POINTS, 0, 2);
    
    glBindVertexArray(0);
  }

private:
  PMat _pmat1, _pmat2;
  Link _l;
  GLuint vbo, vao;
};