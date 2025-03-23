#include "fmepch.h"
#include "core/Macros.h"
#include "core/Application.h"
#include "renderer/ViewportLayer.h"
#include "core/logger/LogLayer.h"

int main() {
	FMEditor::Log::Init();


	// create app
	std::unique_ptr<FMEditor::Application> app;
	app.reset(new FMEditor::Application);


	// enable viewport //-----------------------------------------------
	FMEditor::ViewportLayer* viewport = new FMEditor::ViewportLayer();
	app->PushLayer(viewport);

	// enable log window //-----------------------------------------------
	FMEditor::LogLayer* logger = FMEditor::LogLayer::Get();
	app->PushLayer(logger);

	app->Run();
}