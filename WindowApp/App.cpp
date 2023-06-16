#include <d3d12.h>
#include <wrl.h>
#include "App.h"
#include "GraphicsError.h"

// IID_PPV_ARGS: __uuidof(**(&devices)), IID_PPV_ARGS_Helper(&devices)

//IID_PPV_ARGS 是一個宏, 它用于簡化從接口獲取指針的語法。
//在 COM 编程中, 我們經常需要向接口的方法傳遞接口 ID(IID)和接口指針變量的地址(&args), 以獲取該接口的指針。舉個例子:
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_IUnknown, (void**)&ptr);
//這裡我們傳遞 IID_IUnknown 和& ptr 以獲取 IUnknown 接口的指針。
//IID_PPV_ARGS 宏可以簡化上面的語法。我們可以寫成 :
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_PPV_ARGS(&ptr));

//這段代碼導入了一些 Direct3D 12 和 Windows 執行時 C++ 模板庫(WRL)的頭文件。
//它定義了 chil::app 命名空間和Run函數。Run函數接收一個 win::IWindow & 參考作為引數。
//在該函數中, 它聲明了一個指向 ID3D12Device2 的 ComPtr 智慧指針。然後它調用 D3D12CreateDevice 函數來創建 Direct3D 12 設備, 傳遞 nullptr 作為配接器和 D3D_FEATURE_LEVEL_12_2 作為最低功能級別。
//D3D12CreateDevice的結果被流入 chk 標記中使用 >> 運算符。這將對 D3D12CreateDevice 返回的 HRESULT 進行錯誤檢查。如果發生錯誤, 它將拋出一個 std::runtime_error, 其中包含有關錯誤的詳細信息。
//總的來說, 這段代碼:
//-聲明了一個指向 ID3D12Device2 Direct3D 12 設備的 ComPtr 智能指針
//- 调用 D3D12CreateDevice 函數來創建該設備
//- 使用 >> 流運算符對 D3D12CreateDevice 的結果進行錯誤檢查
//- 如果發生錯誤, 拋出一個 std::runtime_error 並帶有錯誤詳細信息


/*This code imports some Direct3D 12 and Windows Runtime C++ Template Library(WRL) headers.
It defines a chil::app namespace and a Run function within it.The Run function takes a win::IWindow & reference as an argument.
In the function, it declares a ComPtr smart pointer to ID3D12Device2.It then calls the D3D12CreateDevice function to create a Direct3D 12 device, passing nullptr for the adapter and D3D_FEATURE_LEVEL_12_2 for the minimum feature level.
The result of D3D12CreateDevice is streamed into the chk token using the >> operator. This performs error checking on the HRESULT returned by D3D12CreateDevice.If an error occurs, it will throw a std::runtime_error with details about the error.
In summary, this code:
-Declares a smart pointer ComPtr to a ID3D12Device2 Direct3D 12 device
- Calls D3D12CreateDevice to create the device
- Uses the >> streaming operator to perform error checking on the result of D3D12CreateDevice
- If an error occurs, throws a std::runtime_error with error details
So the purpose of this code is to create a Direct3D 12 device, with error handling and error details provided on failur*/

namespace chil::app
{
	using Microsoft::WRL::ComPtr;

	int Run(win::IWindow& window)
	{
		ComPtr<ID3D12Device2> devices;
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&devices)) >> chk;
	
		return 0;
	}
}