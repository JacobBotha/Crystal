#pragma once

#include "Vertex.h"

namespace Crystal
{
	class Buffer
	{
	public:
		enum class BufferType
		{
			Vertex,
			Staging,
			Uniform
		};
		virtual ~Buffer() = default;

		virtual void BindData(std::vector<Vertex> &vertices) = 0;
		virtual BufferType GetType() = 0;

		static std::shared_ptr<Buffer> Create(BufferType type, size_t size);
	};
}
