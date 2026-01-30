#include "mesh.hpp"
#include "mesh_buffer_factory.hpp"
#include <GL/glew.h>

bool Mesh::configure(void* context) {
    meshBuffer = MeshBufferFactory::create(api, context);
    if (!meshBuffer) return false;
    
    bool result = meshBuffer->createBuffers(vertices, normals);

    if (api == GraphicsAPI::OPENGL && result) {
        glEnable(GL_DEPTH_TEST);
    }
    
    return result;
}
