#include "shader.h"

#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

#include "debug.h"

VertexShader::VertexShader(const GfxContext& gfx, LPCWSTR sourcePath)
	: Shader(gfx)
{
	GFX_ASSERT(D3DReadFileToBlob(L"VertexShader.cso", &m_blob));
	GFX_ASSERT(m_gfx.GetDevice()->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader));
}

void VertexShader::Bind() const
{
	m_gfx.GetContext()->VSSetShader(m_shader.Get(), nullptr, 0);
}

PixelShader::PixelShader(const GfxContext& gfx, LPCWSTR sourcePath)
	: Shader(gfx)
{
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	GFX_ASSERT(D3DReadFileToBlob(L"PixelShader.cso", &blob));
	GFX_ASSERT(m_gfx.GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));
}

void PixelShader::Bind() const
{
	m_gfx.GetContext()->PSSetShader(m_shader.Get(), nullptr, 0);
}