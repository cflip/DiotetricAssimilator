#include "constant_buffer.h"

#include "debug.h"

ConstantBuffer::ConstantBuffer(const GfxContext& gfx, const void* initialData, int dataSize)
	: m_gfx(gfx)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = dataSize;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;

	GFX_ASSERT(m_gfx.GetDevice()->CreateBuffer(&desc, &data, &m_buffer));
}

void ConstantBuffer::Update(const void* data, int dataSize)
{
	// TODO: This does a memory copy every single frame

	D3D11_MAPPED_SUBRESOURCE msr;
	GFX_ASSERT(m_gfx.GetContext()->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr));
	CopyMemory(msr.pData, data, dataSize);
	m_gfx.GetContext()->Unmap(m_buffer.Get(), 0);
}

void VertexConstantBuffer::Bind() const
{
	m_gfx.GetContext()->VSSetConstantBuffers(0, 1, m_buffer.GetAddressOf());
}

void PixelConstantBuffer::Bind() const
{
	m_gfx.GetContext()->PSSetConstantBuffers(0, 1, m_buffer.GetAddressOf());
}
