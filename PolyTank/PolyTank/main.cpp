#include "Polytank.h"

int main() {
	PolyTank::get().run();
	
	return 0;
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
) {
	main();
}