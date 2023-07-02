# swap_chain
> [Swap Chains](https://learn.microsoft.com/en-us/windows/win32/direct3d12/swap-chains)

# 
```c++
// swap chain 交换链：虛擬幀緩衝區
		ComPtr<IDXGISwapChain4> swapChain;
		{
			const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
				.Width = width,
				.Height = height,
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.Stereo = FALSE,
				.SampleDesc = {
					.Count = 1,
					.Quality = 0
				},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = bufferCount,
				.Scaling = DXGI_SCALING_STRETCH,
				.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
				.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
			};
			ComPtr<IDXGISwapChain1> swapChain1;
			dxgiFactory->CreateSwapChainForHwnd(
				commandQueue.Get(),
				window.GetHandle(),
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain1
			) >> chk;
			// 打少了個1 GG
			swapChain1.As(&swapChain) >> chk;
		}
```

具體步驟:
1. 定義DXGI_SWAP_CHAIN_DESC1結構體，指定交換鏈的參數:
   - 解析度
   - 格式
   - 緩衝區使用模式
   - 緩衝區計數
   - 縮放模式
   - 交換效果
   - Alpha 模式
   - 標誌 
   - 等等
2. 調用CreateSwapChainForHwnd()創建交換鏈1(Swap Chain 1)
3. 使用As()方法從交換鏈1獲取交換鏈4(Swap Chain 4)接口，存到swapChain指針中

這樣一來，我們就創建好了交換鏈，其提供DXGI_FORMAT_R8G8B8A8_UNORM格式的幀緩衝區，用作渲染目標。

使用交換鏈我們可以:
   - 取得交換鏈的描述符
   - 獲取下一個幀緩衝區
   - 显示/切換幀緩衝區
   - IDXGISwapChain4 接口提供了額外的功能，比如允許 tearing。