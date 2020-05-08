#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "test.h"

struct gt_coordinates
{
    double gt_lat;
    double gt_lon;
    double gt_bearing;
};
struct img_coordinates
{
    double img_lat;
    double img_lon;
    double img_bearing;
};
struct dashboard
{
    int ROI_X;
    int ROI_Y;
    int frame_height;
    int frame_width;
    const char* color;
    const char* output_red;
    const char* output_green;
};
struct Object_Parameter
{
    const char* shape;
    const char* color;
    const char* text;
    struct gt_coordinates gt;
    struct img_coordinates img;
    struct dashboard db;
};

static int print_preallocated(cJSON* root)
{
    /* declarations */
    char* out = NULL;
    char* buf = NULL;
    char* buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("cJSON_Print result:\n%s\n", out);
            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    /* success */
    printf("%s\n", buf);

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("cJSON_Print result:\n%s\n", out);
        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}

/* Create objects */
static void create_object(void)
{
    cJSON* root = NULL;
    cJSON* field = NULL;
    int i = 0;
  
    struct Object_Parameter fields[3] =
    {
        {
            "Rectangle",
            "Yellow",
            "Kempten",
            37.7668,
            -1.223959e+2,
            433.656,
            7425.14253,
            42.1235,
            475.0102,
            500,
            500,
            1280,
            720,
            "RED-COLOR",
            "RED_DETECTED",
            "GREEN_NOT_DETECTED"
        },
        {
           "Circle",
            "Red",
            "120",
            37.7668,
            -1.223959e+2,
            433.656,
            7425.14253,
            42.1235,
            475.0102,
            500,
            500,
            1280,
            720,
            "GREEN-COLOR",
            "RED_NOT_DETECTED",
            "GREEN_DETECTED"
        },
        {
            "Triangle",
            "Red",
            "Go Fast",
            37.7668,
            -1.223959e+2,
            433.656,
            7425.14253,
            42.1235,
            475.0102,
            0,
            0,
            0,
            0,
            "NO_COLOR",
            "RED_NOT_DETECTED",
            "GREEN_NOT_DETECTED"
        },
    };
    /* Creating array */
    root = cJSON_CreateArray();
    for (i = 0; i < 3; i++)
    {
        cJSON_AddItemToArray(root, field = cJSON_CreateObject());
        cJSON_AddStringToObject(field, "Shape", fields[i].shape);
        cJSON_AddStringToObject(field, "Color", fields[i].color);
        cJSON_AddStringToObject(field, "Text", fields[i].text);
        cJSON_AddNumberToObject(field, "gt_lat", fields[i].gt.gt_lat);
        cJSON_AddNumberToObject(field, "gt_long", fields[i].gt.gt_lon);
        cJSON_AddNumberToObject(field, "gt_bearing", fields[i].gt.gt_bearing);
        cJSON_AddNumberToObject(field, "img_lat", fields[i].img.img_lat);
        cJSON_AddNumberToObject(field, "img_long", fields[i].img.img_lon);
        cJSON_AddNumberToObject(field, "img_bearing", fields[i].img.img_bearing);
        cJSON_AddNumberToObject(field, "ROI_X-COORDINATE", fields[i].db.ROI_X);
        cJSON_AddNumberToObject(field, "ROI_Y-COORDINATE", fields[i].db.ROI_Y);
        cJSON_AddNumberToObject(field, "Frame_Height", fields[i].db.frame_height);
        cJSON_AddNumberToObject(field, "Frame_Width", fields[i].db.frame_width);
        cJSON_AddStringToObject(field, "Color", fields[i].db.color);
        cJSON_AddStringToObject(field, "Output-1", fields[i].db.output_red);
        cJSON_AddStringToObject(field, "Output-2", fields[i].db.output_green);
    }
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
}

int main(void)
{
    /* print the version */
    printf("cJSON Version for C: %s\n", cJSON_Version());
    // Call function for JSON_object creation
    create_object();
    return 0;
}

int supports_full_hd(const char* const monitor)
{
    const cJSON* resolution = NULL;
    const cJSON* resolutions = NULL;
    const cJSON* name = NULL;
    int status = 0;
    cJSON* monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL)
    {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Checking monitor \"%s\"\n", name->valuestring);
    }

    resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    cJSON_ArrayForEach(resolution, resolutions)
    {
        cJSON* width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON* height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))
        {
            status = 0;
            goto end;
        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080))
        {
            status = 1;
            goto end;
        }
    }

end:
    cJSON_Delete(monitor_json);
    return status;
}
