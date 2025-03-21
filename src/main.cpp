#include "fmepch.h"
#include "core/Macros.h"
#include "core/Application.h"

int main() {
	FMEditor::Log::Init();
	FME_INFO("Log system initialized.");
	std::unique_ptr<FMEditor::Application> app;
	app.reset(new FMEditor::Application);
	app->Run();
}