#include "application.hpp"

int main(void) {
	Application application;
	const int exitStatus = application.startApplication(1280, 720);
	exit(exitStatus);
}
