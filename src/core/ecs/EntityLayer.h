#pragma once
#include "core/Macros.h"
#include "core/Layer.h"
#include "entt/entity/registry.hpp"

namespace FMEditor {

	class EntityLayer : public Layer
	{
	public:
		EntityLayer();
		void OnImguiRender() override;


		//entt::entity AddEntity(std::string name);
		entt::registry& GetRegistry() { return m_Registry; }
	private:
		entt::registry m_Registry;
	};

}

