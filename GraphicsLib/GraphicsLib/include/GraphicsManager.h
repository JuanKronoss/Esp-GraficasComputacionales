/*****************************************************************************/
// Device, Context, SwapChain, and RenderTargetView are created in this class.
/*****************************************************************************/

#pragma once

#include "Helpers.h"
#include "DXInterfaces.h"

class GraphicsManager
{
 public:

  GraphicsManager(void* srcHandle);
  ~GraphicsManager() = default;

  void
  CreateBackBuffer();

  void
  ResizeSwapChain(uint32 width, uint32 height);

  // Create context functions

  D3DDeviceContext*
  CreateDeferredContext() const;

  // Render Targets

  D3DResource* GetBackBuffer() const { return m_pBackBuffer; }
  RenderTargetView* GetBackBufferRTV() const { return m_pBackBufferRTV; }

  void
  SetRenderTarget(RenderTargetView* pRTV, DepthStencilView* pDSV);
  void
  ClearRenderTarget(RenderTargetView* pRTV, const float clearColor[4]);

  // Present Functions

  void
  Present(uint32 syncInterval = 1, uint32 presentFlags = 0);

 private:

  Vector<IDXGIAdapter1*>
  GetAdapters();

  D3DDevice* m_pDevice = nullptr;
  D3DDeviceContext* m_pActiveContext = nullptr;
  D3DDeviceContext* m_pImmediateContext = nullptr;
  D3DDeviceContext* m_pDeferredContext = nullptr;

  SwapChain* m_pSwapChain = nullptr;

  D3DTexture2D* m_pBackBuffer = nullptr;
  RenderTargetView* m_pBackBufferRTV = nullptr;
  
#if defined(_DEBUG)
  ID3D11Debug* m_pDebug = nullptr;
#endif

};
