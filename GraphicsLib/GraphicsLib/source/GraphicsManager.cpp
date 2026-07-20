#include "GraphicsManager.h"

GraphicsManager::GraphicsManager(void* srcHandle)
{
  auto hWnd = static_cast<HWND>(srcHandle);

  RECT rc;
  GetClientRect(hWnd, &rc);

  auto width = rc.right - rc.left;
  auto height = rc.bottom - rc.top;

  auto vecAdapters = GetAdapters();
  
  Vector<D3D_FEATURE_LEVEL> featureLevels = {
#if !USING(DX_VERSION_11_0)
    D3D_FEATURE_LEVEL_11_1,
#endif
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };

  D3D_FEATURE_LEVEL featureLevel;

  ID3D11Device* pDevice = nullptr;
  ID3D11DeviceContext* pContext = nullptr;

  uint32 deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
  deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  ThrowIfFailed(D3D11CreateDevice(vecAdapters[0],
                                  D3D_DRIVER_TYPE_UNKNOWN,
                                  nullptr,
                                  deviceFlags,
                                  featureLevels.data(),
                                  static_cast<uint32>(featureLevels.size()),
                                  D3D11_SDK_VERSION,
                                  &pDevice,
                                  &featureLevel,
                                  &pContext));

  m_pDevice = GetAs<D3DDevice>(pDevice);
  SAFE_RELEASE(pDevice);

  m_pImmediateContext = GetAs<D3DDeviceContext>(pContext);
  SAFE_RELEASE(pContext);

  m_pDeferredContext = CreateDeferredContext();
  m_pActiveContext = m_pImmediateContext;

#if defined(_DEBUG)
  ThrowIfFailed(m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_pDebug));
#endif

  // Create swap chain
#if USING(DX_VERSION_11_0)
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  memset(&swapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swapChainDesc.OutputWindow = hWnd;
  swapChainDesc.BufferCount = 2;
  swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
  swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapChainDesc.BufferDesc.Width = width;
  swapChainDesc.BufferDesc.Height = height;
  swapChainDesc.Windowed = true;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;

  DXGIDevice* pDXGIDevice = nullptr;
  DXGIAdapter* pAdapter = nullptr;
  DXGIFactory* pFactory = nullptr;

  ThrowIfFailed(m_pDevice->QueryInterface(__uuidof(DXGIDevice), (void**)&pDXGIDevice));
  ThrowIfFailed(pDXGIDevice->GetAdapter(&pAdapter));
  ThrowIfFailed(pAdapter->GetParent(__uuidof(DXGIFactory), (void**)&pFactory));
  ThrowIfFailed(pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain));

#elif USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2) || USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
  memset(&swapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));

  swapChainDesc.Width = width;
  swapChainDesc.Height = height;
  swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swapChainDesc.Stereo = false;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 2;
  swapChainDesc.Scaling = DXGI_SCALING_NONE;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  IDXGIDevice1* pDXGIDevice = nullptr;
  IDXGIAdapter* pAdapter = nullptr;
  IDXGIFactory2* pFactory = nullptr;

  ThrowIfFailed(m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDevice));
  ThrowIfFailed(pDXGIDevice->GetAdapter(&pAdapter));
  ThrowIfFailed(pAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pFactory));
#if USING(DX_VERSION_11_1)
  ThrowIfFailed(pFactory->CreateSwapChainForHwnd(m_pDevice, hWnd, &swapChainDesc, nullptr, nullptr, &m_pSwapChain));
#elif USING(DX_VERSION_11_2) || USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
  IDXGISwapChain1* pSwapChain1 = nullptr;
  ThrowIfFailed(pFactory->CreateSwapChainForHwnd(m_pDevice, hWnd, &swapChainDesc, nullptr, nullptr, &pSwapChain1));
  m_pSwapChain = GetAs<SwapChain>(pSwapChain1);
#endif
  ThrowIfFailed(pDXGIDevice->SetMaximumFrameLatency(3));

#endif

  CreateBackBuffer();
}

void
GraphicsManager::CreateBackBuffer()
{
  assert(m_pDevice && m_pSwapChain);

  SAFE_RELEASE(m_pBackBuffer);
  SAFE_RELEASE(m_pBackBufferRTV);

  ThrowIfFailed(m_pSwapChain->GetBuffer(0, __uuidof(D3DTexture2D), (void**)&m_pBackBuffer));
#if USING(DX_VERSION_11_0) || USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2)
  ThrowIfFailed(m_pDevice->CreateRenderTargetView(m_pBackBuffer, nullptr, &m_pBackBufferRTV));
#elif USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
  ThrowIfFailed(m_pDevice->CreateRenderTargetView1(m_pBackBuffer, nullptr, &m_pBackBufferRTV));
#endif

}

void
GraphicsManager::ResizeSwapChain(uint32 width, uint32 height)
{
  if (!m_pSwapChain) {
    return;
  }

  SAFE_RELEASE(m_pBackBuffer);
  SAFE_RELEASE(m_pBackBufferRTV);

  ThrowIfFailed(m_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

  CreateBackBuffer();
}

D3DDeviceContext*
GraphicsManager::CreateDeferredContext() const
{
  assert(m_pDevice);

  D3DDeviceContext* pContext = nullptr;
#if USING(DX_VERSION_11_0)
  m_pDevice->CreateDeferredContext(0, &pContext);
#elif USING(DX_VERSION_11_1)
  m_pDevice->CreateDeferredContext1(0, &pContext);
#elif USING(DX_VERSION_11_2)
  m_pDevice->CreateDeferredContext2(0, &pContext);
#elif USING(DX_VERSION_11_3)
  m_pDevice->CreateDeferredContext3(0, &pContext);
#elif USING(DX_VERSION_11_4)
  ID3D11DeviceContext* pContext4 = nullptr;
  m_pDevice->CreateDeferredContext(0, &pContext4);
  pContext = GetAs<D3DDeviceContext>(pContext4);
#endif
  return pContext;
}

void
GraphicsManager::SetRenderTarget(RenderTargetView* pRTV, DepthStencilView* pDSV)
{
  RenderTargetView* pTarget = nullptr;
  DepthStencilView* pDSTarget = nullptr;

  if (pRTV) {
    pTarget = pRTV;
  }
  if (pDSV) {
    pDSTarget = pDSV;
  }

#if USING(DX_VERSION_11_0) || USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2)
  m_pActiveContext->OMSetRenderTargets(1, &pTarget, pDSTarget);
#elif USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
  ID3D11RenderTargetView* pTarget0 = nullptr;
  pTarget0 = GetAs<ID3D11RenderTargetView1>(pTarget);
  m_pActiveContext->OMSetRenderTargets(1, &pTarget0, pDSTarget);

#endif
}

void
GraphicsManager::ClearRenderTarget(RenderTargetView* pRTV, const float clearColor[4])
{
  m_pActiveContext->ClearRenderTargetView(pRTV, clearColor);
}

void
GraphicsManager::Present(uint32 syncInterval, uint32 presentFlags)
{
#if USING(DX_VERSION_11_0)
  m_pSwapChain->Present(syncInterval, presentFlags);
#elif USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2) || USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
  DXGI_PRESENT_PARAMETERS presentParameters;
  memset(&presentParameters, 0, sizeof(DXGI_PRESENT_PARAMETERS));
  m_pSwapChain->Present1(syncInterval, presentFlags, &presentParameters);
#endif
}

Vector<DXGIAdapter*>
GraphicsManager::GetAdapters()
{
  Vector<DXGIAdapter*> vecAdapters;

  DXGIFactory* pFactory = nullptr;
  DXGIAdapter* pAdapter = nullptr;

  // Enumerate adapters
#if USING(DX_VERSION_11_0)
  ThrowIfFailed(CreateDXGIFactory1(__uuidof(DXGIFactory), (void**)&pFactory));

  for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
  {
    DXGI_ADAPTER_DESC desc;
    pAdapter->GetDesc(&desc);

    vecAdapters.push_back(pAdapter);
  }

#elif USING(DX_VERSION_11_1) || USING(DX_VERSION_11_2) || USING(DX_VERSION_11_3) || USING(DX_VERSION_11_4)
  ThrowIfFailed(CreateDXGIFactory1(__uuidof(DXGIFactory), (void**)&pFactory));

  for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
  {
    DXGI_ADAPTER_DESC1 desc;
    pAdapter->GetDesc1(&desc);

    vecAdapters.push_back(pAdapter);
  }

#endif

  SAFE_RELEASE(pFactory);

  return vecAdapters;
}
