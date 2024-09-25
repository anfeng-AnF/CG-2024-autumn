#pragma once

#include "Bindable.h"
#include <type_traits>
#include <memory>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...p)
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}
		static void DebugString() {
			Get().printDebugMsg();
		};
		static void Remove(std::string bindId) {
			Get().RemoveBind(bindId);
		};
	private:
		void RemoveBind(std::string bindId) {
			auto it = binds.find(bindId);
			if (it != binds.end()) {
				if (it->second.use_count() <= 2) {
					binds.erase(it);
				}
			}
		};
		void printDebugMsg() {
			static int size = binds.size();
			if (binds.size() != size) {
				std::ostringstream oss; 
				size = binds.size();
				oss << "------------------------------------------" << binds.size() << std::endl;
				for (const auto& bind : binds) {
					//oss << "Key: " << bind.first << "\n";
				}

				//oss << "End of binds\n";

				// 使用 DebugStringA 输出调试信息
				OutputDebugStringA(oss.str().c_str());
			}
		};
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(Graphics& gfx, Params&&...p)
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto i = binds.find(key);
			if (i == binds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
		}
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
	};
}