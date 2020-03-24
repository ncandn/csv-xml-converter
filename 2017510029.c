#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *replacement(const char *str, const char *old, const char *new) {
    char *result;
    int i, count = 0;
    int newlength = strlen(new);
    int oldlength = strlen(old);

    for (i = 0; str[i] != '\0'; i++) // Quantity of the occurrence of the word in the string.
    {
        if (strstr(&str[i], old) == &str[i])
        {
            count++;
            // Jumping to index after the old word.
            i += oldlength - 1;
        }
    }
    // Making new string of enough length.
    result = (char *)malloc(i + count * (newlength - oldlength) + 1);
    i = 0;
    while (*str)
    {
        // Compare the item with the result.
        if (strstr(str, old) == str)
        {
            strcpy(&result[i], new);
            i += newlength;
            str += oldlength;
        }
        else
            result[i++] = *str++;
    }
    result[i] = '\0';
    return result;
}

int main(int argc, char *argv[]) {
    // Creating files and declaring default variables. (titles, rows etc.)
    char delim[2] = ",";
    char opsys[3] = "\n";
    char old[] = ",,";
    char new[] = ",*,";
    char fp_name[strlen(argv[1])];
    char xml_name[strlen(argv[2])];
    strcpy(fp_name, argv[1]);
    strcpy(xml_name, argv[2]);
    char str_name[strlen(argv[2]) - 4];
    strcpy(str_name, strtok(argv[2],"."));
    // Argument order.
    int delim_num = 0;
    int opsys_num = 0;

    // Checking which argument holds the separator value and opsys value.
    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], "-separator") == 0){
            delim_num = i + 1;
        }
        if (strcmp(argv[i], "-opsys") == 0){
            opsys_num = i + 1;
        }
    }

    // Re-assigning separators according to the given argument.
    if (strcmp(argv[delim_num],"1") == 0) {
        strcpy(delim, ",");
        strcpy(old, ",,");
        strcpy(new,",*,");
    }
    else if (strcmp(argv[delim_num],"2") == 0) {
        strcpy(delim, "\t");
        strcpy(old, "\t\t");
        strcpy(new,"\t*\t");
    }
    else if (strcmp(argv[delim_num],"3") == 0) {
        strcpy(delim, ";");
        strcpy(old, ";;");
        strcpy(new,";*;");
    }
    else
        puts("Invalid separator.");


    // Re-formatting the csv file according to the given argument.
    if (strcmp(argv[opsys_num],"1") == 0)
        strcpy(opsys, "\r\n");
    else if (strcmp(argv[opsys_num],"2") == 0)
        strcpy(opsys, "\n");
    else if (strcmp(argv[opsys_num],"3") == 0)
        strcpy(opsys, "\r");
    else
        puts("Invalid format.");

    // Help & Information.
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i],"-h") == 0)
            puts("ARGUMENT INFO\n"
                 "-separator > defines separator between each datum (1 = comma, 2 = tab, 3 = semicolon)\n"
                 "-opsys > defines the end of line character format (1 = windows, 2 = linux, 3 = macos)\n");

    }

    // Initializing xml structure and file operations.
    char titles[500][500]; // Creating a title array for a dynamic xml file.
    int title_count = 0;
    int row = 1; // Start of the increasing row count.
    FILE *xml_file = fopen(xml_name, "w");
    FILE *fp = fopen(fp_name, "r");
    fprintf(xml_file,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n");
    char str[1000]; // Line read from the csv file.

    if (fp == NULL){ // Error check.
        printf("Could not open file.");
        return 1;
    }

    while (fgets(str,1000,fp) != NULL) { // Dynamic titles.

        char *item = strtok(str, delim); // Splitting the line into a char pointer.
        while(item != NULL){ // Navigating through the line that was split.
            char *result = replacement(item," ","_"); // Underscored_row_names.
            title_count++;
            result = strlwr(result); // lowercasing_row_names.
            result = replacement(result, opsys,""); // Handling the EOL.
            strcpy(titles[title_count], result);
            item = strtok(NULL, delim); // Navigating within the line.
            free(result); // Freeing the allocated data.
        }
        break; // Terminating the loop after reading the first line.
    }

    fprintf(xml_file,"<%s>\n", str_name); // Main title of the xml file.
    title_count = 1;
    while (fgets(str, 1000, fp) != NULL) { // Main loop.
        // Replacing the null character between two delimeters with an asterix(*) for easier line handling.
        char *result = replacement(str,old,new);
        // Replacing the EOL with an asterix(*) for easier line handling once again.
        result = replacement(result, opsys,"*");
        // Item stores the split line.
        char *item;
        item = strtok(result, delim);
        fprintf(xml_file, "\t<row id=\"%d%s",row,"\">\n");

        while(item != NULL) { // Navigating through the items and assigning them to the proper titles.

            if(strcmp(item,"*") == 0){
                fprintf(xml_file, "\t\t<%s/>\n", titles[title_count]);
            }
            else
                fprintf(xml_file, "\t\t<%s>%s</%s>\n", titles[title_count], item, titles[title_count]);
            item = strtok(NULL, delim);
            title_count++;

        }
        title_count = 1;

        fprintf(xml_file,"\t</row>\n");
        free(result);
        row++;
    }
    fprintf(xml_file,"</%s>", str_name);
    fclose(fp);
    fclose(xml_file);
    return 0;
}