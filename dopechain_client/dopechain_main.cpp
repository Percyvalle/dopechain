#include "dopechain_app.hpp"


int main(int argc, char** argv) {

	DopechainApp app{argc, argv};
	
	if (!app.Initialize()) {
		getchar();
		return EXIT_FAILURE;
	}

	spdlog::info("Initialize success");
	while (app.IsWorking()) {
		app.Update();
	}

	getchar();

	return EXIT_SUCCESS;
}