#include "Enemy.h"
#include "PolyTank.h"

#include <random>
#include <chrono>
#include <thread>

using namespace DirectX;

Enemy::Enemy(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Tank* pTarget, Level& lvl, Node startNode) :
	shootingCooldown(1.0f),
	shootingRange(12.0f),
	pTarget(pTarget)
{
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, lvl.worldPos(startNode));
	pLvl = &lvl;
}

Enemy::~Enemy()
{
	if (pathBuilder.joinable()) {
		pathBuilder.join();
	}

	PolyTank::get().deleteGameObject(pTank);
}

void Enemy::update(float dt)
{
	aim();
	shootingCooldown -= dt;
	if (shootingCooldown < 0.0f) {
		pTank->shoot(PolyTank::get().getGfx(), PolyTank::get().getPcs(), 20.0f);
		shootingCooldown = 1.0f;
	}
	
	if (!pathLocked) {
		if (!path.empty()) {
			move(dt);
		}
		else {
			if (pathBuilder.joinable()) {
				pathBuilder.join();
			}

			pathBuilder = std::thread([this]() -> void {
				buildPath();
			});
		}
	}
}

void Enemy::move(float dt)
{
	XMVECTOR pos = pTank->getGroundPosition();
	XMVECTOR target = pLvl->worldPos(path.back());
	XMVECTOR diff = target - pos;
	XMVECTOR diffLength = XMVector3Length(diff);

	XMVECTOR targetDir = diff / diffLength;
	XMVECTOR dir = -pTank->bodyToWorld().r[2];

	float rotateAxisY = XMVectorGetZ(targetDir) * XMVectorGetX(dir) - XMVectorGetX(targetDir) * XMVectorGetZ(dir);
	if (fabsf(rotateAxisY) < 0.1f) {
		if (XMVectorGetX(targetDir) * XMVectorGetX(dir) + XMVectorGetZ(targetDir) * XMVectorGetZ(dir) > 0.0f) {
			pTank->driveForward(dt);
		} 
		else {
			pTank->driveBackward(dt);
		}
	}
	else if (rotateAxisY > 0.0f) {
		pTank->turnRight(dt);
	}
	else {
		pTank->turnLeft(dt);
	}

	if (XMVectorGetX(diffLength) < 0.5f) {
		path.pop_back();
	}

}

void Enemy::aim()
{
	XMVECTOR diff = XMVector4Transform(pTarget->getPosition(), XMMatrixInverse(nullptr, pTank->bodyToWorld()));

	float yaw = atan2f(XMVectorGetX(diff), XMVectorGetZ(diff));

	diff -= XMVectorSet(0.8f * cosf(yaw), 0.1f, 0.8f * sinf(yaw), 0.0f);

	float y = XMVectorGetY(diff);
	float x = XMVectorGetX(XMVector2Length(XMVectorSwizzle<0, 2, 3, 3>(diff)));

	float g = Physics::g;
	float v = 20.0f;
	float v2 = v * v;

	float sqrt1 = sqrtf(-g*g*x*x - 2.0f*g*v2*y + v2*v2);
	float sqrt2 = sqrtf((-g * y + v2 + sqrt1) / (x * x + y * y));

	float vy = (y * (-g * y + v2 + sqrt1) / (x * x + y * y) + g) / (v * sqrt2);
	float vx = x * sqrt2 / v;

	float pitch1 = atan2(vy, vx);
	float pitch2 = atan2(-0.5f * vy, -0.5f * vx);

	float pitch = pitch2;
	
	if (isnan(pitch)) {
		pitch = -pi;
	}

	pTank->setTurretRotation(yaw + pi, pitch + pi);

}

struct NodeData {
	Node pos;
	float shortestDist;
	bool visited;
};

void Enemy::buildPath()
{
	pathLocked = true;

	uint32_t w = pLvl->getW();
	uint32_t d = pLvl->getD();
	uint32_t h = pLvl->getH();

	auto index = [w, d, h](const Node& n) -> size_t {
		return n.i * w * d + n.j * w + n.k;
	};

	path.clear();

	static std::random_device rd;
	static std::default_random_engine rng(rd());
	std::uniform_real_distribution uDist(0.0f, 2.0f * pi);

	float angle = uDist(rng);
	XMVECTOR targetOffset = 4.0f * XMVectorSet(cos(angle), 0.0f, sin(angle), 0.0f);

	XMVECTOR initialPos = pTank->getPosition();
	XMVECTOR targetPos = pTarget->getGroundPosition() + targetOffset;

	Node initial = pLvl->findNearestNode(initialPos);
	Node current = initial;
	Node target = pLvl->findNearestNode(targetPos);
	
	std::vector<NodeData> nodes;
	std::vector<Node> prevNode;

	nodes.reserve(h * d * w);
	prevNode.resize(h * d * w);
	for (uint32_t i = 0; i < h; i++) {
		for (uint32_t j = 0; j < d; j++) {
			for (uint32_t k = 0; k < w; k++) {
				NodeData n;
				n.pos.i = i;
				n.pos.j = j;
				n.pos.k = k;
				n.shortestDist = std::numeric_limits<float>::infinity();
				n.visited = false;

				nodes.push_back(n);
			}
		}
	}
	
	nodes[index(initial)].shortestDist = 0.0f;
	
	bool foundNode = true;
	while (foundNode) {
		size_t col = index(current);

		for (size_t row = 0; row < w * h * d; row++) {

			if (!nodes[row].visited && pLvl->hasEdge(row, col)) {
				XMVECTOR currentPos = pLvl->worldPos(current);
				XMVECTOR nextPos = pLvl->worldPos(nodes[row].pos);
				
				float nextDist = XMVectorGetX(XMVector3Length(nextPos - currentPos)) + nodes[col].shortestDist;
				
				if (nextDist < nodes[row].shortestDist) {
					prevNode[row] = current;
					nodes[row].shortestDist = nextDist;
				}

			}
		}

		nodes[index(current)].visited = true;

		float minDist = std::numeric_limits<float>::infinity();
		foundNode = false;

		for (size_t next = 0; next < nodes.size(); next++) {
			if (!nodes[next].visited && nodes[next].shortestDist < minDist) {
				current = nodes[next].pos;
				minDist = nodes[next].shortestDist;
				foundNode = true;
			}
		}

		if (!foundNode) {
			break;
		}

		if (current == target) {
			Node n = target;
			while (!(n == initial)) {
				path.push_back(n);
				n = prevNode[index(n)];
			}
			break;
		}

	}

	if (!foundNode) {
		buildPath();
	}

	pathLocked = false;
}

bool Node::operator==(const Node& rhs) const
{
	return i==rhs.i && j==rhs.j && k==rhs.k;
}