#include "GraphicsError.h"
#include <Core/src/win/Utilities.h> //ToNarrow
#include <Core/src/utl/String.h>
#include <ranges>
#include <format> // std::format

namespace rn = std::ranges;
namespace vi = std::views;

// FAILED : (((HRESULT)(grabber.hr)) < 0)
// 

	/*����, �����x��һ��FAILED��, ��춙z��HrGrabber��ӛ䛵�HRESULTֵ�Ƿ�С�0, ���Ƿ����ʧ�����e�`��
	Ȼ��, ��HrGrabber�� >> ������������, ʹ��FAILED��z��hr��λ��ֵ�����С�0, ������l���e�`, �t:
	1. �@ȡ�e�`������Ϣ, �^�V\r��\n�ַ�, �D��std::string��
	2. ƴ���e�`��Ϣ���e�`�l�����ļ������Д�, �K�G��һ��std::runtime_error������
	3. ��hrֵ����, �t���M���κβ�����
	����, �@�δ��a��ԭ�е��e�`�z�y�C����, �����ˌ��e�`�Y���Ľ����ͮ�������̎��һ���z�y���e�`, ���ԫ@ȡ���x���e�`��Ϣ�;��_���e�`λ��, �K�Ԯ�������ʽ���{�öˈ�档*/


//This code extends the previous code and further improves error detection and handling functions.
//First, it defines a FAILED macro to check if the HRESULT value recorded in HrGrabber is less than 0, indicating failure or error.
//Then, in the >> operator function of HrGrabber, use the FAILED macro to check the value of the hr field.If it is less than 0, indicating an error has occurred, then:
//1. Get the error description information, filter \r and \n characters, and convert to std::string.
//2. Stitch the error information, file name where the error occurred and line number, and throw a std::runtime_error exception.
//3. If the hr value is normal, do nothing.
//Therefore, this code adds parsing of error results and exception throwing processing to the original error detection mechanism.Once an error is detected, readable error information and accurate error location can be obtained, and the calling end can be reported in the form of an exception.
//Overall, now this mechanism can detect errors in code segments, record locations, and provide readable error feedback, which greatly improves the robustness and testability of the code.

namespace chil::app
{
	CheckerToken chk;

	HrGrabber::HrGrabber(unsigned int hr, std::source_location loc) noexcept
		:
		hr(hr),
		loc(loc)
	{}
	void operator>>(HrGrabber g, CheckerToken)
	{
		if (FAILED(g.hr))
		{
			// get error description as narrow string with crlf removed
			auto errorString = utl::ToNarrow(win::GetErrorDescription(g.hr)) |
				vi::transform([](char c) {return c == '\n' ? ' ' : c; }) |
				vi::filter([](char c) {return c != '\r'; }) |
				rn::to<std::basic_string>();

			throw std::runtime_error(
				std::format("Graphic Error: {} \n	{}({})",
					errorString, g.loc.file_name(), g.loc.line()
			));
		}
	}
}