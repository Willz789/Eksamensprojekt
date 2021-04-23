#include "StaticBody.h"

using namespace DirectX;

StaticBody::StaticBody(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR pos, DirectX::FXMVECTOR rot) :
	Body(std::move(pShape), pos, rot) {}

void StaticBody::move(DirectX::FXMVECTOR translation) {}

float StaticBody::getInvMass() const {
	return 0.0f;
}


