#include <d3d12.h>
#include <wrl.h>
#include "App.h"
#include "GraphicsError.h"

// IID_PPV_ARGS: __uuidof(**(&devices)), IID_PPV_ARGS_Helper(&devices)

//IID_PPV_ARGS ��һ����, �����ں����Ľӿګ@ȡָᘵ��Z����
//�� COM �����, �҂�������Ҫ��ӿڵķ������f�ӿ� ID(IID)�ͽӿ�ָ�׃���ĵ�ַ(&args), �ԫ@ȡԓ�ӿڵ�ָᘡ��e������:
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_IUnknown, (void**)&ptr);
//�@�e�҂����f IID_IUnknown ��& ptr �ԫ@ȡ IUnknown �ӿڵ�ָᘡ�
//IID_PPV_ARGS ����Ժ���������Z�����҂����Ԍ��� :
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_PPV_ARGS(&ptr));

//�@�δ��a������һЩ Direct3D 12 �� Windows ���Еr C++ ģ���(WRL)���^�ļ���
//�����x�� chil::app �������g��Run������Run��������һ�� win::IWindow & ��������������
//��ԓ������, ������һ��ָ�� ID3D12Device2 �� ComPtr �ǻ�ָᘡ�Ȼ�����{�� D3D12CreateDevice �����턓�� Direct3D 12 �O��, ���f nullptr ����������� D3D_FEATURE_LEVEL_12_2 ������͹��ܼ��e��
//D3D12CreateDevice�ĽY�������� chk ��ӛ��ʹ�� >> �\������@���� D3D12CreateDevice ���ص� HRESULT �M���e�`�z�顣����l���e�`, ��������һ�� std::runtime_error, ���а������P�e�`��Ԕ����Ϣ��
//���ā��f, �@�δ��a:
//-����һ��ָ�� ID3D12Device2 Direct3D 12 �O��� ComPtr ����ָ�
//- ���� D3D12CreateDevice �����턓��ԓ�O��
//- ʹ�� >> ���\����� D3D12CreateDevice �ĽY���M���e�`�z��
//- ����l���e�`, ����һ�� std::runtime_error �K�����e�`Ԕ����Ϣ


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