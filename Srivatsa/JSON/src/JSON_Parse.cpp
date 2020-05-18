#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string>

int main(int argc, char **argv)
{  
    //const char* strjson = "{\"Shape\": \"Rectangle\",\"Color\": \"Yellow\",\"Text\": \"Kempten 5KM AHEAD\",\"gt\": [{\"gt_lat\": 37.7668,\"gt_long\": -1.223959e+2,\"gt_bearing\": 433.656}]}";

    const char* strjson = "{\"Shape\": \"Rectangle\",\"Color\": \"Yellow\",\"Text\": \"Kempten 5KM AHEAD\",\"gt\": [{\"gt_lat\": 37.7668,\"gt_long\": -1.223959e+2,\"gt_bearing\":433.656}],\"image\": [{\"img_lat\":425.14253,\"img_long\": 42.1235,\"img_bearing\":475.0102}]}";
    printf("String taken successfully\n");
    cJSON* src = cJSON_Parse(strjson);
    // Fetch the values from the string
    char* Shape = cJSON_GetObjectItemCaseSensitive(src, "Shape")->valuestring;
    char* Color = cJSON_GetObjectItemCaseSensitive(src, "Color")->valuestring;
    char* Text = cJSON_GetObjectItemCaseSensitive(src, "Text")->valuestring;
   
    printf("Shape = %s\n", Shape);
    printf("Color = %s\n", Color);
    printf("Text = %s\n", Text);
    // Now let's iterate through the moons array
    cJSON* gt = cJSON_GetObjectItemCaseSensitive(src, "gt");
    cJSON* img = cJSON_GetObjectItemCaseSensitive(src, "image");
    // Get the count
    int gt_count = cJSON_GetArraySize(gt);
    int i;
    for (i = 0; i < gt_count; i++) {
        printf("Loop entered. GT Data fetched\n");
        // Get the JSON element and then get the values as before
        cJSON* ground_truth = cJSON_GetArrayItem(gt, i);
        double gt_lat  = cJSON_GetObjectItemCaseSensitive(ground_truth, "gt_lat")->valuedouble;
        double gt_long = cJSON_GetObjectItemCaseSensitive(ground_truth, "gt_long")->valuedouble;
        double gt_bearing = cJSON_GetObjectItemCaseSensitive(ground_truth, "gt_bearing")->valuedouble;
        printf("gt_latitude = %d, \n", gt_lat);
        printf("gt_longitude = %d, \n", gt_long);
        printf("gt_bearing = %d, \n", gt_bearing);
    }
    int img_count = cJSON_GetArraySize(img);
    int j;
    for (j = 0; j < img_count; j++) {
        printf("Loop entered. Image Data Fetched\n");
        // Get the JSON element and then get the values as before
        cJSON* image_data = cJSON_GetArrayItem(img, j);
        double img_lat = cJSON_GetObjectItemCaseSensitive(image_data, "img_lat")->valuedouble;
        double img_long = cJSON_GetObjectItemCaseSensitive(image_data, "img_long")->valuedouble;
        double img_bearing = cJSON_GetObjectItemCaseSensitive(image_data, "img_bearing")->valuedouble;
        printf("image_latitude = %d, \n", img_lat);
        printf("image_longitude = %d, \n", img_long);
        printf("image_bearing = %d, \n", img_bearing);
    }
    // Finally remember to free the memory!
    cJSON_Delete(src);
    return 0;
}
