#include "Texture.h"

/*
	stb_image replaces SOIL, which was a Windows-only prebuilt binary.
	This is the only translation unit that pulls in the implementation.
*/
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



Texture* CreateTexture(std::string File) {
    unsigned int texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height;
    int channels;
    // 4 = force RGBA, what SOIL_LOAD_RGBA used to do
    unsigned char* image = stbi_load(File.c_str(), &width, &height, &channels, 4);
    if (image == NULL) {
        std::cout << "[Texture] Fail read " << File << std::endl;
        glDeleteTextures(1, &texture);
        return nullptr;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return new Texture(texture, width, height);
}

Texture::Texture(unsigned int id, int width, int height) : id(id), width(width), height(height) {}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}
