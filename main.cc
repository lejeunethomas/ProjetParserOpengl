#include "tiny_obj_loader.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct vec2 {
    float x, y;
};

struct vec3 {
    float x, y, z;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoords;
};

struct Mesh {
    Vertex* vertices;
    uint32_t vertexCount;
};

Mesh MeshGlobal;
GLuint Texture;

Mesh chargerModele(const char* filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath, nullptr, true);

    if (!warn.empty()) { std::cout << "Attention: " << warn << std::endl; }
    if (!err.empty()) { std::cerr << "Erreur: " << err << std::endl; }
    if (!ret) { exit(1); }

    std::vector<Vertex> TempVertex;

    for (size_t s = 0; s < shapes.size(); s++) {
        
        size_t index_offset = 0;
        
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Vertex vertex;

                vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                if (idx.normal_index >= 0) {
                    vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texcoords.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.texcoords.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }

                TempVertex.push_back(vertex);
            }
            index_offset += 3;
        }
    }

    Mesh Mesh;
    Mesh.vertexCount = TempVertex.size();
    
    Mesh.vertices = new Vertex[Mesh.vertexCount];
    
    for(uint32_t i = 0; i < Mesh.vertexCount; i++) {
        Mesh.vertices[i] = TempVertex[i];
    }

    return Mesh;
}

GLuint chargerTexture(const char* filepath) {
    GLuint texID = 0;
    int w, h, comp;

    uint8_t* data = stbi_load(filepath, &w, &h, &comp, STBI_rgb_alpha);

    if (data == nullptr) {
        std::cerr << "Erreur: Impossible de charger l'image texture " << filepath << std::endl;
        return 0;
    }

    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texID;
}

void afficherMesh(Mesh& mesh) {

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glBegin(GL_TRIANGLES);

    for (uint32_t i = 0; i < mesh.vertexCount; i++) {
        float x = mesh.vertices[i].position.x;
        float y = mesh.vertices[i].position.y;
        float z = mesh.vertices[i].position.z;

        glTexCoord2f(mesh.vertices[i].texcoords.x, mesh.vertices[i].texcoords.y);
        
        glNormal3f(mesh.vertices[i].normal.x, mesh.vertices[i].normal.y, mesh.vertices[i].normal.z);
        glVertex3f(x, y, z);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void libererMesh(Mesh& mesh) {
    delete[] mesh.vertices;
    mesh.vertices = nullptr;
    mesh.vertexCount = 0;
}

void affichage(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(2.0f, -5.0f, -6.0f);
    float temps = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    
    glRotatef(temps * 50.0f, 0.0f, 1.0f, 0.0f);
    afficherMesh(MeshGlobal);

    glFlush();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Affichage 3D OBJ");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1f, 1000.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    Texture = chargerTexture("");
    MeshGlobal = chargerModele("");

    glutDisplayFunc(affichage);
    glutMainLoop();

    libererMesh(MeshGlobal);

    return 0;
}