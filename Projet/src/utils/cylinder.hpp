#pragma once

#include "glad/glad.h"
#include "uniform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <cmath>

struct CylinderVertex
{
    CylinderVertex(const glm::vec3 &pos, const glm::vec3 &norm, const glm::vec2 &tex)
        : position{pos}, normal{norm}, texCoords{tex} {}

    static unsigned long sizeOfVertex()
    {
        return sizeof(position) + sizeof(normal) + sizeof(texCoords);
    }

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class CylinderCustom
{
public:
    CylinderCustom(GLfloat radius, GLfloat height, GLuint segments) : m_nVertexCount(0)
    {
        build(radius, height, segments);
        initObj(0, 1, 2);
    }

    const CylinderVertex *getDataPointer() const { return &m_Vertices[0]; }
    GLsizei getVertexCount() const { return m_nVertexCount; }
    const unsigned long getVertexSize() const { return CylinderVertex::sizeOfVertex(); }

    void draw(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix,
              const glm::mat4 &projMatrix, const UniformHandler &handler)
    {
        const auto mvMatrix = viewMatrix * modelMatrix;
        const auto mvpMatrix = projMatrix * mvMatrix;
        const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));
        glUniformMatrix4fv(handler.uModelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(handler.uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
        glUniformMatrix4fv(handler.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, getVertexCount());
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    void build(GLfloat radius, GLfloat height, GLuint segments)
    {
        GLfloat halfHeight = height / 2.0f;
        GLfloat angleStep = 2.0f * M_PI / segments;

        // Generate vertices for the cylinder sides
        for (GLuint i = 0; i < segments; ++i)
        {
            GLfloat angle = i * angleStep;
            GLfloat nextAngle = (i + 1) * angleStep;
            
            glm::vec3 p1(radius * cos(angle), -halfHeight, radius * sin(angle));
            glm::vec3 p2(radius * cos(nextAngle), -halfHeight, radius * sin(nextAngle));
            glm::vec3 p3(radius * cos(nextAngle), halfHeight, radius * sin(nextAngle));
            glm::vec3 p4(radius * cos(angle), halfHeight, radius * sin(angle));
            
            glm::vec3 normal1 = glm::normalize(glm::vec3(cos(angle), 0.0f, sin(angle)));
            glm::vec3 normal2 = glm::normalize(glm::vec3(cos(nextAngle), 0.0f, sin(nextAngle)));

            m_Vertices.emplace_back(p1, normal1, glm::vec2(i / float(segments), 0.0f));
            m_Vertices.emplace_back(p2, normal2, glm::vec2((i + 1) / float(segments), 0.0f));
            m_Vertices.emplace_back(p3, normal2, glm::vec2((i + 1) / float(segments), 1.0f));

            m_Vertices.emplace_back(p1, normal1, glm::vec2(i / float(segments), 0.0f));
            m_Vertices.emplace_back(p3, normal2, glm::vec2((i + 1) / float(segments), 1.0f));
            m_Vertices.emplace_back(p4, normal1, glm::vec2(i / float(segments), 1.0f));
        }

        m_nVertexCount = m_Vertices.size();
    }

    void initVaoPointer(GLuint vPos, GLuint vNorm, GLuint vTex)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(vPos);
        glEnableVertexAttribArray(vNorm);
        glEnableVertexAttribArray(vTex);
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, getVertexSize(),
                              (GLvoid *)offsetof(CylinderVertex, position));
        glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, getVertexSize(),
                              (GLvoid *)offsetof(CylinderVertex, normal));
        glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, getVertexSize(),
                              (GLvoid *)offsetof(CylinderVertex, texCoords));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initVboPointer()
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, getVertexCount() * getVertexSize(),
                     getDataPointer(), GL_STATIC_DRAW);
    }

    void initObj(GLuint vPos, GLuint vNorm, GLuint vTex)
    {
        initVboPointer();
        initVaoPointer(vPos, vNorm, vTex);
    }

    std::vector<CylinderVertex> m_Vertices;
    GLsizei m_nVertexCount;
    GLuint vbo;
    GLuint vao;
};
