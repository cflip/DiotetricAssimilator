#pragma once

#include <vector>

#include "gfx.h"

class VertexBuffer 
{
public:
	VertexBuffer(const GfxContext&, const std::vector<Vertex>& vertices);

	void Bind() const;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	const GfxContext& m_gfx;
};

class IndexBuffer
{
public:
	IndexBuffer(const GfxContext&, const std::vector<unsigned short>& indices);
	
	void Bind() const;

	unsigned Count() const { return m_indexCount; }
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	const GfxContext& m_gfx;

	unsigned m_indexCount;
};