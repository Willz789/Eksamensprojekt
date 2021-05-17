#pragma once

#include "IBindable.h"
#include "Util.h"


class BlendState : public IBindable {
public:
	enum class Mode {
		OPAQUE, ADD, BLEND
	};

public:
	BlendState(Graphics& gfx, Mode mode);

	void bind(Graphics& gfx) override;

	static std::string uid(Mode mode);

private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> pState;

};

