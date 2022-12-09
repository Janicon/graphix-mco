#pragma once
class Skybox {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int tex;
    ShaderManager shader;

    glm::mat4 default_projection = glm::perspective(
        glm::radians(60.f),
        1.f,
        .1f,
        100.f
    );

    float defaultVertices[24]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };
    unsigned int defaultIndices[36]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };
    glm::vec4 filterColor;

public:
	Skybox() {
        // Creates buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(defaultVertices), &defaultVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &defaultIndices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        
        std::string faces[]{
            "Skybox/uw_ft.jpg",
            "Skybox/uw_bk.jpg",
            "Skybox/uw_up.jpg",
            "Skybox/uw_dn.jpg",
            "Skybox/uw_rt.jpg",
            "Skybox/uw_lf.jpg",
        };

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

        // Prevent pixelating
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Prevent tiling
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Load skybox images
        for (int i = 0; i < 6; i++) {
            int w, h, skyCChannel;
            stbi_set_flip_vertically_on_load(false);

            unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &skyCChannel, 0);

            if (data) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGB,
                    w,
                    h,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    data
                );

                stbi_image_free(data);
            }
        }

        stbi_set_flip_vertically_on_load(true);
        
        // Creates vertex and fragment shader for skybox
        shader = ShaderManager("skybox");

        filterColor = glm::vec4(0.05, 0.1, .5, 0.5);
	}

    /* Resets filter color to normal
    *  @param color (optional) - ovveride filter color to set
    */
    void resetFilterColor(glm::vec4 color = glm::vec4(0.05, 0.1, .5, 0.5)) {
        filterColor = color;
    }

    /* Draws skybox
    *  @param viewMatrix - view matrix of camera
    *  @param isFPP - used to flag the use of color filter
    */
    void draw(glm::mat4 viewMatrix, int isFPP) {
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        shader.useShaderProgram();

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(viewMatrix));

        shader.sendMat4("projection", default_projection);
        shader.sendMat4("view", sky_view);
        shader.sendVec4("filterColor", filterColor);
        shader.sendInt("isFPP", isFPP);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }
    
    /* Deletion of buffers after object use */
    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};