#pragma once
#include "RendererAPI.h"

namespace Crystal {
	class CRYSTAL_API Shader {
	public:
		~Shader() = default;

		virtual	void CompileOrGetBinaries() = 0;
		virtual std::string GetName() { return m_Name; }

		static std::shared_ptr<Shader> Create(const std::string fileName);
	protected:
		Shader::Shader(const std::string fileName) : m_FileName(fileName), m_Name(GetNameFromFile(fileName)) {}

		std::string m_FileName;
		std::string m_Name;
	private:
		static std::string GetNameFromFile(const std::string& fileName);
	};
}
