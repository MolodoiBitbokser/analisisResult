#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<xstring>

using namespace std;

const int N = 3; // Size of matrices

void decodePairName(string pairName, int& i, int& j) {
    try {
        i = stoi(pairName.substr(0, 1));
        j = stoi(pairName.substr(1, 1));
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        // Handle the error appropriately, such as setting i and j to default values
        i = 0;
        j = 0;
    }
}

// Function to multiply a matrix by a subgroup
vector<vector<int>> multiplyMatrix(vector<vector<int>> matrix, int subgroup) {
    vector<vector<int>> result(N, vector<int>(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = matrix[(static_cast<std::vector<std::vector<int, std::allocator<int>>, std::allocator<std::vector<int, std::allocator<int>>>>::size_type>(i) + j) % N]
                [(static_cast<std::vector<int, std::allocator<int>>::size_type>(i) * j + subgroup) % N];
        }
    }
    return result;
}

// Function to check if a pair of matrices is a duplicate
bool isDuplicate(vector<vector<int>> matrix1, vector<vector<int>> matrix2) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (matrix1[i][j] != matrix2[i][j]) {
                return false;
            }
        }
    }
    return true;
}

int main()
{
    // Open matrices.txt file
    ifstream matricesFile("matrices.txt");
    if (!matricesFile.is_open()) {
        cout << "Error: could not open matrices.txt file" << endl;
        return 1;
    }

    // Read matrices from file
    vector<vector<int>> matrices;
    vector<int> matrix(N * N);
    string line;
    while (getline(matricesFile, line)) {
        if (line.empty()) {
            continue;
        }
        int pos = 0;
        for (int i = 0; i < N; i++) {
            getline(matricesFile, line);
            for (int j = 0; j < N; j++) {
                int num = 0;
                if (!line.empty()) {
                    try {
                        if (static_cast<unsigned long long>(pos) + 1 <= line.size()) {
                            num = stoi(line.substr(pos, 1));
                            matrix[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) * N + j] = num;
                            pos += 2;
                        }
                        else {
                            // Handle the error appropriately, such as setting num to a default value
                            num = 0;
                        }
                    }
                    catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid argument: " << e.what() << std::endl;
                        // Handle the error appropriately, such as setting num to a default value
                        num = 0;
                    }
                }
            }
        }
        matrices.push_back(matrix);
    }


    // Close matrices.txt file
    matricesFile.close();

    // Open knownDuplicates.txt file
    ifstream knownDuplicatesFile("knownDuplicates.txt");
    if (!knownDuplicatesFile.is_open()) {
        cout << "Error: could not open knownDuplicates.txt file" << endl;
        return 1;
    }

    // Open ResultAnalysis.txt file
    ofstream resultAnalysisFile("ResultAnalysis.txt");
    if (!resultAnalysisFile.is_open()) {
        cout << "Error: could not open ResultAnalysis.txt file" << endl;
        return 1;
    }

    // Process each line in the file
    while (getline(knownDuplicatesFile, line)) {
        // Decode pair names into subgroups
        string pairName1 = line.substr(0, 2);
        string pairName2 = line.substr(5, 2);
        int i1, j1, i2, j2;
        decodePairName(pairName1, i1, j1);
        decodePairName(pairName2, i2, j2);

        // matrices corresponding to subgroups
        vector<vector<int>> matrix1, matrix2;
        for (int i = 0; i < matrices.size(); i++) {
            if (matrices[i][0] == i1 && matrices[i][1] == j1) {
                matrix1 = vector<vector<int>>(N, vector<int>(N));
                for (int k = 0; k < N; k++) {
                    for (int l = 0; l < N; l++) {
                        matrix1[k][l] = matrices[i][static_cast<std::vector<int, std::allocator<int>>::size_type>(k) * N + l];
                    }
                }
            }
            if (matrices[i][0] == i2 && matrices[i][1] == j2) {
                matrix2 = vector<vector<int>>(N, vector<int>(N));
                for (int k = 0; k < N; k++) {
                    for (int l = 0; l < N; l++) {
                        matrix2[k][l] = matrices[i][static_cast<std::vector<int, std::allocator<int>>::size_type>(k) * N + l];
                    }
                }
            }

            // If matrices are found, identify the ratio for the forming matrices
            if (!matrix1.empty() && !matrix2.empty()) {
                while (!isDuplicate(matrix1, matrix2)) {
                    matrix1 = multiplyMatrix(matrix1, i1 + j1);
                    matrix2 = multiplyMatrix(matrix2, i2 + j2);
                }
                resultAnalysisFile << pairName1 << " = " << pairName2 << " is a duplicate" << endl;
                resultAnalysisFile << "The ratio for the forming matrices is:" << endl;
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        resultAnalysisFile << matrix1[i][j] / matrix2[i][j] << " ";
                    }
                    resultAnalysisFile << endl;
                }
                resultAnalysisFile << endl;
            }
        }

        // Close knownDuplicates.txt file
        knownDuplicatesFile.close();

        // Close ResultAnalysis.txt file
        resultAnalysisFile.close();

        return 0;
    }
}
