#pragma once

#include "gfx.h"

class Shader
{
public:
	Shader(const GfxContext& gfx) : m_gfx(gfx) {}

	virtual void Bind() const = 0;
protected:
	const GfxContext& m_gfx;
};

class VertexShader : public Shader
{
public:
	VertexShader(const GfxContext&, LPCWSTR sourcePath);

	virtual void Bind() const override;

	ID3DBlob* GetSourceBlob() const { return m_blob.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
};

class PixelShader : public Shader
{
public:
	PixelShader(const GfxContext&, LPCWSTR sourcePath);

	virtual void Bind() const override;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;
};
