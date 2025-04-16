#pragma once

#include <algorithm>
#include <glm/ext.hpp>

#include "../utils/uniform.hpp"
#include "Link.hpp"

struct Vertex_PMat {
  glm::vec3 _pos;
};

struct Vertex_Link {
  glm::vec3 start, end;
};

class Engine
{
public:
  Engine() :
      // _pmat1{glm::vec3(-1, 0, 1)},
      // _pmat2{glm::vec3(1, 0, 1)},
      _pmats{PMat{1.0f, glm::vec3(0), glm::vec3(1), PMat::Model::FIXE}, PMat{1.0f, glm::vec3{1, 1, 0}, glm::vec3{1}, PMat::Model::LEAP_FROG}}
      // _l{_pmat1, _pmat2, 1.f}
  {
    // Create vertices_pmat with all required attributes
    
    glGenBuffers(1, &vbo_pmat);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);

    glBufferData(GL_ARRAY_BUFFER, _pmats.size() * sizeof(PMat), _pmats.data(), GL_STATIC_DRAW);

    // glGenBuffers(1, &vbo_link);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_link);

    // glBufferData(GL_ARRAY_BUFFER, vertices_pmat.size() * sizeof(Vertex_PMat), vertices_pmat.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position attribute (layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PMat), 
                         (void*)offsetof(PMat, _pos));
    glEnableVertexAttribArray(0);
    
    // // Normal attribute (layout 1)
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PMat), 
    //                      (void*)offsetof(Vertex_PMat, _normal));
    // glEnableVertexAttribArray(1);
    
    // // TexCoord attribute (layout 2)
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PMat), 
    //                      (void*)offsetof(Vertex_PMat, _texCoords));
    // glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (auto i = 0; i < _pmats.size() - 1; ++i) {
      auto& pmat1 = _pmats[i];
      auto& pmat2 = _pmats[i+1];
      _links.push_back(Link{pmat1, pmat2, 2.f});
    }
  }

  void update() 
  { 
    for (auto& link : _links) {
      link.updateHook();
    }

    for (auto& pmat : _pmats) {
      pmat.update(0.0001);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);

    glBufferData(GL_ARRAY_BUFFER, _pmats.size() * sizeof(PMat), _pmats.data(), GL_STATIC_DRAW);
    // std::vector<Vertex_PMat> vertices(_pmats.size());
    // std::transform(_pmats.begin(), _pmats.end(),
    //               std::back_inserter(vertices),
    //               [](const PMat& pmat) {return Vertex_PMat{pmat.position()};});

    // std::cout << vertices[0]._pos << std::endl;

    // glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);

    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_PMat), vertices.data(), GL_STATIC_DRAW);
  }

  void draw(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const UniformHandler& handler) 
  {
    update();
    // for (auto& pmat : _pmats) {
    const auto mvMatrix = viewMatrix * glm::mat4(1.f);
    const auto mvpMatrix = projMatrix * mvMatrix;
    const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pmat);
    glBindVertexArray(vao);
    
    glUniformMatrix4fv(
        handler.uModelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(
        handler.uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(
        handler.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    
    glPointSize(10.0f); // Make points larger and visible
    
    glDrawArrays(GL_POINTS, 0, 2);

    glLineWidth(3.0f);  // Optional: make the line thicker
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // }

    // for (auto & link: _links) {

    // }
  }

private:
  // void updatePositions() {
  //   for (auto &pmat : _pmats) {
  //     _positions.emplace_back(pmat.position());
  //   }   
  // }

  std::vector<PMat> _pmats;
  // std::vector<glm::vec3> _positions;
  std::vector<Link> _links;
  GLuint vbo_pmat, vbo_link, vao;
};