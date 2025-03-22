#pragma once

namespace FMEditor {

	// abstract class
	class Layer
	{
	public:
		Layer::Layer(const std::string& name) :m_DebugName(name) {}

		// selectable
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent() {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}


