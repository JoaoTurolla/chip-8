#pragma once

#include <cstdint>
#include <SDL2/SDL.h>
#include <GL/gl.h>


class Platform{

	friend class Imgui;

	public:
		Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
		~Platform();
		void Update(void const* buffer, int pitch);
		bool ProcessInput(uint8_t* keys);

	private:
		SDL_Window* window{};
		SDL_GLContext glContext{};
		GLuint framebufferTexture;
		SDL_Renderer* renderer{};
		SDL_Texture* texture{};

};