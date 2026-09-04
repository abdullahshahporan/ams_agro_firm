#ifndef AMS_AGRO_FARM_TEXTURE_MANAGER_H
#define AMS_AGRO_FARM_TEXTURE_MANAGER_H

#include <glad/glad.h>

struct FarmTextures
{
    unsigned int grass{0};
    unsigned int soil{0};
    unsigned int wood{0};
    unsigned int brick{0};
    unsigned int roof{0};
    unsigned int metal{0};
    unsigned int concrete{0};
    unsigned int hay{0};
    unsigned int bark{0};
    unsigned int leaves{0};
};

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    const FarmTextures& farm() const { return textures_; }
    bool allFilesLoaded() const { return allFilesLoaded_; }

private:
    unsigned int loadTexture(
        const char* path,
        GLint wrapS,
        GLint wrapT,
        GLint minFilter,
        GLint magFilter,
        bool& loadedFromFile);

    FarmTextures textures_;
    bool allFilesLoaded_{true};
};

#endif
