#ifndef WRAP
#define WRAP

#include <string>
#include <vector>

std::vector<std::string> commonWrap(std::string rawInput, int width) {
    std::vector<std::string> output, processedInput;
    
    // Special case for a column width of 1
    if (width == 1) {
        for (int i = 0; i < rawInput.length(); i++) {
            std::string build = "-";
            build[0] = rawInput[i];

            output.emplace_back(build);
        }

        return output;
    }

    // Turn the input string into a vector containing just the words
    for (int i = 0; rawInput.length() > 0; i++) {
        std::string build = "";

        while ((rawInput[0] != ' ') && (rawInput.length() != 0)) {
            build += rawInput[0];

            rawInput.erase(0, 1);
        }

        processedInput.emplace_back(build);

        rawInput.erase(0, 1);
    }

    // Calculate the amount of items there will be in the output vector
    int characters = 0; 
    for (int i = 0; i < processedInput.size(); i++) {
        for (int j = 0; j < processedInput[i].length(); j++) {
            characters++;
        }
    }
    // Since integers will always round a decimal down, an extra line is added if the result isn't a whole number
    int lines = characters / 38;
    if (characters % width != 0) {
        lines++;   
    }

    // Now indexes can be used for output without worrying about a memory error
    for (int i = 0; i < lines + 1; i++) {
        output.emplace_back("");
    }

    int remainder = width, index = 0;

    // Turn the vector of individual words into a vector of lines where each item's length doesn't exceed the width
    while (processedInput.size() != 0) {        
        if (processedInput[0].length() < remainder) {
            output[index] += processedInput[0] + " ";

            remainder -= (processedInput[0].length() + 1);

            processedInput.erase(processedInput.begin());
        }
        else if (processedInput[0].length() == remainder) {
            output[index] += processedInput[0];

            // Increasing the index acts like a \n if the output were a string
            index++;

            remainder = width;

            processedInput.erase(processedInput.begin());
        }
        else {
            if (output[0].length() > 0) {
                index++;
            }

            if (processedInput[0].length() < width) {
                output[index] += processedInput[0] + " ";

                remainder = width - (processedInput[0].length() + 1);

                processedInput.erase(processedInput.begin());
            }
            else if (processedInput[0].length() == width) {
                output[index] += processedInput[0];

                // Increasing the index acts like a \n if the output were a string
                index++;

                remainder = width;

                processedInput.erase(processedInput.begin());
            }
            else {
                for (int i = 0; i < width - 1; i++) {
                    output[index] += processedInput[0][0];

                    processedInput[0].erase(0, 1);
                }

                output[index] += "-";

                if (processedInput[0].length() < width) {
                    index++;
                }

                remainder = width;
            }
        }
    }

    return output;
}

#endif
