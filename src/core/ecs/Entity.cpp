#include "fmepch.h"
#include "Entity.h"

namespace FMEditor {
	Entity::Entity(std::string name, entt::entity entity) : m_DebugName(name), m_EntityHandle(entity)
	{
	}
}