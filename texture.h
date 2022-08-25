#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "gfx.h"

class Texture
{
public:
	Texture(GfxContext const&, LPCWSTR filename);
	void Bind(GfxContext const&);
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
};