#pragma once

class Graphics;

class IBindable {
public:
	virtual void bind(Graphics& gfx) = 0;
};
