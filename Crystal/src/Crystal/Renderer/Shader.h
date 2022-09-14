#pragma once

namespace Crystal {
	class CRYSTAL_API Shader {
	public:
		~Shader() = default;

		static std::shared_ptr<Shader> Create(const std::string& fileName);
	};
}
