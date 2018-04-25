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

#ifndef InputFile_hpp
#define InputFile_hpp

#include "../libgene/source/file/sequence/SequenceFile.hpp"
#include "../libgene/source/flags/CommandLineFlags.hpp"
#include "../libgene/source/utils/Tokenizer.hpp"
#include "../libgene/source/utils/StringUtils.hpp"
#include "../libgene/source/def/Flags.hpp"

#include <string>
#include <map>
#include <vector>
#include <iostream>

class InputFile {
 private:
    std::map<std::string, std::vector<std::string>> id_values_pairs_;
    const std::vector<std::string> empty_string_vector;
    
 public:
    InputFile(const std::string &file_path)
    {
        auto flags = std::make_unique<CommandLineFlags>();
        flags->SetSetting(Flags::kInputFormat, "txt");

        auto input_file = SequenceFile::FileWithName(file_path,
                                                     flags,
                                                     OpenMode::Read);
        if (input_file == nullptr) {
            std::cerr << "Unable to open the input file." << std::endl;
            std::terminate();
        }
        std::string extension = utils::GetExtension(file_path);
        char delimiter = ',';
        if (extension == "tsv" || extension == "tsvc")
            delimiter = '\t';
        
        if (extension == "csvc" || extension == "tsvc") {
            // Either 'csvc' or 'tsvc'
            // Skip the first column definition record
            input_file->Read();
        }
        
        Tokenizer splitter(delimiter);
        SequenceRecord line;

        while (!(line = input_file->Read()).Empty()) {
            splitter.SetText(line.seq);
            std::string id;
            std::string value;

            splitter.ReadNext();
            id = splitter.GetNextToken();

            splitter.ReadNext();
            value = splitter.GetNextToken();

            id_values_pairs_[id].emplace_back(value);
        }
    }
    
    const std::vector<std::string>& ValuesForId(std::string id) const
    {
        if (id_values_pairs_.find(id) == id_values_pairs_.end())
            return empty_string_vector;
        
        return id_values_pairs_.at(id);
    }
};

#endif /* InputFile_hpp */
