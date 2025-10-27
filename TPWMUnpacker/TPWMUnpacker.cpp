// TPWMUnpacker.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <fstream>
#include <vector>


/*
The packing algorithm shortens repeating byte sequences with a length between 3 and 18 bytes to 2 bytes.
In these 2 bytes, the length of the sequence and the relative offset (max. 0xFFF bytes) is recorded.
Longer sequences are accordingly composed of several such bytes with unpacking information.
All other byte sequences remain unchanged.
In order to determine during unpacking where in the packed file an unpacking information is located instead of normal data bytes,
the packer inserts an extra byte whose bits for the next 8 bytes indicate whether they are data bytes or unpacking information.
*/
static int Unpack_TPWM(char* Input_Buffer, unsigned long packed_size, char* Output_Buffer, unsigned long unpacked_size)
{
    unsigned char b1, b2;
    unsigned short packbyte;
    int bit, i;
    unsigned long input_offset = 0;
    unsigned long output_offset = 0;
    unsigned int distance, length;

    while ((input_offset < packed_size) && (output_offset < unpacked_size))
    {
        // Lire l'octet de contrôle (flag byte)
        packbyte = (unsigned char)Input_Buffer[input_offset++];

        for (bit = 0; bit < 8; bit++)
        {
            if ((input_offset >= packed_size) || (output_offset >= unpacked_size))
                break;

            // Décale le bit courant pour savoir si c'est une commande (bit = 1) ou une donnée brute (bit = 0)
            if (packbyte & 0x80) // bit de poids fort
            {
                if (input_offset + 1 >= packed_size)
                    return -1; // Pas assez de données pour lire b1 et b2

                b1 = (unsigned char)Input_Buffer[input_offset++];
                b2 = (unsigned char)Input_Buffer[input_offset++];

                // Longueur = (b1 & 0x0F) + 2
                length = (b1 & 0x0F) + 2;

                // Distance = b2 + ((b1 & 0xF0) << 4)
                distance = b2 | ((b1 & 0xF0) << 4);

                // Copier depuis les données déjà décompressées
                for (i = 0; i <= length; i++)
                {
                    if (output_offset >= unpacked_size || output_offset < distance)
                        return -1; // Erreur de dépassement de tampon

                    Output_Buffer[output_offset] = Output_Buffer[output_offset - distance];
                    output_offset++;
                }
            }
            else
            {
                // Donnée brute à copier telle quelle
                if (input_offset >= packed_size || output_offset >= unpacked_size)
                    return -1;

                Output_Buffer[output_offset++] = Input_Buffer[input_offset++];
            }

            packbyte <<= 1; // Passe au bit suivant
        }
    }

    // Vérification finale
    if (output_offset != unpacked_size)
        return -1;

    return 0; // Succès
}

// Utilitaire pour lire un entier 32 bits little-endian à partir d’un buffer
static unsigned long read_le_uint32(const char* buf) {
    return ((unsigned char)buf[0]) |
        ((unsigned char)buf[1] << 8) |
        ((unsigned char)buf[2] << 16) |
        ((unsigned char)buf[3] << 24);
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        std::fstream fsin, fsout;
        fsin.open(argv[1], std::fstream::in | std::fstream::binary);
        // Lis tout le fichier dans un vecteur
        std::vector<char> input_buffer((std::istreambuf_iterator<char>(fsin)), std::istreambuf_iterator<char>());

        // Files compressed with this packer can be easily recognized by an ID string "TPWM" directly at the beginning of the file.
        if(strncmp(input_buffer.data(), "TPWM", 4) == 0)
        {
            std::string outputfilename(argv[1]);
            outputfilename += ".out";
            fsout.open(outputfilename, std::fstream::out | std::fstream::trunc | std::fstream::binary);

            // This ID string is followed by four bytes, which is the size of the unpacked data...
            unsigned long unpacked_size = read_le_uint32(input_buffer.data() + 4);

            auto output_buffer = std::make_unique<char[]>(unpacked_size);
            std::memset(output_buffer.get(), 0, unpacked_size);

            // The size of the packed data thus results from the file size - 8
            const auto packed_size = input_buffer.size() - 8;

            // ... and then the packed data itself.
            const auto ret = Unpack_TPWM(input_buffer.data() + 8, packed_size, output_buffer.get(), unpacked_size);

            if (ret != -1)
            {
                fsout.write(output_buffer.get(), unpacked_size);
                fsout.flush();
            }
            fsout.close();
        }

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
