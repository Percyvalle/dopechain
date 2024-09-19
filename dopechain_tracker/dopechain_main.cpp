#include "server_impl.hpp"

int main(int argc, char** argv) {
	DopechainTracker tracker{ 20702, Utils::GetCountCPU() };

	if (!tracker.Start()) {
		spdlog::info("The server did not start!");
		return EXIT_FAILURE;
	}

	while (tracker.IsRunning()) {
		tracker.Update(false);
		tracker.CheckClientConnection();
	}

	return EXIT_SUCCESS;
}