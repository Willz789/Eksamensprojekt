#pragma once

#include "Application.h"
#include "Fire.h"

class PolyTank : public Application {
public:
	PolyTank();

	void update(float t, float dt) override;
	void render() override;

private:
	Scene scene;
	Menu menu;
	Fire fire;
};

