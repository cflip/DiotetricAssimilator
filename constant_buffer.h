#pragma once

#include "gfx.h"

class ConstantBuffer
{
public:
	ConstantBuffer(const GfxContext&, const void* data, int dataSize);

	virtual void Bind() const = 0;
	void Update(const void* data, int dataSize);
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	const GfxContext& m_gfx;
};

class VertexConstantBuffer : public ConstantBuffer
{
public:
	VertexConstantBuffer(const GfxContext& gfx, const void* data, int dataSize)
		: ConstantBuffer(gfx, data, dataSize) {}

	virtual void Bind() const override;
};

class PixelConstantBuffer : public ConstantBuffer
{
public:
	PixelConstantBuffer(const GfxContext& gfx, const void* data, int dataSize)
		: ConstantBuffer(gfx, data, dataSize) {}

	virtual void Bind() const override;
};
