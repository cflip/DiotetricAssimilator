#pragma once

#include <DirectXMath.h>

#include "gfx.h"

struct ConstantBufferData
{
	DirectX::XMMATRIX transform;
};

class ConstantBuffer
{
public:
	ConstantBuffer(const GfxContext&, ConstantBufferData initialData);

	void Bind() const;
	void Update(ConstantBufferData);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	const GfxContext& m_gfx;
};