//
//  Scene.cpp
//  OpenGLTest
//
//  Created by Samuel Hall on 08/08/2016.
//
//

#include "Scene.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Viewport.hpp"

#include <GLFW/glfw3.h>

#include <OpenGL/OpenGL.h>

void Scene::initialize(nanogui::Screen *gui_screen)
{
    // Setup scene.    
    std::shared_ptr<Shader> triangle_shader(new Shader());
    
    triangle_shader->set_shader("./Shaders/triangle.vert", GL_VERTEX_SHADER);
    triangle_shader->set_shader("./Shaders/triangle.frag", GL_FRAGMENT_SHADER);
    triangle_shader->initialize();
    
    std::shared_ptr<Material> triangle_material( new Material(triangle_shader) );
    
    std::shared_ptr<Mesh> triangle_mesh(new Mesh());
    
    std::vector<GLfloat> vertices = {
        // front
        -1.0, -1.0,  1.0, 1.0,
        0.0, 0.0,
        
        1.0, -1.0,  1.0, 1.0,
        1.0, 0.0,
        
        1.0,  1.0,  1.0, 1.0,
        1.0, 1.0,
        
        -1.0,  1.0,  1.0, 1.0,
        0.0, 1.0,
        
        // back
        -1.0, -1.0, -1.0, 1.0,
        0.0, 0.0,
        
        1.0, -1.0, -1.0, 1.0,
        1.0, 0.0,
        
        1.0,  1.0, -1.0, 1.0,
        1.0, 1.0,
        
        -1.0,  1.0, -1.0, 1.0,
        0.0, 1.0
    };
    std::vector<GLuint> indices = {  // Note that we start from 0!
        // front
        0, 1, 2,
        2, 3, 0,
        // top
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // bottom
        4, 0, 3,
        3, 7, 4,
        // left
        4, 5, 1,
        1, 0, 4,
        // right
        3, 2, 6,
        6, 7, 3,
    };
    
    std::vector<unsigned int> attributes = {4, 2};
    
    triangle_mesh->initialize(vertices, attributes, indices);
    triangle_mesh->set_material(triangle_material);
    triangle_mesh->set_position( glm::vec3(0.0f,0.0f,0.0f) );
    triangle_mesh->set_scale( glm::vec3(1.0f,1.0f,1.0f) );
    
    m_root_node->add_child(triangle_mesh);
    
    // Reload shader when it's files are changed.
    m_shader_reloader = std::unique_ptr<ShaderReloader>( new ShaderReloader() );
    
    m_shader_reloader->add_files_to_watch([=]{
        
        m_renderer->queue_function_before_render([triangle_shader] {
            triangle_shader->delete_shader();
            triangle_shader->initialize();
        });
    },
                                    "./Shaders/triangle.frag",
                                    "./Shaders/triangle.vert"
                                    );
}

void Scene::draw()
{
    m_renderer->draw(m_root_node);
}

void Scene::mouse_button_callback(int button, int action, int modifiers)
{
    if(button == GLFW_MOUSE_BUTTON_1) {
        
        if(action == GLFW_PRESS) {
            is_mouse_down = true;
            first_mouse = true;
        }
        
        else
            is_mouse_down = false;
    }
}

void Scene::mouse_callback(double xpos, double ypos)
{
    // Rotate scene
    if(is_mouse_down) {
    
        if(first_mouse)
        {
            lastX = xpos;
            lastY = ypos;
            first_mouse = false;
        }
        
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
        
        lastX = xpos;
        lastY = ypos;
        
        m_camera_container->set_orientation( m_camera_container->get_orientation() * glm::angleAxis(0.05f * xoffset, glm::vec3(0.0f, 1.0f, 0.0f)) );
        m_camera_container->set_orientation( m_camera_container->get_orientation() * glm::angleAxis(0.05f * yoffset, glm::vec3(1.0f, 0.0f, 0.0f)) );
    }
}

void Scene::scroll_callback(double xoffset, double yoffset)
{
    glm::vec3 new_camera_position = m_camera->get_position() + glm::vec3(0.0f, 0.0f, yoffset);
    
    if (new_camera_position.z < 1.0f)
        new_camera_position.z = 1.0f;
    
    if (new_camera_position.z > 200.0f)
        new_camera_position.z = 200.0f;
    
    m_camera->set_position( new_camera_position );
}

void Scene::key_callback(int key, int action)
{
    if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
        m_camera_container->set_orientation( m_camera_container->get_orientation() * glm::angleAxis(0.5f, glm::vec3(1.0f, 0.0f, 0.0f)) );
    
    else if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
        m_camera_container->set_orientation( m_camera_container->get_orientation() * glm::angleAxis(0.5f, glm::vec3(-1.0f, 0.0f, 0.0f)) );
    
    else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE)
        m_camera_container->set_orientation( m_camera_container->get_orientation() * glm::angleAxis(0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) );
    
    else if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE)
        m_camera_container->set_orientation( m_camera_container->get_orientation() * glm::angleAxis(0.5f, glm::vec3(0.0f, -1.0f, 0.0f)) );
    
    else if(key == GLFW_KEY_R && action == GLFW_PRESS)
        m_camera_container->set_orientation(glm::quat());
}
