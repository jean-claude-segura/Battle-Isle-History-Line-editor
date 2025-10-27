#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>

static int XOR70(std::istream & fsin, std::ostream & fsout)
{
	// Lis tout le fichier dans un vecteur
	std::vector<char> vin((std::istreambuf_iterator<char>(fsin)), std::istreambuf_iterator<char>());

	char* tempin = vin.data();
	auto filesize = vin.size();
	const char sep_pattern[] = { 0x02, 0x01, 0x01, 0x00, 0x00 };
	const char sep_pattern_multi[] = { 0x02, 0x02, 0x01, 0x00, 0x00 };
	const char sep_pattern_split[] = { 0x02, 0x01, 0x00, 0x01, 0x00 };
	const char sep_pattern_split_end[] = { 0x02, 0x02, 0x00, 0x00, 0x00 };
	for (size_t i = 0; i < filesize; ++i, ++tempin)
	{
		if (strncmp(tempin, sep_pattern, 5) == 0)
		{
			fsout << "[1]" << std::endl;
			i += 4;
			tempin += 4;
		}
		else if (strncmp(tempin, sep_pattern_multi, 5) == 0)
		{
			fsout << "[2]" << std::endl;
			i += 4;
			tempin += 4;
		}
		else if (strncmp(tempin, sep_pattern_split, 5) == 0)
		{
			fsout << "[3]" << std::endl;
			i += 4;
			tempin += 4;
		}
		else if (strncmp(tempin, sep_pattern_split_end, 5) == 0)
		{
			fsout << "[4]" << std::endl;
			i += 4;
			tempin += 4;
		}
		else
		{
			char xored = *tempin ^ 0x70;
			fsout.write(&xored, 1);
		}
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
		std::string outputfilename(strFileName);
		outputfilename += ".x70";
		fsout.open(outputfilename, std::fstream::out | std::fstream::trunc | std::fstream::binary);

		if (fsout.is_open())
		{			
			ret = XOR70(fsin, fsout);

			if (ret != -1)
			{
				fsout.flush();
				fsout.close();
			}
			else
			{
				// Should delete the file here.
				fsout.close();
			}
		}

		fsin.close();
	}

	return ret;
}
