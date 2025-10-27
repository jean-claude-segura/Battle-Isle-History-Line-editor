#define _CRT_SECURE_NO_WARNINGS

// XOR70.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
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

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		std::fstream fsin, fsout;
		fsin.open(argv[1], std::fstream::in | std::fstream::binary);
		// Lis tout le fichier dans un vecteur
		std::vector<char> input_buffer((std::istreambuf_iterator<char>(fsin)), std::istreambuf_iterator<char>());

		std::string outputfilename(argv[1]);
		outputfilename += ".x70";
		fsout.open(outputfilename, std::fstream::out | std::fstream::trunc | std::fstream::binary);

		const auto unpacked_size = input_buffer.size();

		auto output_buffer = std::make_unique<char[]>(unpacked_size);
		std::memset(output_buffer.get(), 0, unpacked_size);

		const auto ret = XOR70(input_buffer.data(), unpacked_size, output_buffer.get());

		if (ret != -1)
		{
			fsout.write(output_buffer.get(), unpacked_size);
			fsout.flush();
		}
		fsout.close();

		fsin.close();
	}
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
