#pragma once
#include <source_location>

//������ CheckerToken �Y���w, ���ƺ�������һ�N��ӛ��C�ơ�
//Ȼ���� HrGrabber �Y���w, �������ɂ��ɆT:
//-hr : һ�� unsigned int ��͵ę�λ
//- loc : һ�� std::source_location ��͵ę�λ, ���ӛ䛴��a��λ����Ϣ
//HrGrabber ߀���x��һ�����캯��, �ژ���rӛ䛴��aλ�á�
//���ᶨ�x��һ�����Ʋ����� >> , ���� HrGrabber �� CheckerToken ���������
//���w��, �@�δ��a�������ƺ��� :
//	���xһ�N�C�ƻ��ӛ CheckerToken, ��һ�N�����aλ��ӛ䛹��ܵ� HrGrabber �Y���w��
//	���\�е� CheckerToken ��ӛ�r, ����ʹ�� HrGrabber �Y���wӛ䛴˕r�Ĵ��aλ��, �K����ĳЩ����(>> ������)��
//	�@������춴��a���e�`�z�y���ɜy��̎�����\��ĳ�����л��׳��e���aǰ����� CheckerToken, һ�����e����ӛ�λ�ÁK��������̎��

//This code defines a namespace called chil::app and defines two structures and an operator in it.
//First, the CheckerToken structure seems to be used as a mark or mechanism.
//Then the HrGrabber structure contains two members :
//-hr : An unsigned int field
//- loc : A std::source_location type field used to record the location information of the code
//HrGrabber also defines a constructor that records the code location when constructing.
//Finally, a right shift operator >> is defined with HrGrabber and CheckerToken as operands.
//Overall, the purpose of this code appears to be :
//Define a mechanism or mark CheckerToken, and a HrGrabber structure with code location recording function.
//When the CheckerToken mark is reached, the code location can be recorded using the HrGrabber structure, and some operations(>> operator) can be performed.
//This can be used for error detection, detection and handling of code.Place CheckerToken before and after a sensitive or error - prone code segment, and once an error occurs, the location can be recorded and corresponding handling can be performed.

namespace chil::app
{
	// CreateDeviceOrWhatever(...) >> chk;
	struct CheckerToken {} chk;
	struct HrGrabber
	{
		HrGrabber(unsigned int hr, std::source_location = std::source_location::current()) noexcept;
		unsigned int hr;
		std::source_location loc;
	};
	void operator>>(HrGrabber, CheckerToken);

}