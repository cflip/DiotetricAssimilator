#include "shader.h"

#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

VertexShader::VertexShader(const GfxContext& gfx, LPCWSTR sourcePath)
	: Shader(gfx)
{
	D3DReadFileToBlob(L"VertexShader.cso", &m_blob);
	m_gfx.GetDevice()->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader);
}

void VertexShader::Bind() const
{
	m_gfx.GetContext()->VSSetShader(m_shader.Get(), nullptr, 0);
}

PixelShader::PixelShader(const GfxContext& gfx, LPCWSTR sourcePath)
	: Shader(gfx)
{
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	D3DReadFileToBlob(L"PixelShader.cso", &blob);
	m_gfx.GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader);
}

void PixelShader::Bind() const
{
	m_gfx.GetContext()->PSSetShader(m_shader.Get(), nullptr, 0);
}