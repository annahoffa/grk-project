#include <iostream>
#include "skybox.h"

namespace grk {

    unsigned int loadTexture(const char* path);
    unsigned int loadCubemap(std::vector<std::string> faces);

    // global variables for this file
    unsigned int skyboxVAO, skyboxVBO;
    Shader shader;
    Shader skyboxShader;
    unsigned int cubemapTexture;

    //// settings
    //const unsigned int SCR_WIDTH = 800;
    //const unsigned int SCR_HEIGHT = 600;
    //
    //// camera
    //Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    //float lastX = (float)SCR_WIDTH / 2.0;
    //float lastY = (float)SCR_HEIGHT / 2.0;
    //bool firstMouse = true;


    void initializeSkybox(Core::Shader_Loader loader)
    {
        // build and compile shaders
        // -------------------------
        shader.load(loader, "shaders/cubemap.vert", "shaders/cubemap.frag");
        skyboxShader.load(loader, "shaders/skybox.vert", "shaders/skybox.frag");

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------

        //float cubeVertices[] = {
        //    // positions          // texture Coords
        //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        //     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        //    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        //};
        float skyboxVertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        //// cube VAO
        //unsigned int cubeVAO, cubeVBO;
        //glGenVertexArrays(1, &cubeVAO);
        //glGenBuffers(1, &cubeVBO);
        //glBindVertexArray(cubeVAO);
        //glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        //glEnableVertexAttribArray(0);
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        //glEnableVertexAttribArray(1);
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // skybox VAO
        /*unsigned int skyboxVAO, skyboxVBO;    // moved to global */
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        std::vector<std::string> faces
        {
            "textures/skybox/right.jpg",
            "textures/skybox/left.jpg",
            "textures/skybox/top.jpg",
            "textures/skybox/bottom.jpg",
            "textures/skybox/front.jpg",
            "textures/skybox/back.jpg"
        };
        cubemapTexture = loadCubemap(faces);

        // shader configuration
        // --------------------
        shader.use();
        shader.setInt("texture1", 0);

        // Render the skybox
        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);
    }

    void renderSkybox(glm::mat4 cameraMatrix, glm::mat4 projectionMatrix)
    {
        //// input
        //// -----
        //processInput(window);

        //// draw scene as normal
        //shader.use();
        //glm::mat4 model = glm::mat4(1.0f);
        //glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //shader.setMat4("model", model);
        //shader.setMat4("view", view);
        //shader.setMat4("projection", projection);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(cameraMatrix)));
        skyboxShader.setMat4("projection", projectionMatrix);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

    void deleteSkybox()
    {
        // optional: de-allocate all resources once they've outlived their purpose:
        /*glDeleteVertexArrays(1, &cubeVAO);*/
        glDeleteVertexArrays(1, &skyboxVAO);
        /*glDeleteBuffers(1, &cubeVBO);*/
        glDeleteBuffers(1, &skyboxVAO);
    }


    // // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // // ---------------------------------------------------------------------------------------------
    // void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    // {
    //     // make sure the viewport matches the new window dimensions; note that width and 
    //     // height will be significantly larger than specified on retina displays.
    //     glViewport(0, 0, width, height);
    // }


    // utility function for loading a 2D texture from file
    // ---------------------------------------------------
    unsigned int loadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }


    // loads a cubemap texture from 6 individual texture faces, aka creates the skybox
    // order:
    // +X (right)
    // -X (left)
    // +Y (top)
    // -Y (bottom)
    // +Z (front) 
    // -Z (back)
    // -------------------------------------------------------
    unsigned int loadCubemap(std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        // Creates a cubemap
        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
}