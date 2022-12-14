#pragma once

class Model {
protected:
    static enum pivot { ORIGIN, OBJECT };

    // Object attributes
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;
    tinyobj::attrib_t attributes;
    std::vector<GLuint> mesh_indices;
    std::vector<GLfloat> fullVertexData;
    GLintptr uvPtr = 3 * sizeof(GLfloat);

    // Texture attributes
    int img_width, img_height, color_channels;
    GLuint texture;
    int norm_width, norm_height, norm_channels;
    GLuint normTex;

    // Flags
    bool usingNormals;
    pivot pivotPoint = OBJECT;

    // Draw attributes
    GLuint VAO, VBO;
    int offset;
    glm::vec3 position, scale, rotation;
    glm::mat4 transformation;

    /* Loads object vertices from given filepath */
    void loadObj(std::string objPath) {
        // Load object from file
        bool success = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &material,
            &warning,
            &error,
            objPath.c_str()
        );
        
        // Calculate tangents and bitangents if using normals
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        if(usingNormals)
            for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
                tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
                tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
                tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

                glm::vec3 v1 = glm::vec3(
                    attributes.vertices[vData1.vertex_index * 3],
                    attributes.vertices[vData1.vertex_index * 3 + 1],
                    attributes.vertices[vData1.vertex_index * 3 + 2]);

                glm::vec3 v2 = glm::vec3(
                    attributes.vertices[vData2.vertex_index * 3],
                    attributes.vertices[vData2.vertex_index * 3 + 1],
                    attributes.vertices[vData2.vertex_index * 3 + 2]);

                glm::vec3 v3 = glm::vec3(
                    attributes.vertices[vData3.vertex_index * 3],
                    attributes.vertices[vData3.vertex_index * 3 + 1],
                    attributes.vertices[vData3.vertex_index * 3 + 2]);

                glm::vec2 uv1 = glm::vec2(
                    attributes.texcoords[vData1.texcoord_index * 2],
                    attributes.texcoords[vData1.texcoord_index * 2 + 1]
                );

                glm::vec2 uv2 = glm::vec2(
                    attributes.texcoords[vData2.texcoord_index * 2],
                    attributes.texcoords[vData2.texcoord_index * 2 + 1]
                );

                glm::vec2 uv3 = glm::vec2(
                    attributes.texcoords[vData3.texcoord_index * 2],
                    attributes.texcoords[vData3.texcoord_index * 2 + 1]
                );

                glm::vec3 deltaPos1 = v2 - v1;
                glm::vec3 deltaPos2 = v3 - v1;

                glm::vec2 deltaUV1 = uv2 - uv1;
                glm::vec2 deltaUV2 = uv3 - uv1;

                float r = 1.f / ((deltaUV1.x * deltaUV2.y) - deltaUV1.y * deltaUV2.x);

                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                tangents.push_back(tangent);
                tangents.push_back(tangent);
                tangents.push_back(tangent);

                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
            }
        
        // Assign vertex indices of mesh to array
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];

            int vertexIndex = vData.vertex_index * 3;
            int normalIndex = vData.normal_index * 3;
            int uvIndex = vData.texcoord_index * 2;

            fullVertexData.push_back(attributes.vertices[vertexIndex]);
            fullVertexData.push_back(attributes.vertices[vertexIndex + 1]);
            fullVertexData.push_back(attributes.vertices[vertexIndex + 2]);

            fullVertexData.push_back(attributes.normals[normalIndex]);
            fullVertexData.push_back(attributes.normals[normalIndex + 1]);
            fullVertexData.push_back(attributes.normals[normalIndex + 2]);

            fullVertexData.push_back(attributes.texcoords[uvIndex]);
            fullVertexData.push_back(attributes.texcoords[uvIndex + 1]);

            // Add tangents and bitangents if using normals
            if (usingNormals) {
                fullVertexData.push_back(tangents[i].x);
                fullVertexData.push_back(tangents[i].y);
                fullVertexData.push_back(tangents[i].z);

                fullVertexData.push_back(bitangents[i].x);
                fullVertexData.push_back(bitangents[i].y);
                fullVertexData.push_back(bitangents[i].z);
            }
        }
    }

    /* Loads texture from path */
    void loadTex(std::string texPath, int colorMode) {
        unsigned char* tex_bytes = stbi_load(texPath.c_str(),
            &img_width,
            &img_height,
            &color_channels,
            0);

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0); // "Layer"
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D, // Type
            0, // Index
            colorMode, // Image format (rgb = jpeg/png with alpha, rgba = png/images with alpha)
            img_width,
            img_height,
            0, // Border
            colorMode,
            GL_UNSIGNED_BYTE, // Texture data type
            tex_bytes
        );

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes);
    }

    /* Loads normal texture from path */
    void loadNorm(std::string texPath, int colorMode) {
        unsigned char* norm_bytes = stbi_load(texPath.c_str(),
            &norm_width,
            &norm_height,
            &norm_channels,
            0);

        glGenTextures(1, &normTex);
        glActiveTexture(GL_TEXTURE1); // "Layer"
        glBindTexture(GL_TEXTURE_2D, normTex);

        glTexImage2D(
            GL_TEXTURE_2D, // Type
            0, // Index
            colorMode, // Image format (rgb = jpeg/png with alpha, rgba = png/images with alpha)
            norm_width,
            norm_height,
            0, // Border
            colorMode,
            GL_UNSIGNED_BYTE, // Texture data type
            norm_bytes
        );

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(norm_bytes);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParamateri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

public:
    Model() {}

    Model(std::string objPath,
        std::string texPath, int texFormat,
        bool useNormals, std::string normPath, int normFormat,
        glm::vec3 pos, float size, glm::vec3 rot)
    {
        // Initialize flags
        usingNormals = useNormals;

        // Load object from file
        loadObj(objPath);
        // Load texture if specified
        if(!texPath.empty())
            loadTex(texPath, texFormat);
        // Load normals if specified
        if (!normPath.empty())
            loadNorm(texPath, normFormat);

        // Initialize draw vectors
        offset = 8;
        if (useNormals)
            offset = 14;
        position = pos;
        scale = glm::vec3(size);
        rotation = rot;
    }

    /* Initialize buffers for drawing */
    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Bind VAO
        glBindVertexArray(VAO);
        // Create an array buffer for vertex positions
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Add size of vertex array (bytes) and contents to buffer    
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * fullVertexData.size(),
            fullVertexData.data(),
            GL_STATIC_DRAW
        );

        // Instruct VAO how to interpret array buffer
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            offset * sizeof(GL_FLOAT),
            (void*)0
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_TRUE,
            offset * sizeof(GL_FLOAT),
            (void*)(3 * sizeof(GLfloat))
        );
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(
            2, // Tex coords
            2,
            GL_FLOAT,
            GL_FALSE,
            offset * sizeof(GL_FLOAT),
            (void*)(6 * sizeof(GLfloat))
        );
        glEnableVertexAttribArray(2);

        if (usingNormals) {
            glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                offset * sizeof(GL_FLOAT),
                (void*)(8 * sizeof(GLfloat))
            );
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(
                4,
                3,
                GL_FLOAT,
                GL_FALSE,
                offset * sizeof(GL_FLOAT),
                (void*)(11 * sizeof(GLfloat))
            );
            glEnableVertexAttribArray(4);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    /* Getters */
    bool isUsingNormals() {
        return usingNormals;
    }
    glm::vec3 getPos() {
        return position;
    }
    glm::vec3 getRotation() {
        return rotation;
    }

    /* Setters */
    void setPivotOrigin() {
        pivotPoint = ORIGIN;
    }
    void setPivotObject() {
        pivotPoint = OBJECT;
    }
    void setPosition(glm::vec3 position) {
        this->position = position;
    }
    void setRotation(glm::vec3 rotation) {
        this->rotation = rotation;
    }

    /* Methods */
    /* Draws object
    *  @param transformationLoc - uniform index to pass transformation matrix
    *  @param tex0 - uniform index to assign texture
    *  @param tex1 - uniform index to assign normals
    */
    void draw(unsigned int transformationLoc, unsigned int tex0, unsigned int tex1 = -1) {
        glBindVertexArray(VAO);

        transformation = glm::mat4(1.0f);

        // NOTE: multiplication order
        // https://stackoverflow.com/questions/52770929/rotate-object-around-origin-as-it-faces-origin-in-opengl-with-glm
        // Changes order of transformation based on pivoting on origin or object
        if (pivotPoint == ORIGIN) {
            // Rotate
            transformation = glm::rotate(
                transformation,
                glm::radians(rotation[0]),
                glm::vec3(0.f, 1.f, 0.f));
            transformation = glm::rotate(
                transformation,
                glm::radians(rotation[1]),
                glm::vec3(1.f, 0.f, 0.f));
            transformation = glm::rotate(
                transformation,
                glm::radians(rotation[2]),
                glm::vec3(0.f, 0.f, 1.f));

            // Translate
            transformation = glm::translate(transformation, position);
        }
        else {
            // Translate
            transformation = glm::translate(transformation, position);

            // Rotate
            transformation = glm::rotate(
                transformation,
                glm::radians(rotation[0]),
                glm::vec3(0.f, 1.f, 0.f));
            transformation = glm::rotate(
                transformation,
                glm::radians(rotation[1]),
                glm::vec3(1.f, 0.f, 0.f));
            transformation = glm::rotate(
                transformation,
                glm::radians(rotation[2]),
                glm::vec3(0.f, 0.f, 1.f));
        }

        // Scale
        transformation = glm::scale(transformation, scale);

        // Position object/s
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation));
        
        // Bind texture to object and draw
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0, 0);

        // If included, bind normals to object and draw
        if (tex1 != -1) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normTex);
            glUniform1i(tex1, 1);
        }

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / offset);
    }

    /* Modifies position of camera
    *  @param value - value to move XYZ position of object
    */
    void modPos(glm::vec3 value) {
        position += value;
    }

    /* Modifies rotation of camera
    *  @param newRot - value to change XYZ rotation of object
    */
    void adjustRotate(glm::vec3 newRot) {
        rotation += newRot;
    }

    /* Deletion of buffers after object use */
    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};