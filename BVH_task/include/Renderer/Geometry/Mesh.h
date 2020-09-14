/* Mesh.h
 * Author : Kozhukharov Nikita
 * Mesh class is used to draw connected object, generated by one array of vertices
 */

#pragma once

#include <Renderer/Shaders.h>
#include <Math/Vec.h>
#include <vector>
#include <string>


struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
    Vertex(vec3 p, vec3 n, vec2 t);
};

struct Texture {
    unsigned int id;
    std::string type;
};


class Mesh {
    int drawMode;
    int shaderId;
protected:
    glm::vec3 pos;
    glm::vec3 color;
    unsigned int VAO, VBO, EBO;
    void setupMesh();

    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

public:
    // vectors will be moved to avoid copying
    Mesh(std::vector<Vertex> & vertices, std::vector<unsigned int> & indices,
        std::vector<Texture> & textures, glm::vec3 color = glm::vec3(0.2, 0.4, 0.6));
    // shader is necessary to give him texture attributes
    virtual void draw(const ShaderProgram & shader);            

    void setDrawMode(int mode);
    void setShader(int shaderId);
    void setColor(glm::vec4 col);
    int getShaderId();
    ~Mesh();
};

/* */
class Box : public Mesh {
    
public:
   Box(std::vector<Vertex>& vertices, 
       std::vector<unsigned int>& indices);

   void draw(const ShaderProgram&) override;

};
