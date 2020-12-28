#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>

#include "https.h"
#include "web_task.h"
#include "ca.c"

#define plog printf

static size_t web_write_mem(void *ptr, size_t size, size_t nmemb, void *data)
{
    //ignore data
    if (!data) return size * nmemb;

    web_buffer_t *buf = (web_buffer_t *)data;

    size_t left_size = buf->size - buf->datalen;
    size_t copy_size = size * nmemb > left_size ? left_size : size * nmemb;

    memcpy(buf->buf + buf->datalen, ptr, copy_size);
    buf->datalen += copy_size;
    return size * nmemb;
}

static size_t web_write_file(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

WebTask::WebTask()
{
	memset(&buf_, 0, sizeof(buf_));

	is_getfile_ = 0;

    https_info_ = calloc(1, sizeof(HTTP_INFO));
    http_init((HTTP_INFO *)https_info_, TRUE);

    is_get_ = 1;
}

WebTask::~WebTask()
{
	if(buf_.buf)
		free(buf_.buf);
    if(https_info_) {
        http_close((HTTP_INFO *)https_info_);
        free(https_info_);
    }
}

void WebTask::SetUrl(const char* url)
{
	plog("%s\n", url);
	url_ = url;
}
void WebTask::AddPostString(const char* item_name, const char* item_data)
{
	plog("%s:%s\n", item_name, item_data);
    is_get_ = 0;
    post_items_[item_name] = item_data;
}

void WebTask::AddPostString(const char* item_name,  const char* item_data, bool need_signed)
{
    //if(need_signed)
        //m_need_signed_kv[item_name] = item_data;
    AddPostString(item_name, item_data);
}
void WebTask::AddPostPicture(const char* item_name, const char* file_path, const char* file_name)
{
	AddPostFile(item_name, file_path, file_name, "image/jpeg");
}
void WebTask::AddPostFile(const char* item_name, const char* file_path, const char* file_name, const char* content_type )
{	
	plog("%s:%s\n", item_name, file_path);
	if( access(file_path, F_OK) != 0 )
		return;
    PostFileInfo file;
    file.filePath = file_path;
    file.fileName = file_name;
    file.fileType = content_type;
    post_files_.push_back(file);
}
void WebTask::AddPostFileChunked(const char* item_name, const char* file_path, const char* file_name , const char* content_type )
{
	plog("%s:%s\n", item_name, file_path);
	if( access(file_path, F_OK) != 0 )
		return;
}
int WebTask::DoGetString()
{
	buf_.size = 200*1024;
	buf_.buf = (char*)malloc(buf_.size);	
	buf_.datalen = 0;
	memset(buf_.buf, 0, buf_.size);
	
	return 0;
}
int WebTask::WaitTaskDone()
{
    if (is_get_) {
        int ret = http_get((HTTP_INFO*)https_info_, (char*)url_.c_str(), buf_.buf, buf_.size);

        printf("return code: %d \n", ret);
        printf("return body: %s \n", buf_.buf);
    } else {
        std::string form_data;
        PostStringList::const_iterator it;
        const char* boundary = "--WebTaskBundaryCish10fha8df03xafh0ahdyf2";
        for (it = post_items_.begin(); it != post_items_.end(); it++) {
            form_data += "--";
            form_data += boundary;
            form_data += "\r\n";
            form_data += "Content-Disposition: form-data; name=\"";
            form_data += it->first;
            form_data += "\"\r\n\r\n";
            form_data += it->second;
            form_data += "\r\n";
        }
        for (int i = 0; i < post_files_.size(); i++) {
            /*
            "--1234567890abcdef\r\n"
            "Content-Disposition: form-data; name=\"upload\"; filename=\"test.txt\"\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "test message\r\n"
            "--1234567890abcdef--\r\n"
            */
            PostFileInfo & finfo = post_files_[i];
            form_data += "--";
            form_data += boundary;
            form_data += "\r\n";
            form_data += "Content-Disposition: form-data; name=\"upload\"; filename=\"";
            form_data += finfo.fileName;
            form_data += "\"\r\nContent-Type: ";
            form_data += finfo.fileType;
            form_data += "\r\n\r\n";
            //file body
            FILE *file = fopen(finfo.filePath.c_str(), "rb");
            while(file && !feof(file)) {
                char buf[BUFSIZ];
                int ret = fread(buf, 1, sizeof(buf), file);
                if (ret > 0) {
                    form_data.append(buf, ret);
                } else {
                    break;
                }
            }
            if (file) fclose(file);
            form_data +="\r\n";
        }
        form_data += "--";
        form_data += boundary;
        form_data += "--\r\n";
        int  ret = http_post_formdata((HTTP_INFO*)https_info_, (char*)url_.c_str(), 
                (char*)boundary, (char*)form_data.c_str(), form_data.size(), buf_.buf, buf_.size);
        printf("return code: %d \n", ret);
        printf("return body: %s \n", buf_.buf);
    }
    return 0;
}
int WebTask::DoGetFile(const char* range) {

  is_getfile_ = 1;

  return 0;
}

const char*  WebTask::GetResultString()
{
	if(is_getfile_ == 0 && buf_.buf)
		return buf_.buf;
	else
		return NULL;
}

const char*  WebTask::GetFilePath()
{
}

void WebTask::SetConnectTimeout(int timeo)
{
    plog("[%d]\n", timeo);
}

