#include "buffer.h"

#include "debug.h"

VertexBuffer::VertexBuffer(const GfxContext& gfx, const std::vector<Vertex>& vertices)
	: m_gfx(gfx)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
	desc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices.data();

	GFX_ASSERT(m_gfx.GetDevice()->CreateBuffer(&desc, &data, &m_buffer));
}

void VertexBuffer::Bind() const
{
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	m_gfx.GetContext()->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), &stride, &offset);
}

IndexBuffer::IndexBuffer(const GfxContext& gfx, const std::vector<unsigned short>& indices)
	: m_gfx(gfx), m_indexCount(static_cast<unsigned>(indices.size()))
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(indices.size()) * sizeof(unsigned short);
	desc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indices.data();

	GFX_ASSERT(m_gfx.GetDevice()->CreateBuffer(&desc, &data, &m_buffer));
}

void IndexBuffer::Bind() const
{
	m_gfx.GetContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
