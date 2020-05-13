#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string>

//Function for LKAS
void LKAS()
{
    FILE* fp;
    char buffer[1024]; //To store the contents of JSON File
    fp = fopen("E:/Kempten/LivingLab/cJSON/anayat_LKAS.json", "r"); //JSON File which contains the string
    fread(buffer, 1024, 1, fp); // Read JSON File. Transferring contents to buffer
    fclose(fp);
    printf("String taken successfully\n");
    cJSON* src = cJSON_Parse(buffer);
    cJSON* Features = cJSON_GetObjectItemCaseSensitive(src, "Features");
    cJSON* ROI = cJSON_GetObjectItemCaseSensitive(src, "roi");
    int feature_count = cJSON_GetArraySize(Features);
    int i;
    for (i = 0; i < feature_count; i++)
    {
        printf("Loop entered. Features Array data fetched\n");
        // Get the JSON element and then get the values as before
        cJSON* features = cJSON_GetArrayItem(Features, i);
        char* Feature_activated = cJSON_GetObjectItemCaseSensitive(features, "Feature_activated")->valuestring;
        char* ID = cJSON_GetObjectItemCaseSensitive(features, "ID")->valuestring;
        char* name = cJSON_GetObjectItemCaseSensitive(features, "name")->valuestring;
        char* color = cJSON_GetObjectItemCaseSensitive(features, "color")->valuestring;
        char* Output_1 = cJSON_GetObjectItemCaseSensitive(features, "Output_1")->valuestring;
        char* Output_2 = cJSON_GetObjectItemCaseSensitive(features, "Output_2")->valuestring;
        char* Output_3 = cJSON_GetObjectItemCaseSensitive(features, "Output_3")->valuestring;
        char* Output_4 = cJSON_GetObjectItemCaseSensitive(features, "Output_4")->valuestring;

        printf("Feature_activated = %s, \n", Feature_activated);
        printf("ID = %s, \n", ID);
        printf("name = %s, \n", name);
        printf("color = %s, \n", color);
        printf("Output_1 = %s, \n", Output_1);
        printf("Output_2 = %s, \n", Output_2);
        printf("Output_3 = %s, \n", Output_3);
        printf("Output_4 = %s, \n", Output_4);
    }
    int ROI_count = cJSON_GetArraySize(ROI);
    int j;
    for (j = 0; j < ROI_count; j++)
    {
        printf("Loop entered. ROI Data array Fetched\n");
        // Get the JSON element and then get the values as before
        cJSON* ROI_data = cJSON_GetArrayItem(ROI, j);
        int X = cJSON_GetObjectItemCaseSensitive(ROI_data, "x")->valueint;
        int Y = cJSON_GetObjectItemCaseSensitive(ROI_data, "y")->valueint;
        int Height = cJSON_GetObjectItemCaseSensitive(ROI_data, "Height")->valueint;
        int Width = cJSON_GetObjectItemCaseSensitive(ROI_data, "Width")->valueint;

        printf("X-Coordinate = %i, \n", X);
        printf("Y-Coordinate = %i, \n", Y);
        printf("Height of the image captured = %i, \n", Height);
        printf("Width of the image captured = %i, \n", Width);
    }
}
// Function for ACC
void ACC()
{
    FILE* fp;
    char buffer[1024]; //To store the contents of JSON File
    fp = fopen("E:/Kempten/LivingLab/cJSON/anayat_ACC.json", "r"); //JSON File which contains the string
    fread(buffer, 1024, 1, fp); // Read JSON File. Transferring contents to buffer
    fclose(fp);
    printf("String taken successfully\n");
    cJSON* src = cJSON_Parse(buffer);
    cJSON* Features = cJSON_GetObjectItemCaseSensitive(src, "Features");
    cJSON* ROI = cJSON_GetObjectItemCaseSensitive(src, "roi");
    int feature_count = cJSON_GetArraySize(Features);
    int i;
    for (i = 0; i < feature_count; i++)
    {
        printf("Loop entered. Features Array data fetched\n");
        // Get the JSON element and then get the values as before
        cJSON* features = cJSON_GetArrayItem(Features, i);
        char* Feature_activated = cJSON_GetObjectItemCaseSensitive(features, "Feature_activated")->valuestring;
        char* ID = cJSON_GetObjectItemCaseSensitive(features, "ID")->valuestring;
        char* name = cJSON_GetObjectItemCaseSensitive(features, "name")->valuestring;
        char* color = cJSON_GetObjectItemCaseSensitive(features, "color")->valuestring;
        char* Output_1 = cJSON_GetObjectItemCaseSensitive(features, "Output_1")->valuestring;
        char* Output_2 = cJSON_GetObjectItemCaseSensitive(features, "Output_2")->valuestring;
        char* Output_3 = cJSON_GetObjectItemCaseSensitive(features, "Output_3")->valuestring;
        char* Output_4 = cJSON_GetObjectItemCaseSensitive(features, "Output_4")->valuestring;

        printf("Feature_activated = %s, \n", Feature_activated);
        printf("ID = %s, \n", ID);
        printf("name = %s, \n", name);
        printf("color = %s, \n", color);
        printf("Output_1 = %s, \n", Output_1);
        printf("Output_2 = %s, \n", Output_2);
        printf("Output_3 = %s, \n", Output_3);
        printf("Output_4 = %s, \n", Output_4);
    }
    int ROI_count = cJSON_GetArraySize(ROI);
    int j;
    for (j = 0; j < ROI_count; j++)
    {
        printf("Loop entered. ROI Data array Fetched\n");
        // Get the JSON element and then get the values as before
        cJSON* ROI_data = cJSON_GetArrayItem(ROI, j);
        int X = cJSON_GetObjectItemCaseSensitive(ROI_data, "x")->valueint;
        int Y = cJSON_GetObjectItemCaseSensitive(ROI_data, "y")->valueint;
        int Height = cJSON_GetObjectItemCaseSensitive(ROI_data, "Height")->valueint;
        int Width = cJSON_GetObjectItemCaseSensitive(ROI_data, "Width")->valueint;

        printf("X-Coordinate = %i, \n", X);
        printf("Y-Coordinate = %i, \n", Y);
        printf("Height of the image captured = %i, \n", Height);
        printf("Width of the image captured = %i, \n", Width);
    }
}

int main(int argc, char* argv[])
{
    if (argc > 4)
    {
        printf("Too many arguments\n");
        return -1;
    }
    int type;
    type = atoi(argv[1]);   
        if (type == 1)
        {
            printf("Function LKAS is called\n");
            LKAS();
            printf("Function LKAS called, result displayed");
        }
        else if (type == 2)
        {
            printf("Function ACC is called\n");
            ACC();
            printf("Function ACC called, results displayed\n");
        }
        else
        {
            printf("Incorrect arguments\n Please Check Again");
        }
    return 0;   
}
