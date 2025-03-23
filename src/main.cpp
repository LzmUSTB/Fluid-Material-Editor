#include "fmepch.h"
#include "core/Macros.h"
#include "core/Application.h"
#include "renderer/ViewportLayer.h"

int main() {
	FMEditor::Log::Init();
	std::unique_ptr<FMEditor::Application> app;
	app.reset(new FMEditor::Application);
	FMEditor::ViewportLayer* viewport = new FMEditor::ViewportLayer();
	app->PushLayer(viewport);
	app->Run();
}