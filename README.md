# Chil-d3d 學習之旅 (DirectX 12)
> Game engine project in C++/DX11 to explore software infrastructure ideas

本倉庫跟隨 Chil 的項目，一步步學習 DirectX12 API

# Shallow Dive 
- [Direct3D 12 Shallow Dive 1](https://www.youtube.com/watch?v=volqcWZjRig&ab_channel=ChiliTomatoNoodle)

# 學習資料
- [Swapchain的概念](https://zhuanlan.zhihu.com/p/104244526)
- [DirectX12学习笔记](https://www.zhihu.com/search?type=content&q=dirextx12)

# D3D12 基礎結構 (順序構成)
- Factory dxgi D3D12
- Devices D3D12
- [Command Queue (命令隊列)](reference/CommandQueue.md)
- [swap chain](reference/swap_chain.md)
- rtv descriptor heap
- depth buffer (2023/07/03)
- dev descriptor heap (2023/07/03)
- dsv and handle (2023/07/03)
- [Command allocator](reference/CommandAllocator.md)
- Command list
- [fence](reference/fence.md)
- Vertex data
- Vertex buffer
  - INIT content data
  - create commited resource cpu upload for vertex data
  - Copy array of vertex data to upload buffer
  - reset allocator and list
- Create the Vertex buffer view (CPU)
- index buffer (2023/07/03)
- index buffer view (2023/07/03)
- Root signature
- [PSO](reference/PSO.md)
- scissor rect
- projection martix
- render loop

# hlsl代碼詳解
- [hlsl_cube](hlsl_example/hlsl_cube.md)

# 參考資料
- [d3d12-shallow](https://github.com/planetchili/d3d12-shallow/tree/master)
- [Direct3D 12 programming guide](https://learn.microsoft.com/en-us/windows/win32/direct3d12/directx-12-programming-guide)
- [Direct3D 12 Graphics](https://learn.microsoft.com/en-us/windows/win32/api/_direct3d12/)
- [DirectX-Headers](https://github.com/microsoft/DirectX-Headers)
- [DXGI](https://learn.microsoft.com/en-us/windows/win32/api/_direct3ddxgi/)
