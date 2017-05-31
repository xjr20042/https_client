#include <stdio.h>
#include <string.h>
#include "https.h"


int main(int argc, char *argv[])
{
    char *url;
    char data[1024], response[4096];
    char err[100];

    int  i, ret, size;
    int  mode;

    HTTP_INFO hi1, hi2;


    // Init http session. verify: check the server CA cert.
    http_init(&hi1, FALSE);
    http_init(&hi2, TRUE);

//    url = "https://wavegw.clova.ai/rswave/tts/1.6.5_23803-dcbee999d8a7452dde1e69d38aaceb31-1494844985383?tid=14dee084f5905f9c8f628cbd052ec529&hash=29581ae85d777e16445dbeda175c4fa4c344d437c79e5991ec5832557be3a805";

//    url = argv[1];

//    ret = http_get(&hi1, url, response, 4096);

//    printf("len: %d, %s", ret, response);


/*
    url = "https://localhost:8080/upload";
    sprintf(data,
            "--1234567890abcdef\r\n"
            "Content-Disposition: form-data; name=\"upload\"; filename=\"test.txt\"\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "test message\r\n"
            "--1234567890abcdef--\r\n\r\n"
    );

    ret = http_post(&hi1, url, data, response, sizeof(response));

    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);
*/

    url = "https://localhost:8080/upload";

    if(http_open(&hi1, url) < 0)
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);

        goto error;
    }

    snprintf(hi1.request.method, 8, "POST");
    hi1.request.close = FALSE;
    hi1.request.chunked = TRUE;
    snprintf(hi1.request.content_type, 256, "multipart/form-data; boundary=1234567890abcdef");

    size = sprintf(data,
                   "--1234567890abcdef\r\n"
                           "Content-Disposition: form-data; name=\"upload\"; filename=\"test.txt\"\r\n"
                           "Content-Type: text/plain\r\n\r\n"
                           "test message\r\n"
                           "--1234567890abcdef--\r\n"
    );

    hi1.request.content_length = size;


    if(http_write_header(&hi1) < 0)
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);

        goto error;
    }

    if(http_write(&hi1, data, size) != size)
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);

        goto error;
    }

    // Write end-chunked
    if(http_write_end(&hi1) < 0)
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);

        goto error;
    }

    http_read_init(&hi1);

    while(1)
    {
        mode = http_parse(&hi1);

        if((mode & HTTP_PARSE_WRITE) == HTTP_PARSE_WRITE)
        {
            printf("write: len: %ld, %s \n", hi1.body_len, hi1.body);
        }

        if((mode & HTTP_PARSE_READ) == HTTP_PARSE_READ)
        {
            ret = http_read(&hi1);
            if (ret < 0)
            {
                mbedtls_strerror(ret, err, 100);
                printf("socket error: %s(%d)", err, ret);

                break;
            }
            else if (ret == 0)
            {
                break;
            }
        }
        else if((mode & HTTP_PARSE_CHUNK) == HTTP_PARSE_CHUNK)
        {
            printf("return: HTTP_PARSE_CHUNK: chunk_size: %ld \n", hi1.chunk_size);
        }
        else if((mode & HTTP_PARSE_END) == HTTP_PARSE_END)
        {
            printf("return: HTTP_PARSE_END: content_length: %ld \n", hi1.response.content_length);
            break;
        }
        else if((mode & HTTP_PARSE_ERROR) == HTTP_PARSE_ERROR)
        {
            snprintf(response, 256, "http parse error.");
            break;
        }
    }

    if(hi1.response.close == TRUE)
    {
        http_close(&hi1);
    }

/*
    // Test a http get method.
    url = "http://httpbin.org/get?message=https_client";

    ret = http_get(&hi1, url, response, sizeof(response));

    printf("return code: %d \n", hi1.response.status);
    printf("return body: len: %d, %s \n", ret, response);

    // Test a http post method.

    url = "http://httpbin.org/post";
    sprintf(data, "{\"message\":\"Hello, https_client!\"}");

    ret = http_post(&hi1, url, data, response, sizeof(response));

    printf("return code: %d \n", hi1.response.status);
    printf("return body: len: %d, %s \n", ret, response);

    // Test a https get method.

    url = "https://httpbin.org/get?message=https_client";

    ret = http_get(&hi2, url, response, sizeof(response));

    printf("return code: %d \n", hi1.response.status);
    printf("return body: len: %d, %s \n", ret, response);

    // Test a https post method.

    url = "https://httpbin.org/post";
    sprintf(data, "{\"message\":\"Hello, https_client!\"}");

    ret = http_post(&hi2, url, data, response, sizeof(response));

    printf("return code: %d \n", hi1.response.status);
    printf("return body: len: %d, %s \n", ret, response);

    // Test a https post with the chunked-encoding data.

    url = "https://httpbin.org/post";

    if(http_open_chunked(&hi2, url) == 0)
    {
        size = sprintf(data, "[{\"message\":\"Hello, https_client %d\"},", 0);

        if(http_write_chunked(&hi2, data, size) != size)
        {
            http_strerror(data, 1024);
            printf("socket error: %s \n", data);

            goto error;
        }

        for(i=1; i<4; i++)
        {
            size = sprintf(data, "{\"message\":\"Hello, https_client %d\"},", i);
            if(http_write_chunked(&hi2, data, size) != size)
            {
                http_strerror(data, 1024);
                printf("socket error: %s \n", data);

                goto error;
            }
        }

        size = sprintf(data, "{\"message\":\"Hello, https_client %d\"}]", i);
        if(http_write_chunked(&hi2, data, strlen(data)) != size)
        {
            http_strerror(data, 1024);
            printf("socket error: %s \n", data);

            goto error;
        }

        ret = http_read_chunked(&hi2, response, sizeof(response));

        printf("return code: %d \n", ret);
        printf("return body: %s \n", response);

    }
    else
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);
    }

    error:
*/

error:

    http_close(&hi1);
    http_close(&hi2);

    return 0;
}
