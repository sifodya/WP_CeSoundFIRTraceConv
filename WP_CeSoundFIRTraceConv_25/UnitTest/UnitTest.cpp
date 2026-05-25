#include "pch.h"
#include "CppUnitTest.h"
//#include "WP_CeSoundFIRTraceConv_24FX.h"
//#include "WP_CeSoundFIRTraceConv_24FX.cpp"
//#include <AK/SoundEngine/Common/AkCommonDefs.h>
//#include <memory>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:

		CsC parseComplex(const std::string& s)
		{
			// Find separator between real and imaginary part
			size_t plusPos = s.find('+');
			size_t minusPos = s.find('-', 1);

			size_t splitPos;

			if (plusPos != std::string::npos)
				splitPos = plusPos;
			else
				splitPos = minusPos;

			float real = std::stof(s.substr(0, splitPos));

			std::string imagStr = s.substr(splitPos);

			// Remove trailing 'i'
			imagStr.pop_back();

			float imag = std::stof(imagStr);

			return CsC(real, imag);
		}
		TEST_METHOD(TestInitialiseFDLWithFilter)
		{
			
			WP_CeSoundFIRTraceConv_24FX* testObject = new WP_CeSoundFIRTraceConv_24FX();

			testObject->m_bufferSize = 512;
			CsVector mockBuffer;
			CsVector2C expectedResult;

			std::fstream file("C:\\Users\\cedri\\Desktop\\output3.txt");
			std::fstream fileResult("C:\\Users\\cedri\\Desktop\\ComplexResult.csv");
			double x;
			while (file >> x)
			{
				mockBuffer.emplace_back(x);
			}

			std::string line;
			while (std::getline(fileResult, line)) {

				std::stringstream ss(line);

				std::string cell;

				std::vector<CsC> row;

				while (std::getline(ss, cell, ',')) {
					row.push_back(parseComplex(cell));
				}

				expectedResult.push_back(row);
			}

			file.close();
			fileResult.close();

			testObject->initialiseFDLWithFilter(mockBuffer);

			Assert::AreEqual(testObject->m_FDL_H.size(), expectedResult.size(), L"Row mismatch");

			float tolerance = 1e-5f;

			for (auto i{ 0 }; i < expectedResult.size(); i++)
			{
				Assert::AreEqual(testObject->m_FDL_H[i].size(), expectedResult[i].size(), L"Col mismatch");
				for (auto j{ 0 }; j < expectedResult[i].size(); j++)
				{
					Assert::AreEqual(expectedResult[i][j].real(), testObject->m_FDL_H[i][j].real(), tolerance, L"Real part mismatch");
					Assert::AreEqual(expectedResult[i][j].imag(), testObject->m_FDL_H[i][j].imag(), tolerance, L"Imaginary part mismatch");
				}
			}
		}
	};
}
