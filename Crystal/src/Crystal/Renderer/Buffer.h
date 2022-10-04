#pragma once

#include "Vertex.h"

namespace Crystal
{
	class Buffer
	{
	public:
		enum class BufferType
		{
			//Multi,
			Vertex,
			Index,
			Staging,
			Uniform
		};

		struct BufferRegion
		{
			BufferType type;
			uint64_t size;
			uint64_t offset;
		};

		virtual ~Buffer() = default;

		virtual void BindData(std::vector<Vertex> &vertices) = 0;
		virtual void BindData(std::vector<Index>& indices) = 0;
		//virtual void BindData(std::vector<Vertex> &vertices, std::vector<Index> &indices) = 0;
		virtual void BindData(std::vector<void*>& data, std::vector<BufferRegion>& typeOffsets) = 0;
		virtual bool HasType(BufferType type) = 0;

		static std::shared_ptr<Buffer> Create(BufferType type, size_t size);
		static std::shared_ptr<Buffer> Create(std::set<BufferType>& types, size_t size);
	};
}
