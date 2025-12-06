#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstdio>

using namespace std;

// генератор количества файлов
int CountFiles(int min_range, int max_range, mt19937 &gen){
    uniform_int_distribution<> cnt_files(min_range, max_range);
    return cnt_files(gen);
}

// генератор содержимого файлов
string ContentFiles(int ex, mt19937 &gen){
    uniform_int_distribution<> count_char(5, 7);
    uniform_int_distribution<> content_char(97, 122);
    uniform_int_distribution<> count_string(3, 5);
    string word;
    ostringstream oss;
    oss << "id,name";
    if (ex == 0){ 
        int len = count_char(gen);
        for(int i=0; i<len; i++) word += (char)content_char(gen);
        return word;
    } else { 
        int rows = count_string(gen);
        for (int i = 1; i <= rows; i++){
            word.clear(); 
            int len = count_char(gen);
            for(int j=0; j<len; j++){
                word += (char)content_char(gen);
            }
            oss << "\\n" << i << "," << word;
        }
        return oss.str();
    }
}

// основная функция генерации JSON-файлов
void GenerateJSON(int min_files, int max_files) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> ext(0, 1);
    uniform_int_distribution<> count_json(5, 10);

    int count_files = CountFiles(min_files, max_files, gen);
    int cnt_json = count_json(gen); 
    int count_in_every = count_files / cnt_json;
    int count_in_last = count_files % cnt_json;

    for (int i = 1; i <= cnt_json; i++) {
        string json_name = "../data/data" + to_string(i) + ".json";
        ofstream fout(json_name); 
        fout << "[\n";

        int files_in_this_json = (i != cnt_json) ? count_in_every : count_in_every + count_in_last;

        for (int j = 1; j <= files_in_this_json; j++){
            int ext_ext = ext(gen);
            string file_name = "file_" + to_string(j+count_in_every*(i-1)) + (ext_ext == 1 ? ".csv" : ".txt");
            string content = ContentFiles(ext_ext, gen);

            fout << "{\"name\":\"" << file_name << "\",\"content\":\"" << content << "\"}";

            if (j != files_in_this_json) fout << ","; 
            fout << "\n";
        }

        fout << "]";            
        fout.close();             
    }
}
