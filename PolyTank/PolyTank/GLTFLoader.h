#pragma once

#include "Scene.h"
#include <filesystem>
#include <stdint.h>
#include <json.hpp>

namespace GLTF {

	using Buffer = std::vector<uint8_t>;

	struct BufferView {
		const Buffer* pBuf;
		size_t offset, size;
	};

	struct Accessor {
		void readElements(void* out, size_t outStride) const;

		const BufferView* pView;
		size_t elementSize, elementCount;
	};
	
	class Loader {
	public:
		Loader(const std::filesystem::path& file);

		void getScene(Graphics& gfx, SceneNode* pTarget, size_t sceneIdx = std::numeric_limits<size_t>::max()) const;
		
		static std::shared_ptr<IBindable> defaultMaterial(Graphics& gfx);

	private:
		void parseNode(Graphics& gfx, const nlohmann::json& jnode, SceneNode* pNode) const;
		void parseMesh(Graphics& gfx, const nlohmann::json& jmesh, SceneNode* pNode) const;
		std::shared_ptr<IBindable> parseMaterial(Graphics& gfx, const nlohmann::json& jmaterial) const;
		std::shared_ptr<IBindable> readIndexBuffer(Graphics& gfx, const std::string& name, const Accessor* pAccessor) const;
		std::shared_ptr<IBindable> readVertexBuffer(Graphics& gfx, const std::string& name, const nlohmann::json& jattributes) const;
		std::unique_ptr<IDrawable> parsePrimitive(Graphics& gfx, const nlohmann::json& jprimitive, const std::string& name) const;
		
	private:
		nlohmann::json gltf;

		std::vector<GLTF::Buffer> buffers;
		std::vector<GLTF::BufferView> bufViews;
		std::vector<GLTF::Accessor> accessors;
	};


}


