#include "Enemy.h"
#include "PolyTank.h"

#include <random>
#include <chrono>

using namespace DirectX;

inline void newNode(uint32_t iNode, uint32_t jNode, uint32_t kNode, std::vector<Node>& nodes, uint32_t currentNode, bool lift) {
	bool registered = false;
	for (Node& node : nodes) {
		if (node.i == iNode && node.j == jNode && node.k == kNode) {
			registered = true;

			float dist = sqrt((jNode - nodes[currentNode].j) * (jNode - nodes[currentNode].j)
				+ (kNode - nodes[currentNode].k) * (kNode - nodes[currentNode].k));
			if (lift) {
				dist += 1;
			}
			if (nodes[currentNode].shortestDist + dist < node.shortestDist) {
				node.shortestDist = nodes[currentNode].shortestDist + dist;
				node.pathToNode = nodes[currentNode].pathToNode;
				node.pathToNode.push_back(&nodes[currentNode]);
			}
			break;
		}
	}
	if (!registered) {
		Node newNode;
		newNode.i = iNode;
		newNode.j = jNode;
		newNode.k = kNode;
		newNode.visited = false;
		newNode.pathToNode = nodes[currentNode].pathToNode;
		newNode.pathToNode.push_back(&nodes[currentNode]);
		float dist = sqrt((jNode - nodes[currentNode].j) * (jNode - nodes[currentNode].j)
			+ (kNode - nodes[currentNode].k) * (kNode - nodes[currentNode].k));
		if (lift) {
			dist += 1;
		}
		newNode.shortestDist = nodes[currentNode].shortestDist + dist;
		nodes.push_back(newNode);
	}
}

Enemy::Enemy(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Tank* pTarget, Level& lvl) :
	pTarget(pTarget)
{
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, XMVectorSet(10.0f, 3.0f, 10.0f, 0.0f));
	pLvl = &lvl;
	buildPath();
}

void Enemy::update(float dt)
{
	if (path.size() > 0) {
		// move tank
	}
}

void Enemy::buildPath()
{
	size_t startTime = std::chrono::system_clock::now().time_since_epoch().count();
	uint32_t w = pLvl->getW();
	uint32_t d = pLvl->getD();
	uint32_t h = pLvl->getH();

	path.clear();

	static std::random_device rd;
	static std::default_random_engine rng(rd());
	std::uniform_real_distribution uDist(0.0f, 2.0f * pi);

	XMVECTOR playerPos = pTarget->getPosition();	
	
	XMVECTOR initialPos = pTank->getPosition();
	Node initialNode = pLvl->findNearestNode(initialPos);
	initialNode.shortestDist = 0;
	initialNode.visited = false;
	
	bool foundPath = false;
	while (foundPath == false) {
		float angle = uDist(rng);
		XMVECTOR targetOffset = 4.0f * XMVectorSet(cos(angle), 0.0f, sin(angle), 0.0f);

		XMVECTOR targetPos = playerPos + targetOffset;

		Node targetNode = pLvl->findNearestNode(targetPos);
		targetNode.shortestDist = std::numeric_limits<float>::infinity();
		targetNode.visited = false;

		std::vector<Node> nodes;
		nodes.push_back(initialNode);

		bool completePath = false;
		while (completePath == false) {
			bool allVisisted = true;
			for (Node& node : nodes) {
				if (!node.visited) {
					allVisisted = false;
					break;
				}
			}
			if (allVisisted) {
				completePath = true;
				break;
			}
			uint32_t currentNode = 0;
			float shortestDist = std::numeric_limits<float>::infinity();
			for (uint32_t i = 0; i < nodes.size(); i++) {
				if (!nodes[i].visited && nodes[i].shortestDist < shortestDist) {
					shortestDist = nodes[i].shortestDist;
					currentNode = i;
				}
			}
			nodes[currentNode].visited = true;
			if (nodes[currentNode] == targetNode) {
				completePath = true;
				foundPath = true;
				nodes[currentNode].pathToNode.push_back(&targetNode);
				std::vector<Node> anotherPath;
				path.reserve(nodes[currentNode].pathToNode.size());
				std::transform(std::begin(nodes[currentNode].pathToNode), std::end(nodes[currentNode].pathToNode),
					std::back_inserter(path), [](Node* path) {return *path; });
				break;
			}

			else {
				uint32_t col = nodes[currentNode].i * d * w + nodes[currentNode].j * w + nodes[currentNode].k;
				for (int8_t j = -1; j < 2; j++) {
					for (int8_t k = -1; k < 2; k++) {
						if (j != 0 || k != 0) {
							uint32_t iNode = nodes[currentNode].i;
							uint32_t jNode = nodes[currentNode].j + j;
							uint32_t kNode = nodes[currentNode].k + k;
							if (kNode < w && jNode < d && iNode < h) {
								uint32_t row = iNode * d * w + jNode * w + kNode;
								if ((*pLvl->getEdges())[row * w * h * d + col] == 1) {
									newNode(iNode, jNode, kNode, nodes, currentNode, false);
								}
								else if (pLvl->getLayer(iNode)->getBlockType(jNode, kNode) == 5) {
									if (iNode >= h - 1 || !pLvl->getLayer(iNode + 1)->isBlockSolid(jNode, kNode)) {
										newNode(iNode, jNode, kNode, nodes, currentNode, true);
									}
								}
								else if (iNode < h - 1 && pLvl->getLayer(iNode + 1)->getBlockType(jNode, kNode) == 5) {
									if (iNode >= h - 2 || !pLvl->getLayer(iNode + 2)->isBlockSolid(jNode, kNode)) {
										newNode(iNode + 1, jNode, kNode, nodes, currentNode, true);
									}
								}
							}
						}
					}
				}

				if (pLvl->getLayer(nodes[currentNode].i)->getBlockType(nodes[currentNode].j, nodes[currentNode].k) == 5) {
					uint32_t iNode = nodes[currentNode].i - 1;

					for (int8_t jOff = -1; jOff < 2; jOff++) {
						for (int8_t kOff = -1; kOff < 2; kOff++) {
							uint32_t jNode = nodes[currentNode].j + jOff;
							uint32_t kNode = nodes[currentNode].k + kOff;
							if (jOff != 0 || kOff != 0) {
								if (jOff == 0 || kOff == 0) {
									if (pLvl->getLayer(iNode)->doesBlockExist(jNode, kNode)) {
										if (iNode + 1 >= pLvl->getLayers()->size() || pLvl->getLayer(iNode + 1)->isBlockSolid(jNode, kNode)) {
											if (kNode < w && jNode < d && iNode < h) {
												newNode(iNode, jNode, kNode, nodes, currentNode, false);
											}
										}
									}
								}
								else {
									if (pLvl->getLayer(iNode)->doesBlockExist(jNode, nodes[currentNode].k)) {
										if (iNode + 1 >= pLvl->getLayers()->size() || pLvl->getLayer(iNode + 1)->isBlockSolid(jNode, nodes[currentNode].k)) {
											if (pLvl->getLayer(iNode)->doesBlockExist(nodes[currentNode].j, kNode)) {
												if (iNode + 1 >= pLvl->getLayers()->size() || pLvl->getLayer(iNode + 1)->isBlockSolid(nodes[currentNode].j, kNode)) {
													if (pLvl->getLayer(iNode)->doesBlockExist(jNode, kNode)) {
														if (iNode + 1 >= pLvl->getLayers()->size() || pLvl->getLayer(iNode + 1)->isBlockSolid(jNode, kNode)) {
															if (kNode < w && jNode < d && iNode < h) {
																newNode(iNode, jNode, kNode, nodes, currentNode, false);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	size_t totalTime = std::chrono::system_clock::now().time_since_epoch().count() - startTime;
	std::cout << "\n" << totalTime << "\n";
}

bool Node::operator==(const Node& rhs) const
{
	return i==rhs.i && j==rhs.j && k==rhs.k;
}