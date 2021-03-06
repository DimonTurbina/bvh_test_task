/* Mesh.cpp
 * Author : 
 * Mesh class methods definition
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Renderer/Geometry/Mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
    std::vector<Texture>& textures, glm::vec4 color) 
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)),
        pos(0), drawMode(GL_LINE_LOOP), color(std::move(color)), shaderId(0) {
    setupMesh();
}

void Mesh::draw(const ShaderProgram& shader) {
    shader.use();
    /// todo: manage textures
    
    shader.setVec4(color, std::string("vertexCol"));
    shader.setMat4(glm::mat4(1.0), std::string("model"));
    shader.setBool(countShades, std::string("countDiffuse"));

    if (drawMode == GL_LINE_STRIP) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBindVertexArray(VAO);
    glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    // 3 coordinates of type float, not normalized
    // stride is either the step, which is the size of each package, or 0:
    // then GL understands, that the data is tightly packed and defines the stride itself
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}


void Mesh::setDrawMode(int mode) {
    drawMode = mode;
}

void Mesh::setShader(int shaderId) {
    this->shaderId = shaderId;
}

void Mesh::setColor(glm::vec4 col) {
    color = std::move(col);
}

void Mesh::setShadeMode(bool countShades) {
    this->countShades = countShades;
}

int Mesh::getShaderId() {
    if (shaderId < 0)
        return 0;
    return shaderId;
}

Mesh::~Mesh() {
    // shouldn`t the order be reversed?
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

Vertex::Vertex(vec3 p, vec3 n, vec2 t) 
    : Position(p), Normal(n), TexCoords(t) {}
