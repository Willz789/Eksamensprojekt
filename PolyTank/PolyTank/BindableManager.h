#pragma once

#include "IBindable.h"

#include <memory>
#include <unordered_map>
#include <utility>
#include <cassert>

class BindableManager {
public:
	BindableManager(Graphics& gfx);

	template<typename T, typename... CTorArgs>
	std::shared_ptr<T> get(CTorArgs&&... args);

	void reset();

private:
	std::unordered_map<std::string, std::weak_ptr<IBindable>> bindables;
	Graphics& gfx;
};

template<typename T, typename... CTorArgs>
inline std::shared_ptr<T> BindableManager::get(CTorArgs&&... args) {
	std::string uid = T::uid(std::forward<CTorArgs>(args)...);
	
	if (uid.empty()) {
		return std::make_shared<T>(gfx, std::forward<CTorArgs>(args)...);
	}
	
	auto it = bindables.find(uid);
	if (it == bindables.end() || it->second.expired()) {

		std::shared_ptr<T> bindable = std::make_shared<T>(gfx, std::forward<CTorArgs>(args)...);
		bindables[uid] = bindable;
		return bindable;

	} else {
		assert(std::dynamic_pointer_cast<T>(it->second.lock()));

		return std::static_pointer_cast<T>(it->second.lock());
	}
}
