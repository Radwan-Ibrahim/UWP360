// Image360: a XAML control that renders an equirectangular (360) image
// on the inside of a sphere and lets the user look around by dragging.

#pragma once
#include "pch.h"
#include "Common\DirectXPanelBase.h"
#include "Common\StepTimer.h"
#include "Camera.h"
#include "Transform.h"
#include "VSD3DStarter.h"
#include "Animation\Animation.h"

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;


namespace DirectX360Image
{
	// Public WinRT control consumed from XAML, e.g. <dx:Image360 TextureSource="https://..." />.
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Image360 sealed : DirectX360Image::DirectXPanelBase
	{
	public:
		Image360();

		// ---- Public API ------------------------------------------------------

		// Image URI (http/https or package path) mapped onto the sphere.
		static property DependencyProperty^ TextureSourceProperty
		{
			DependencyProperty^ get() { return m_TextureSourceProperty; }
		}

		property Windows::Foundation::Uri^ TextureSource
		{
			Windows::Foundation::Uri^ get() { return (Windows::Foundation::Uri^)GetValue(TextureSourceProperty); }
			void set(Windows::Foundation::Uri^ value) { SetValue(TextureSourceProperty, value); }
		}

		// World transform of the sphere (scale/rotation/translation).
		property Transform^ Transform
		{
			DirectX360Image::Transform^ get();
		}

		// Camera placed at the sphere's center; rotated by touch/mouse drag.
		property DirectX360Image::Camera^ Camera
		{
			DirectX360Image::Camera^ get();
		}

		void Clear(Windows::UI::Color color);

		// Raised once the sphere mesh and first texture have finished loading.
		event Windows::Foundation::EventHandler<bool>^ ModelLoaded;

	private protected:
		// ---- DirectXPanelBase overrides ----------------------------------------
		virtual void Render() override;
		virtual void CreateDeviceResources() override;
		virtual void CreateSizeDependentResources() override;

		// Mesh file (.cmo) loaded from the DirectX360Image package folder.
		property Platform::String^ ModelName
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

	private:
		// ---- Scene state -------------------------------------------------------
		static DependencyProperty^							m_TextureSourceProperty;
		Windows::Devices::Sensors::Accelerometer^			m_Accelerometer;
		Platform::String^									m_ModelName = L"Sphere.cmo";
		DirectX360Image::Camera^							m_Camera;
		SkinnedMeshRenderer*								m_skinnedMeshRenderer;
		bool isanimated;
		DirectX360Image::Transform^							m_Transform;
		DirectX::XMMATRIX									m_TransformMatrix;
		std::vector<VSD3DStarter::Mesh*>				    m_meshModels;
		DX::StepTimer                                       m_timer;

		// ---- Direct3D resources ------------------------------------------------
		Microsoft::WRL::ComPtr<IDXGIOutput>                 m_dxgiOutput;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      m_depthStencilView;
		Windows::Foundation::IAsyncAction^					m_renderLoopWorker;

		// Members used to keep track of the graphics state.
		VSD3DStarter::Graphics								m_graphics;
		VSD3DStarter::LightConstants						m_lightConstants;
		VSD3DStarter::MiscConstants							m_miscConstants;

		// ---- Helpers -----------------------------------------------------------

		// Loads the sphere mesh and binds the texture at TextureUrl to it.
		concurrency::task<void> LoadAsync(VSD3DStarter::Graphics& graphics,
			const std::wstring& meshFilename,
			const std::wstring& TextureUrl,
			const bool IsAnimated = false,
			const std::wstring& shaderPathLocation = L"",
			const std::wstring& texturePathLocation = L""
		);

		// Background render thread synchronized to the display's vblank.
		void StartRenderLoop();
		void StopRenderLoop();
		void UpdateAnimation(float TimeDelta);

		// ---- Event handlers ----------------------------------------------------
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnManipulationDelta(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs ^e);
		static void OnTextureSourceChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e);
		void SetTextureSource(Windows::Foundation::Uri^ source);

		~Image360();
	};
}