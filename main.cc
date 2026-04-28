#include "tiny_obj_loader.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <math.h>

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

void afficherMesh(Mesh& mesh) {
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);

    for (uint32_t i = 0; i < mesh.vertexCount; i++) {
        float x = mesh.vertices[i].position.x;
        float y = mesh.vertices[i].position.y;
        float z = mesh.vertices[i].position.z;

        glColor3f(fabs(x), fabs(y), fabs(z));
        
        glVertex3f(x, y, z);
    }

    glEnd();
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
    glTranslatef(0.0f, 0.0f, -5.0f);
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

    MeshGlobal = chargerModele("C:/Users/thoma/Downloads/6e48z1kc7r40-bugatti/bugatti/bugatti.obj");

    glutDisplayFunc(affichage);
    glutMainLoop();

    libererMesh(MeshGlobal);

    return 0;
}