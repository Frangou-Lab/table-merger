/*
 * Copyright 2018 Frangou Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Help_hpp
#define Help_hpp

#include <string>
#include <iostream>

void PrintHelp(FILE *destination)
{
    fprintf(destination, "\
OVERVIEW:\n\
merge_tables v0.1\n\
Build date: %s\n\
\n", __DATE__);
    fprintf(destination,"\
USAGE: merge_tables <input file path> <data matrix path> [-o <output path>]\n\
\n\
OPTIONS:\n\
-h                     - Show this message\n\
-f                     - Override the existing '-merged' file even if it already exists.\n\
EXAMPLES:\n\
merge_tables input_file.csv data_matrix.csv                       - Merge 'data matrix.csv' and an 'input_file.csv' using\n\
                                                                    the first column 'ID'. The output will be located in \n\
                                                                    the same directory as 'data_matrix.csv' file.\n\
merge_tables input_file.csv data_matrix.csv -o ~/merged_file.csv  - Merge 'data matrix.csv' and 'input_file.csv' using\n\
                                                                    the first column (ID). The output will be located at \n\
                                                                    $HOME/merged_file.csv.\n\
");
}

class ArgumentsParser {
 public:
    std::string input_file_path;
    std::string data_matrix_file_path;
    std::string output_file_path;
    
    bool verbose_output{false};
    bool override_output{false};
    
    ArgumentsParser(int argc, const char *argv[])
    {
        if (argc == 1) {
            PrintHelp(stdout);
            std::exit(1);
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string argument = argv[i];

            if (argument == "-v") {
                verbose_output = true;
            } else if (argument == "-o") {
                i++;
                if (i == argc || argv[i][0] == '-') {
                    std::cerr << "No valid output path entered.\n";
                    std::exit(1);
                }
                output_file_path.assign(argv[i]);
            } else if (argument == "-h") {
                PrintHelp(stdout);
                std::exit(0);
            } else if (argument == "-f") {
                override_output = true;
            } else if (argument[0] != '-') {
                if (input_file_path.empty())
                    input_file_path.assign(argument);
                else
                    data_matrix_file_path.assign(argument);
            } else {
                PrintHelp(stderr);
                fprintf(stderr, "Unknown flag '%s'\n", argument.c_str());
                std::exit(1);
            }
        }
    }
};

#endif /* Help_h */
