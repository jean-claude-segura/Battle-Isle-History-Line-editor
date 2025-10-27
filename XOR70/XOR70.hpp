#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>

static int XOR70(char* Input_Buffer, unsigned long packed_size, char* Output_Buffer)
{
	auto tempin = Input_Buffer;
	auto tempout = Output_Buffer;
	const char sep_pattern[] = { 0x02, 0x01, 0x01, 0x00, 0x00 };
	const char sep_pattern_multi[] = { 0x02, 0x02, 0x01, 0x00, 0x00 };
	const char sep_pattern_split[] = { 0x02, 0x01, 0x00, 0x01, 0x00 };
	const char sep_pattern_split_end[] = { 0x02, 0x02, 0x00, 0x00, 0x00 };
	for (unsigned long i = 0; i < packed_size; ++i, ++tempin, ++tempout)
	{
		if (strncmp(tempin, sep_pattern, 5) == 0)
		{
			strncpy(tempout, "[1]\r\n", 5);
			i += 4;
			tempin += 4;
			tempout += 4;
		}
		else if (strncmp(tempin, sep_pattern_multi, 5) == 0)
		{
			strncpy(tempout, "[2]\r\n", 5);
			i += 4;
			tempin += 4;
			tempout += 4;
		}
		else if (strncmp(tempin, sep_pattern_split, 5) == 0)
		{
			strncpy(tempout, "[3]\r\n", 5);
			i += 4;
			tempin += 4;
			tempout += 4;
		}
		else if (strncmp(tempin, sep_pattern_split_end, 5) == 0)
		{
			strncpy(tempout, "[4]\r\n", 5);
			i += 4;
			tempin += 4;
			tempout += 4;
		}
		else
			*tempout = *tempin ^ 0x70;
	}

	return 0; // Succès
}

int XOR70File(const std::string strFileName, const std::string strExt = ".x70")
{
	std::fstream fsin, fsout;
	fsin.open(strFileName, std::fstream::in | std::fstream::binary);

	int ret = -1;

	if(fsin.is_open())
	{
		// Lis tout le fichier dans un vecteur
		std::vector<char> input_buffer((std::istreambuf_iterator<char>(fsin)), std::istreambuf_iterator<char>());

		std::string outputfilename(strFileName);
		outputfilename += ".x70";
		fsout.open(outputfilename, std::fstream::out | std::fstream::trunc | std::fstream::binary);

		if (fsout.is_open())
		{
			const auto unpacked_size = input_buffer.size();

			auto output_buffer = std::make_unique<char[]>(unpacked_size);
			std::memset(output_buffer.get(), 0, unpacked_size);

			ret = XOR70(input_buffer.data(), unpacked_size, output_buffer.get());

			if (ret != -1)
			{
				fsout.write(output_buffer.get(), unpacked_size);
				fsout.flush();
			}
			fsout.close();
		}

		fsin.close();
	}

	return ret;
}
