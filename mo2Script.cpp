#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = filesystem;

void processModFiles(const string& modFolder, const string& modPath, const string& modName) {
    for (const auto& entry : fs::directory_iterator(modPath)) {
        if (fs::is_regular_file(entry.status()) && entry.path().filename().string()!="meta.ini") {
            string filePath = entry.path().string();

            // Verifique se o arquivo existe em outros mods
            for (const auto& otherEntry : fs::directory_iterator(modFolder)) {
                if (fs::is_directory(otherEntry.status()) &&
                    otherEntry.path().filename().string() != modName) {
                    string otherModPath = modFolder + "/" + otherEntry.path().filename().string();
                    size_t found = filePath.find(modName);
                    string newFilePath = filePath.substr(found + modName.length());
                    string otherFilePath = otherModPath + newFilePath;

                    if (fs::exists(otherFilePath)){
                        fs::remove(otherFilePath);
                        break;
                    }
                }
            }
        } else if (fs::is_directory(entry.status())) {
            string subDirPath = entry.path().string();

            // Processa os arquivos nos subdiretórios recursivamente
            processModFiles(modFolder, subDirPath, modName);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <modFolder> <orderFile>" << endl;
        return 1;
    }

    string modFolder = argv[1];
    string orderFile = argv[2];

    ifstream file(orderFile);
    if (!file.is_open()) {
        cerr << "Error in open loadorder file" << endl;
        return 1;
    }

    string line;
    getline(file,line);
    while (getline(file, line)) {
        // Remove quebras de linha
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        if (!line.empty()) {
            // Remove o "+" no início do nome do mod
            if(line[0]=='*') return 0;
            if (line[0] == '+') {
                line.erase(0, 1);
            }

            string modPath = modFolder + "/" + line;
            cout << "in mod: " << line << endl;

            // Processa os arquivos no mod e subdiretórios
            processModFiles(modFolder, modPath, line);
        }
    }

    file.close();
    return 0;
}