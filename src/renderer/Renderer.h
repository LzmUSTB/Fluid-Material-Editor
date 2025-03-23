#pragma once
#include "core/Macros.h"

namespace FMEditor {

	class Renderer
	{
	public:
		virtual ~Renderer() = default;
		virtual void Setup(int width, int height) = 0;
		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual void Submit() = 0;
		virtual unsigned int  GetRenderTexture() = 0;

		static Scope<Renderer> Create();
	};

}


