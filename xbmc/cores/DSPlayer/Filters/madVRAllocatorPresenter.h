/*
 * $Id$
 *
 * (C) 2006-2011 see AUTHORS
 *
 * This file is part of mplayerc.
 *
 * Mplayerc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mplayerc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "AllocatorCommon.h"
#include "mvrInterfaces.h"
#include "IPaintCallback.h"
#include "../../../threads/Event.h"
//#include "..\ID3dHack.h"
  class CmadVRAllocatorPresenter
    : public ISubPicAllocatorPresenterImpl,
      public IPaintCallbackMadvr
  {
    class CSubRenderCallback : public CUnknown, public ISubRenderCallback2, public CCritSec
    {
      CmadVRAllocatorPresenter* m_pDXRAP;

    public:
      CSubRenderCallback(CmadVRAllocatorPresenter* pDXRAP)
        : CUnknown(_T("CSubRender"), NULL)
        , m_pDXRAP(pDXRAP) {
      }

      DECLARE_IUNKNOWN
      STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv) {
        return
          QI(ISubRenderCallback)
          QI(ISubRenderCallback2)
          __super::NonDelegatingQueryInterface(riid, ppv);
      }

      void SetDXRAP(CmadVRAllocatorPresenter* pDXRAP) {
        CAutoLock cAutoLock(this);
        m_pDXRAP = pDXRAP;
      }

      // ISubRenderCallback

      STDMETHODIMP SetDevice(IDirect3DDevice9* pD3DDev) {
        CAutoLock cAutoLock(this);
        return m_pDXRAP ? m_pDXRAP->SetDevice(pD3DDev) : E_UNEXPECTED;
      }

      STDMETHODIMP Render(REFERENCE_TIME rtStart, int left, int top, int right, int bottom, int width, int height) {
        CAutoLock cAutoLock(this);
        return m_pDXRAP ? m_pDXRAP->Render(rtStart, 0, 0, left, top, right, bottom, width, height) : E_UNEXPECTED;
      }
      
      // ISubRendererCallback2

      STDMETHODIMP RenderEx(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, REFERENCE_TIME AvgTimePerFrame, int left, int top, int right, int bottom, int width, int height) {
        CAutoLock cAutoLock(this);
        return m_pDXRAP ? m_pDXRAP->Render(rtStart, rtStop, AvgTimePerFrame, left, top, right, bottom, width, height) : E_UNEXPECTED;
      }
    };
    Com::SmartPtr<IUnknown> m_pDXR;
    IDirect3DDevice9* m_pD3DDevice;
    Com::SmartPtr<ISubRenderCallback2> m_pSRCB;
    Com::SmartSize m_ScreenSize;
    bool  m_bIsFullscreen;
    CEvent m_readyToDraw;
    CEvent m_readyToStartBack;

    LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold Vertices
  public:
    CmadVRAllocatorPresenter(HWND hWnd, HRESULT& hr, CStdString &_Error);
    virtual ~CmadVRAllocatorPresenter();

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    HRESULT InitD3dTest();
    void D3dTest();

    HRESULT SetDevice(IDirect3DDevice9* pD3DDev);
    HRESULT Render(
      REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, REFERENCE_TIME atpf,
      int left, int top, int bottom, int right, int width, int height);

    // ISubPicAllocatorPresenter
    STDMETHODIMP CreateRenderer(IUnknown** ppRenderer);
    STDMETHODIMP_(void) SetPosition(RECT w, RECT v);
    STDMETHODIMP_(SIZE) GetVideoSize(bool fCorrectAR);
    STDMETHODIMP_(bool) Paint(bool fAll);
    STDMETHODIMP GetDIB(BYTE* lpDib, DWORD* size);
    STDMETHODIMP SetPixelShader(LPCSTR pSrcData, LPCSTR pTarget);

    //IPaintCallbackMadvr
    bool IsReady()
    {
      /*testing wait*/
      bool ready = m_readyToDraw.WaitMSec(50);

      return ready;
    
    };
    bool StartBack()
    {
      m_readyToStartBack.Set();
      return true;
    }
  };