#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256

// Function declarations
void sanitizeInput(char *input);                      // Clean input by removing invalid characters
int romanToDecimal(char *roman, char *errorMsg);      // Convert Roman numerals to decimal, return -1 on error
int performArithmetic(int num1, int num2, char operator); // Perform arithmetic operations on two numbers
void numberToWords(int num, char *output);            // Convert a number to words
void processLine(char *line, FILE *outputFile);       // Process a single line from the input file
void concatenateWords(char *result, const char *chunkStr); // Concatenate chunk's words into the final result
void writeResult(int result, FILE *outputFile);       // Write the result to the output file

// Main function: Reads input from file, processes it, and writes results to output file
int main()
{
    FILE *inputFile = fopen("input.txt", "r");
    FILE *outputFile = fopen("output.txt", "w");

    if (!inputFile || !outputFile)
    {
        printf("Error: Cannot open file.\n");
        return 1;  // Exit with error if files can't be opened
    }

    char line[MAX_LINE_LENGTH];
    // Process each line of the input file
    while (fgets(line, sizeof(line), inputFile))
    {
        sanitizeInput(line);  // Remove invalid characters from the input line
        processLine(line, outputFile); // Process and write the result to the output file
    }

    fclose(inputFile);  // Close the input file
    fclose(outputFile); // Close the output file

    printf("Processing complete. Results are saved.\n");
    return 0; // Successfully exit
}

// Function to clean the input by removing invalid characters
void sanitizeInput(char *input)
{
    char *dst = input;
    // Copy only valid alphanumeric characters and operators
    for (char *src = input; *src != '\0'; src++)
    {
        if (isalnum(*src) || *src == ' ' || *src == '+' || *src == '-' || *src == '*' || *src == '/')
        {
            *dst++ = *src;
        }
    }
    *dst = '\0';  // Null-terminate the sanitized string
}

// Function to write the result (in words) to the output file
void writeResult(int result, FILE *outputFile)
{
    char resultWords[1024];
    numberToWords(result, resultWords);  // Convert the result to words
    fprintf(outputFile, "%s\n", resultWords);  // Write the result to the output file
}

// Convert Roman numeral to decimal with error handling
int romanToDecimal(char *roman, char *errorMsg)
{
    int total = 0, prevValue = 0;

    for (int i = strlen(roman) - 1; i >= 0; i--)
    {
        int currentValue = 0;
        // Convert each Roman numeral character to its decimal value
        switch (roman[i])
        {
            case 'I': currentValue = 1; break;
            case 'V': currentValue = 5; break;
            case 'X': currentValue = 10; break;
            case 'L': currentValue = 50; break;
            case 'C': currentValue = 100; break;
            case 'D': currentValue = 500; break;
            case 'M': currentValue = 1000; break;
            default:
                strcpy(errorMsg, "Invalid Roman numeral character.");
                return -1;  // Return -1 on error
        }

        // Adjust total based on the previous numeral
        if (currentValue < prevValue)
        {
            total -= currentValue;  // Subtract if the current numeral is smaller than the previous
        }
        else
        {
            total += currentValue;  // Otherwise, add it
        }
        prevValue = currentValue;
    }

    return total;  // Return the final decimal value
}

// Perform arithmetic operations (addition, subtraction, multiplication, division)
int performArithmetic(int num1, int num2, char operator)
{
    switch (operator)
    {
        case '+': return num1 + num2;  // Addition
        case '-': return abs(num1 - num2);  // Subtraction (absolute value)
        case '*': return num1 * num2;  // Multiplication
        case '/': return (num2 != 0) ? num1 / num2 : 0;  // Division (avoid divide by zero)
        default: return 0;  // Return 0 for invalid operator
    }
}

// Function to process a chunk of a number and convert it to words
void processChunk(int chunk, int place, char *result)
{
    const char *units[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    char chunkStr[256] = "";  // Temporary string to hold words for the current chunk

    int hundreds = chunk / 100;  // Get hundreds place
    int remainder = chunk % 100;  // Get the remaining part after hundreds

    if (hundreds > 0)
    {
        strcat(chunkStr, units[hundreds]);  // Add the hundreds part
        strcat(chunkStr, " Hundred ");  // Append "Hundred"
    }

    if (remainder >= 10 && remainder < 20)
    {
        strcat(chunkStr, teens[remainder - 10]);  // Handle numbers 10-19
    }
    else
    {
        int tensPlace = remainder / 10;  // Tens place
        int onesPlace = remainder % 10;  // Ones place

        if (tensPlace > 0)
        {
            strcat(chunkStr, tens[tensPlace]);  // Add tens place
        }
        if (onesPlace > 0)
        {
            if (tensPlace > 0)
                strcat(chunkStr, " ");  // Add space between tens and ones
            strcat(chunkStr, units[onesPlace]);  // Add ones place
        }
    }

    // Add the place value (e.g., Thousand, Million)
    if (place > 0)
    {
        strcat(chunkStr, " ");
        strcat(chunkStr, thousands[place]);
    }

    concatenateWords(result, chunkStr);  // Add chunk's words to the final result
}

// Helper function to concatenate words into the final result string
void concatenateWords(char *result, const char *chunkStr)
{
    if (strlen(result) > 0)
    {
        strcat(result, " ");  // Add a space if result already contains words
    }
    strcat(result, chunkStr);  // Append the chunk to the result
}

// Convert a number to words using chunks (3 digits at a time)
void numberToWords(int num, char *output)
{
    if (num == 0)
    {
        strcpy(output, "Zero");  // Special case for zero
        return;
    }

    char result[1024] = "";  // String to store the final result
    int place = 0;

    // Process the number in chunks of 3 digits
    while (num > 0)
    {
        int chunk = num % 1000;  // Get the last 3 digits
        if (chunk > 0)
        {
            processChunk(chunk, place, result);  // Process each chunk
        }
        num /= 1000;  // Remove the last 3 digits
        place++;  // Move to the next higher place (thousands, millions, etc.)
    }

    strcpy(output, result);  // Copy the final result to output
}

// Function to process a single line from the input file
void processLine(char *line, FILE *outputFile)
{
    char roman1[64], roman2[64], operator;
    if (sscanf(line, "%s %c %s", roman1, &operator, roman2) != 3)
    {
        fprintf(outputFile, "Invalid input format. Please ensure Roman numerals and operators are correct.\n");
        return;
    }

    char errorMsg[256];
    int num1 = romanToDecimal(roman1, errorMsg);  // Convert the first Roman numeral
    int num2 = romanToDecimal(roman2, errorMsg);  // Convert the second Roman numeral

    if (num1 == -1 || num2 == -1)
    {
        fprintf(outputFile, "%s\n", errorMsg);  // Log error message if conversion fails
        return;
    }

    int result = performArithmetic(num1, num2, operator);  // Perform arithmetic operation
    writeResult(result, outputFile);  // Write result in words to the output file
}
