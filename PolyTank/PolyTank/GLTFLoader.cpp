#include "GLTFLoader.h"

#include <fstream>
#include <unordered_map>
#include <sstream>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "GLTFMaterial.h"

using nlohmann::json;
using namespace DirectX;

inline std::vector<GLTF::Buffer> getBuffers(const json& jbuffers, const std::filesystem::path& file) {

	std::filesystem::path path = file;
	path.remove_filename();

	std::vector<GLTF::Buffer> buffers;

	for (const json& jbuffer : jbuffers) {
		std::filesystem::path bufFile = path;
		bufFile.concat(jbuffer["uri"].get<std::string>());

		size_t bufSize = jbuffer["byteLength"];
		buffers.emplace_back(bufSize);

		std::ifstream bufis(bufFile, std::ios::binary);
		if (!bufis.good()) {
			throw std::runtime_error("");
		}

		bufis.read(reinterpret_cast<char*>(buffers.back().data()), bufSize);
	}

	return buffers;
}


inline std::vector<GLTF::BufferView> getViews(const json& jbufViews, const std::vector<GLTF::Buffer>& buffers) {

	std::vector<GLTF::BufferView> bufViews;

	for (const json& jbufView : jbufViews) {

		bufViews.emplace_back();
		GLTF::BufferView& bufView = bufViews.back();

		bufView.pBuf = &buffers[jbufView["buffer"]];
		bufView.offset = jbufView["byteOffset"];
		bufView.size = jbufView["byteLength"];
	}

	return bufViews;
}

inline size_t getElementSize(const json& jaccessor) {

	std::unordered_map<std::string, size_t> compCounts = {
		{ "SCALAR",	1 },
		{ "VEC2",	2 },
		{ "VEC3",	3 },
		{ "VEC4",	4 },
		{ "MAT2",	4 },
		{ "MAT3",	9 },
		{ "MAT4",	16 }
	};

	size_t componentCount = compCounts[jaccessor["type"]];

	std::unordered_map<uint32_t, size_t> compSizes = {
		{ 5120,	1 },
		{ 5121,	1 },
		{ 5122,	2 },
		{ 5123,	2 },
		{ 5125,	4 },
		{ 5126,	4 },
	};

	size_t componentSize = compSizes[jaccessor["componentType"]];


	return componentCount * componentSize;
}

inline std::vector<GLTF::Accessor> getAccessors(
	const json& jaccessors,
	const std::vector<GLTF::BufferView>& bufViews
) {

	std::vector<GLTF::Accessor> accessors;

	for (const json& jaccessor : jaccessors) {
		accessors.emplace_back();
		GLTF::Accessor& accessor = accessors.back();
		
		accessor.pView = &bufViews[jaccessor["bufferView"]];
		accessor.elementSize = getElementSize(jaccessor);
		accessor.elementCount = jaccessor["count"];
	}

	return accessors;
}

inline XMVECTOR parseFloat3(const json& jarray) {
	return XMVectorSet(jarray[0], jarray[1], jarray[2], 0.0f);
}

inline XMVECTOR parseFloat4(const json& jarray) {
	return XMVectorSet(jarray[0], jarray[1], jarray[2], jarray[3]);
}

GLTF::Loader::Loader(const std::filesystem::path& file) {
	std::ifstream is(file);
	
	gltf = json::parse(is);

	buffers = getBuffers(gltf["buffers"], file);
	bufViews = getViews(gltf["bufferViews"], buffers);
	accessors = getAccessors(gltf["accessors"], bufViews);
}

void GLTF::Loader::getScene(Graphics& gfx, SceneNode* pTarget, size_t sceneIdx) const {

	if (sceneIdx == std::numeric_limits<size_t>::max()) {
		sceneIdx = gltf["scene"];
	}

	const json& jscene = gltf["scenes"][sceneIdx];
	const json& jnodes = gltf["nodes"];

	for (size_t nodeindex : jscene["nodes"]) {
		parseNode(gfx, jnodes[nodeindex], pTarget->addChild());
	}
}

void GLTF::Loader::parseNode(Graphics& gfx, const json& jnode, SceneNode* pNode) const {
	const json& jnodes = gltf["nodes"];
	const json& jmeshes = gltf["meshes"];

	auto it = jnode.find("translation");
	if (it != jnode.end()) {
		pNode->translate(parseFloat3(it->get<json::array_t>()));
	}

	it = jnode.find("rotation");
	if (it != jnode.end()) {
		pNode->rotate(parseFloat4(it->get<json::array_t>()));
	}

	it = jnode.find("scale");
	if (it != jnode.end()) {
		pNode->scale(parseFloat3(it->get<json::array_t>()));
	}

	it = jnode.find("mesh");
	if (it != jnode.end()) {
		parseMesh(gfx, jmeshes[it->get<size_t>()], pNode->getMesh());
	}

	it = jnode.find("children");
	if (it != jnode.end()) {
		for (size_t childindex : it->get<json::array_t>()) {
			parseNode(gfx, jnodes[childindex], pNode->addChild());
		}
	}
}

void GLTF::Loader::parseMesh(Graphics& gfx, const nlohmann::json& jmesh, Mesh* pMesh) const {
	std::string meshName = jmesh["name"];

	const json& jprimitives = jmesh["primitives"];

	for (size_t i = 0; i < jprimitives.size(); i++) {
		std::string primitiveName = (std::stringstream() << meshName << "_" << i).str();
		pMesh->addDrawable(std::move(parsePrimitive(gfx, jprimitives[i], primitiveName)));
	}

}

Drawable GLTF::Loader::parsePrimitive(Graphics& gfx, const nlohmann::json& jprimitive, const std::string& name) const {
	Drawable drawable;	

	Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
	
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	drawable.addBindable(readIndexBuffer(gfx, name, &accessors[jprimitive["indices"]]));
	drawable.addBindable(readVertexBuffer(gfx, name, jprimitive["attributes"]));
	drawable.addBindable(gfx.getBindMgr()->get<VertexShader>("./ShaderBin/VertexShader.cso", &pVSBlob));
	drawable.addBindable(gfx.getBindMgr()->get<PixelShader>("./ShaderBin/PixelShader.cso"));
	drawable.addBindable(gfx.getBindMgr()->get<InputLayout>(inputElements, pVSBlob.Get()));
	drawable.addBindable(gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	auto it = jprimitive.find("material");
	drawable.addBindable(it == jprimitive.end() ? defaultMaterial(gfx) : parseMaterial(gfx, gltf["materials"][it->get<size_t>()]));

	return drawable;
}

std::shared_ptr<IBindable> GLTF::Loader::parseMaterial(Graphics& gfx, const nlohmann::json& jmaterial) const {

	std::shared_ptr<GLTFMaterial> pMaterial = gfx.getBindMgr()->get<GLTFMaterial>();
	XMStoreFloat3(&pMaterial->factors.baseColor, parseFloat3(jmaterial["pbrMetallicRoughness"]["baseColorFactor"]));

	return pMaterial;
}

std::shared_ptr<IBindable> GLTF::Loader::defaultMaterial(Graphics& gfx) {
	std::shared_ptr<GLTFMaterial> pMaterial = gfx.getBindMgr()->get<GLTFMaterial>();
	pMaterial->factors.baseColor = { 1.0f, 1.0f, 1.0f };

	return pMaterial;
}

std::shared_ptr<IBindable> GLTF::Loader::readIndexBuffer(Graphics& gfx, const std::string& name, const Accessor* pAccessor) const {

	if (pAccessor->elementCount % 3 != 0) {
		throw std::runtime_error("invalid gltf file");
	}

	std::vector<Index> indices(pAccessor->elementCount);

	pAccessor->readElements(indices.data(), sizeof(Index));

	// flip triangle winding
	for (size_t i = 0; i < indices.size(); i += 3) {
		std::swap(indices[i], indices[i+1]);
	}

	return gfx.getBindMgr()->get<IndexBuffer>(name, indices);
}

std::shared_ptr<IBindable> GLTF::Loader::readVertexBuffer(Graphics& gfx, const std::string& name, const json& jattributes) const {

	const Accessor* pPositionAccessor = &accessors[jattributes["POSITION"]];
	const Accessor* pNormalAccessor = &accessors[jattributes["NORMAL"]];
	const Accessor* pTexcoordAccessor = &accessors[jattributes["TEXCOORD_0"]];

	if (pNormalAccessor->elementCount != pPositionAccessor->elementCount ||
		pTexcoordAccessor->elementCount != pPositionAccessor->elementCount) {
		throw std::runtime_error("invalid gltf file");
	}

	std::vector<DefaultVertex> vertices(pPositionAccessor->elementCount);

	pPositionAccessor->readElements(&vertices[0].position, sizeof(DefaultVertex));
	pNormalAccessor->readElements(&vertices[0].normal, sizeof(DefaultVertex));
	pTexcoordAccessor->readElements(&vertices[0].texcoord, sizeof(DefaultVertex));
	
	return gfx.getBindMgr()->get<VertexBuffer>(name, vertices);
}

void GLTF::Accessor::readElements(void* out, size_t outStride) const {
	
	const uint8_t* readPos = pView->pBuf->data() + pView->offset;
	uint8_t* writePos = reinterpret_cast<uint8_t*>(out);

	for (size_t i = 0; i < elementCount; i++) {
		memcpy(writePos, readPos, elementSize);

		readPos += elementSize;
		writePos += outStride;
	}
}
