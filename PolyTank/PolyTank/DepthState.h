#pragma once

#include "IBindable.h"
#include "Util.h"

class DepthState : public IBindable {
public:
	DepthState(Graphics& gfx, bool writeDepth);

	void bind(Graphics& gfx) override;

	static std::string uid(bool writeDepth);

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pState;

};

