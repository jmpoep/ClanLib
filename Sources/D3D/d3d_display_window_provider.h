/*
**  ClanLib SDK
**  Copyright (c) 1997-2020 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "API/Display/TargetProviders/display_window_provider.h"
#include "API/Display/Render/graphic_context.h"
#include "Display/Platform/Win32/win32_window.h"
#include <mutex>

namespace clan
{
	class D3DDisplayWindowProvider : public DisplayWindowProvider
	{
	public:
		D3DDisplayWindowProvider();
		~D3DDisplayWindowProvider() override;

		Rect get_geometry() const override;
		Rect get_viewport() const override;
		float get_pixel_ratio() const override;

		bool has_focus() const override;
		bool is_minimized() const override;
		bool is_maximized() const override;
		bool is_visible() const override;
		bool is_fullscreen() const override { return false; } // FIXME: real implementation
		std::string get_title() const override;
		Size get_minimum_size(bool client_area = false) const override;
		Size get_maximum_size(bool client_area = false) const override;

		GraphicContext& get_gc() override;

		InputDevice &get_keyboard() override { return window.get_keyboard(); }
		InputDevice &get_mouse() override { return window.get_mouse(); }
		std::vector<InputDevice> &get_game_controllers() override { return window.get_game_controllers(); }

		DisplayWindowHandle get_handle() const override;

		bool is_clipboard_text_available() const override;
		bool is_clipboard_image_available() const override;
		std::string get_clipboard_text() const override;
		PixelBuffer get_clipboard_image() const override;

		const ComPtr<ID3D11Device> &get_device() const { return device; }
		const ComPtr<ID3D11DeviceContext> &get_device_context() const { return device_context; }
		D3D_FEATURE_LEVEL get_feature_level() const { return feature_level; }
		const ComPtr<IDXGISwapChain> &get_swap_chain() const { return swap_chain; }
		const ComPtr<ID3D11Texture2D> &get_back_buffer() const { return back_buffer; }
		const ComPtr<ID3D11RenderTargetView> &get_back_buffer_rtv() const { return back_buffer_rtv; }
		const ComPtr<ID3D11Debug> &get_debug() const { return debug; }
		const ComPtr<ID3D11InfoQueue> &get_info_queue() const { return info_queue; }

		Point client_to_screen(const Point &client) override;
		Point screen_to_client(const Point &screen) override;

		void capture_mouse(bool capture) override;

		void create(DisplayWindowSite *site, const DisplayWindowDescription &description) override;

		void show_system_cursor() override;
		CursorProvider *create_cursor(const CursorDescription &cursor_description) override;
		void set_cursor(CursorProvider *cursor) override;
		void set_cursor(StandardCursor type) override;
		void set_cursor_handle(HCURSOR cursor) override;
		void hide_system_cursor() override;

		void set_title(const std::string &new_title) override;
		void set_position(const Rect &pos, bool client_area) override;
		void set_size(int width, int height, bool client_area) override;
		void set_minimum_size(int width, int height, bool client_area) override;
		void set_maximum_size(int width, int height, bool client_area) override;
		void set_enabled(bool enable) override;

		void minimize() override;
		void restore() override;
		void maximize() override;
		void toggle_fullscreen() override;

		void show(bool activate) override;
		void hide() override;

		void bring_to_front() override;

		void flip(int interval) override;

		void update(const Rect &rect);

		void set_clipboard_text(const std::string &text) override;
		void set_clipboard_image(const PixelBuffer &buf) override;

		void request_repaint() override;

		void set_large_icon(const PixelBuffer &image) override;
		void set_small_icon(const PixelBuffer &image) override;

		void enable_alpha_channel(const Rect &blur_rect) override;
		void extend_frame_into_client_area(int left, int top, int right, int bottom) override;

		void validate_context();

		void set_pixel_ratio(float ratio) override;

	private:
		void create_swap_chain_buffers();
		void release_swap_chain_buffers();
		void on_window_resized();
		void log_debug_messages();

		Win32Window window;

		DisplayWindowSite *site;
		GraphicContext gc;

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> device_context;
		D3D_FEATURE_LEVEL feature_level;
		ComPtr<IDXGISwapChain> swap_chain;
		ComPtr<ID3D11Debug> debug;
		ComPtr<ID3D11InfoQueue> info_queue;
		ComPtr<ID3D11Texture2D> back_buffer;
		ComPtr<ID3D11RenderTargetView> back_buffer_rtv;

		bool use_fake_front_buffer;
		ComPtr<ID3D11Texture2D> fake_front_buffer;

		int current_interval_setting;

		typedef HRESULT(WINAPI *FuncD3D11CreateDeviceAndSwapChain)(
			__in_opt IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			__in_opt CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
			__out_opt IDXGISwapChain** ppSwapChain,
			__out_opt ID3D11Device** ppDevice,
			__out_opt D3D_FEATURE_LEVEL* pFeatureLevel,
			__out_opt ID3D11DeviceContext** ppImmediateContext);

		static std::recursive_mutex d3d11_mutex;
		static HMODULE d3d11_dll;
		static FuncD3D11CreateDeviceAndSwapChain d3d11_createdeviceandswapchain;
	};
}
