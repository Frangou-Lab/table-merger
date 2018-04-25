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

#include "Help.hpp"
#include "InputFile.hpp"

#include "../libgene/source/file/sequence/SequenceFile.hpp"
#include "../libgene/source/utils/CppUtils.hpp"
#include "../libgene/source/def/Flags.hpp"
#include "../libgene/source/utils/Tokenizer.hpp"

#include <iostream>

int main(int argc, const char *argv[])
{
    ArgumentsParser arguments(argc, argv);
    
    if (arguments.input_file_path.empty()) {
        std::cerr << "No input files has been provided. See 'help' for reference.\n";
        PrintHelp(stderr);
        return 1;
    }
    
    if (arguments.data_matrix_file_path.empty()) {
        std::cerr << "No data matrix file has been provided. See 'help' for reference.\n";
        PrintHelp(stderr);
        return 0;
    }
    
    InputFile input_file(arguments.input_file_path);

    auto flags = std::make_unique<CommandLineFlags>();
    flags->SetSetting(Flags::kInputFormat, "txt");
    auto data_matrix_file = SequenceFile::FileWithName(arguments.data_matrix_file_path, flags, OpenMode::Read);

    if (!data_matrix_file) {
        std::cerr << "File '" << arguments.input_file_path << "' couldn't be opened. Either it doesn't exist, or you don't have permissions to read it.\n";
        return 1;
    }
    
    size_t dot_position = arguments.data_matrix_file_path.rfind('.');
    std::string extension = utils::GetExtension(arguments.data_matrix_file_path);
    
    if (arguments.output_file_path.empty()) {
        arguments.output_file_path = arguments.input_file_path.substr(0, dot_position - 1) +
                                     "-merged" + "." + extension;
    }
    SequenceRecord input_record;
    if (extension == "csvc" || extension == "tsvc") {
        // Read and skip the first 'columns' line
        input_record = data_matrix_file->Read();
    }

    if (!arguments.override_output) {
        FILE *test_out_file = fopen(arguments.output_file_path.c_str(), "wx");
        
        if (test_out_file == nullptr) {
            std::cout << "File '" << arguments.output_file_path << "' already exists. Do you wish to override it? [Y/n] ";
            char response;
            std::cin >> response;
            if (std::tolower(response) != 'y') {
                std::cerr << "Skipping file '" << arguments.input_file_path << "'\n";
                return 1;
            }
        } else {
            fclose(test_out_file);
        }
    }

    std::unique_ptr<SequenceFile> out_file;
    flags = std::make_unique<CommandLineFlags>();
    flags->SetSetting(Flags::kOutputFormat, "txt");

    if (!(out_file = SequenceFile::FileWithName(arguments.output_file_path,
                                                flags,
                                                OpenMode::Write))) {
        std::cerr << "Couldn't open the output file '" << arguments.output_file_path << "'\n";
        return 1;
    }
    
    char delimiter = ',';
    if (extension == "tsv" || extension == "tsvc")
        delimiter = '\t';
    
    if (!input_record.seq.empty()) {
        // Already has a column description. Transfer it to the output file
        input_record.seq += delimiter;
        input_record.seq += "\"Merged from the input\"";
        out_file->Write(input_record);
    }
    
    while (!(input_record = data_matrix_file->Read()).Empty()) {
        Tokenizer line_tokenizer(input_record.seq, delimiter);

        line_tokenizer.ReadNext();
        std::string id = line_tokenizer.GetNextToken();
        const auto& values = input_file.ValuesForId(id);
        
        input_record.seq += delimiter;
        input_record.seq += '"';
        for (int i = 0; i < values.size(); ++i) {
            input_record.seq += values[i];
            if (i != (values.size() - 1))
                input_record.seq += delimiter;
        }
        input_record.seq += '"';
        out_file->Write(input_record);
    }
    std::cout << "The output file is located at '" << arguments.output_file_path <<  "'\n";
    return 0;
}
