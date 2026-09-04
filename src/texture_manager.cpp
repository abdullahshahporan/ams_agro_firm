#include "texture_manager.h"

#include <stb_image.h>

#include <array>
#include <iostream>

TextureManager::TextureManager()
{
    stbi_set_flip_vertically_on_load(true);
    bool loaded = false;

    textures_.grass = loadTexture("textures/grass.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.soil = loadTexture("textures/soil.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.wood = loadTexture("textures/wood.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.brick = loadTexture("textures/brick.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.roof = loadTexture("textures/roof.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.metal = loadTexture("textures/metal.png", GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.concrete = loadTexture("textures/concrete.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.hay = loadTexture("textures/hay.png", GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.bark = loadTexture("textures/bark.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
    textures_.leaves = loadTexture("textures/leaves.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, loaded);
    allFilesLoaded_ &= loaded;
}

TextureManager::~TextureManager()
{
    const std::array<unsigned int, 10> textureIds = {
        textures_.grass, textures_.soil, textures_.wood, textures_.brick, textures_.roof,
        textures_.metal, textures_.concrete, textures_.hay, textures_.bark, textures_.leaves
    };
    glDeleteTextures(static_cast<GLsizei>(textureIds.size()), textureIds.data());
}

unsigned int TextureManager::loadTexture(
    const char* path,
    GLint wrapS,
    GLint wrapT,
    GLint minFilter,
    GLint magFilter,
    bool& loadedFromFile)
{
    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    int width = 0;
    int height = 0;
    int channelCount = 0;
    unsigned char* imageData = stbi_load(path, &width, &height, &channelCount, 0);
    loadedFromFile = imageData != nullptr;

    if (imageData != nullptr)
    {
        GLenum format = GL_RGB;
        if (channelCount == 1)
            format = GL_RED;
        else if (channelCount == 2)
            format = GL_RG;
        else if (channelCount == 4)
            format = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(imageData);
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << "\nReason: "
                  << stbi_failure_reason() << "\nUsing a visible fallback texture.\n";
        constexpr unsigned char fallback[] = {
            255, 0, 255,   30, 30, 30,
             30,30, 30,  255, 0,255
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, fallback);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}
