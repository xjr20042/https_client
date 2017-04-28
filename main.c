#include <stdio.h>
#include <string.h>
#include "https.h"

#define TRUE    1
#define FALSE   0


int main(int argc, char *argv[])
{
    char *url;
    char data[1024], response[4096];
    int  i, ret, size;

    // Init http session. verify: check the server CA cert.
    http_init(0, TRUE);
    http_init(1, TRUE);

    // Test a http get method.
    url = "http://httpbin.org/get?message=https_client";

    ret = http_get(0, url, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a http post method.

    url = "http://httpbin.org/post";
    sprintf(data, "{\"message\":\"Hello, https_client!\"}");

    ret = http_post(0, url, data, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a https get method.

    url = "https://httpbin.org/get?message=https_client";

    ret = http_get(1, url, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a https post method.

    url = "https://httpbin.org/post";
    sprintf(data, "{\"message\":\"Hello, https_client!\"}");

    ret = http_post(1, url, data, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);

    // Test a https post with the chunked-encoding data.

    url = "https://httpbin.org/post";

    if(http_open_chunked(1, url) == 0)
    {
        size = sprintf(data, "[{\"message\":\"Hello, https_client %d\"},", 0);

        if(http_write_chunked(1, data, size) != size)
        {
            http_strerror(data, 1024);
            printf("socket error: %s \n", data);

            goto error;
        }

        for(i=1; i<4; i++)
        {
            size = sprintf(data, "{\"message\":\"Hello, https_client %d\"},", i);
            if(http_write_chunked(1, data, size) != size)
            {
                http_strerror(data, 1024);
                printf("socket error: %s \n", data);

                goto error;
            }
        }

        size = sprintf(data, "{\"message\":\"Hello, https_client %d\"}]", i);
        if(http_write_chunked(1, data, strlen(data)) != size)
        {
            http_strerror(data, 1024);
            printf("socket error: %s \n", data);

            goto error;
        }

        ret = http_read_chunked(1, response, sizeof(response));

        printf("return code: %d \n", ret);
        printf("return body: %s \n", response);

    }
    else
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);
    }

    error:

    http_close(0);
    http_close(1);

    return 0;
}
