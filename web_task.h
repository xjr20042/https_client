#ifndef WEB_ASYNC_TASK_HHHHHH_
#define WEB_ASYNC_TASK_HHHHHH_

#include <string>
#include <map>
#include <vector>

typedef struct _web_buffer_t {
    char* buf;
    //buffer orignal size
    int   size;
    //length of data that wrote to buffer
    int   datalen;
} web_buffer_t;

typedef std::map<std::string, std::string> PostStringList ;
struct PostFileInfo {
    std::string filePath;
    std::string fileName;
    std::string fileType;
};
typedef std::vector<PostFileInfo> PostFileList;

class WebTask
{
 public:
 	WebTask();
	~WebTask();

	void SetUrl(const char* url);

	void AddPostString(const char* item_name,  const char* item_data);

	void AddPostString(const char* item_name,  const char* item_data, bool need_signed);
	
	void AddPostPicture(const char* item_name, const char* file_path, const char* file_name = NULL);

	void AddPostFile(const char* item_name, const char* file_path, const char* file_name = NULL, const char* content_type = "application/octet-stream");
	void AddPostFileChunked(const char* item_name, const char* file_path, const char* file_name = NULL, const char* content_type = "application/octet-stream");
	
	// must call DoGetString() or DoGetFile() before start task
	int  DoGetString();
	// must call DoGetString() or DoGetFile() before start task
	// range like "0-255"
	int  DoGetFile(const char* range=NULL);
	
	int  WaitTaskDone();

	const char*  GetResultString();
	const char*  GetFilePath();

    /**
     * @timeo seconds
     */
    void SetConnectTimeout(int timeo);

 private:
	std::string url_;
    void* https_info_;
    int   is_get_;
    int   is_getfile_;
	web_buffer_t buf_;
    PostStringList post_items_;
    PostFileList post_files_;
 private:
 	WebTask(const WebTask&);
	WebTask& operator=(const WebTask&);
};
#endif

