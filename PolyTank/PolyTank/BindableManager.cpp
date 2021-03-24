#include "BindableManager.h"

BindableManager::BindableManager(Graphics& gfx) : gfx(gfx) {}

void BindableManager::reset() {
	bindables.clear();
}
