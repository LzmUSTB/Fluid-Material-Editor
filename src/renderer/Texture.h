#pragma once
#include "core/Macros.h"

namespace FMEditor {
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetID() = 0;
		virtual void Bind(uint32_t index=0) = 0;

		static Scope<Texture> Create(uint32_t width, uint32_t height, bool rgba, bool isFloat, bool depth=false);
	};

}
