#include "server_impl.hpp"

int main(int argc, char** argv) {
	DopechainTracker tracker{ 20702, Utils::GetCountCPU() };

	tracker.Start();

	while (true) {
		tracker.Update(false);
		tracker.CheckClientConnection();
	}

	return EXIT_SUCCESS;
}