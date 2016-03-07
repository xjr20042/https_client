#include <stdio.h>
#include "https.h"

int main(int argc, char *argv[])
{
    char *url;
    char data[1024], response[4096];
    int  ret;


    http_init(0);
    http_init(1);

    // Test a http get method
    url = "http://httpbin.org/get?message=https_client";

    ret = http_get(0, url, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a http post method

    url = "http://httpbin.org/post";
    sprintf(data, "{\"message\":\"Hello, https_client!\"}");

    ret = http_post(0, url, data, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a https get method

    url = "https://httpbin.org/get?message=https_client";

    ret = http_get(1, url, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a https post method

    url = "https://httpbin.org/post";
    sprintf(data, "{\"message\":\"Hello, https_client!\"}");

    ret = http_post(1, url, data, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    http_close(0);
    http_close(1);

    return 0;
}
