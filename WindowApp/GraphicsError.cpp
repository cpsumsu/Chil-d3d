#include "GraphicsError.h"
#include <Core/src/win/Utilities.h> //ToNarrow
#include <Core/src/utl/String.h>
#include <ranges>
#include <format> // std::format

namespace rn = std::ranges;
namespace vi = std::views;

// FAILED : (((HRESULT)(grabber.hr)) < 0)
// 

	/*首先, 它定義了一個FAILED宏, 用於檢查HrGrabber中記錄的HRESULT值是否小於0, 即是否代表失敗或錯誤。
	然後, 在HrGrabber的 >> 操作符函數中, 使用FAILED宏檢查hr欄位的值。如果小於0, 即代表發生錯誤, 則:
	1. 獲取錯誤描述信息, 過濾\r和\n字符, 轉為std::string。
	2. 拼接錯誤信息、錯誤發生的文件名和行數, 並丟出一個std::runtime_error異常。
	3. 若hr值正常, 則不進行任何操作。
	所以, 這段代碼在原有的錯誤檢測機制上, 新增了對錯誤結果的解析和異常拋出處理。一旦檢測到錯誤, 可以獲取可讀的錯誤信息和精確的錯誤位置, 並以異常的形式向調用端報告。*/


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