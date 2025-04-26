#include "fmepch.h"
#include "core/Macros.h"
#include "core/Application.h"
#include "renderer/ViewportLayer.h"
#include "core/ecs/EntityLayer.h"
#include "core/logger/LogLayer.h"
#include "simulation/PhysicsLayer.h"

int main() {
	FMEditor::Log::Init();

	// create app
	std::unique_ptr<FMEditor::Application> app;
	app.reset(new FMEditor::Application);

	// enable log window //-----------------------------------------------
	FMEditor::LogLayer* logger = FMEditor::LogLayer::Get();
	app->PushLayer(logger);

	// Entity System //------------------------------------------------
	FMEditor::EntityLayer* entity = new FMEditor::EntityLayer();
	app->PushLayer(entity);

	// physics simulation //--------------------------------------------
	FMEditor::PhysicsLayer* physics = new FMEditor::PhysicsLayer(entity->GetRegistry());
	app->PushLayer(physics);

	// render and viewport //---------------------------------------------------
	FMEditor::ViewportLayer* viewport = new FMEditor::ViewportLayer(entity->GetRegistry());
	app->PushLayer(viewport);

	app->Run();
}