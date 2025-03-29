#pragma once
#include <entt/entity/registry.hpp>


// TODO: currently not in use
namespace FMEditor {

	class Entity
	{
	public:
		Entity(std::string name, entt::entity entity);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			T& component = registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			return component;
		}

	private:
		entt::entity m_EntityHandle{ entt::null };
		std::string m_DebugName;
	};



}

