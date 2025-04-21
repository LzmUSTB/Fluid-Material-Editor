#pragma once
#include <glad/glad.h>
#include "renderer/Texture.h"

namespace FMEditor {

	class OpenGL_Texture : public Texture
	{
	public:
		OpenGL_Texture(uint32_t width, uint32_t height, bool rgba, bool isFloat, bool depth);
		~OpenGL_Texture();

		uint32_t GetID() override { return m_ID; }
		void Bind(uint32_t index = 0) override;
	private:
		GLuint m_ID;
	};

}


