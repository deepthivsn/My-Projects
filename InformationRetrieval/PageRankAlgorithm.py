#!/usr/bin/python -tt

import json
import math
import operator
import numpy as np
import unicodedata
import collections
from collections import defaultdict
import re, cPickle
import gzip
import shlex
from os import listdir
from os.path import isfile, join

def _decode_list(data):
    rv = []
    for item in data:
        if isinstance(item, unicode):
            item = item.encode('utf-8')
        elif isinstance(item, list):
            item = _decode_list(item)
        elif isinstance(item, dict):
            item = _decode_dict(item)
        rv.append(item)
    return rv

def _decode_dict(data):
    rv = {}
    for key, value in data.iteritems():
        if isinstance(key, unicode):
           key = key.encode('utf-8')
        if isinstance(value, unicode):
           value = value.encode('utf-8')
        elif isinstance(value, list):
           value = _decode_list(value)
        elif isinstance(value, dict):
           value = _decode_dict(value)
        rv[key] = value
    return rv

def process_tweets():
  data = []
  tempdata = []
  with open('/home/deepthi/deepthi/test/IR/hw2/mars_tweets_medium.json') as f:
    for line in f:
      tempdata = json.loads(line, object_hook=_decode_dict)
      data.append(tempdata)
  return data

def get_usernames(data):
  usernames = []
  for item in data:
    mydict = item["user"]
    usernames.append(mydict['name'].lower())
  usernames = list(set(usernames))
  return usernames

def get_screennames(data):
  screennames = []
  for item in data:
    mydict = item["user"]
    screennames.append(mydict['screen_name'])
  screennames = list(set(screennames))
  return screennames

def get_textdata(data):
  tweettext = {}
  for item in data:
    tweettext.setdefault(item['user']['screen_name'], (item['text']))
  return tweettext

def construct_graph(textdata):
  mydict = {}
  nousermentions = []
  for item in textdata:
    if len(item['entities']['user_mentions']) != 0:
      for tmp in item['entities']['user_mentions']:
        if tmp['screen_name'] != item['user']['screen_name']:
          mydict.setdefault(tmp['screen_name'], []).append(item['user']['screen_name'])
  for m in mydict:
    mydict[m] = list(set(mydict[m]))
  newdict = {}
  for i in mydict:
    for g in mydict[i]:
      if g not in mydict:
        newdict.setdefault(g, [])
        #newdict[g] = []
  result = dict(mydict.items() + newdict.items())
  return result

def compute_pagerank(graph, N):
  pagerank1 = defaultdict(float)
  pagerank2 = defaultdict(float)
  mydict = {}
  for item in graph:
    if graph[item] != []:
      for tmpitem in graph[item]:
        if tmpitem != item:
          mydict.setdefault(tmpitem, []).append(item)

  for item in graph:
    pagerank1.setdefault(item, 1.0/N)
  n = 0
  while n < 30:
    n += 1
    for key in pagerank1:
      pagerank2[key] = pagerank1[key]
    for k in graph:
     if len(graph[k]) != 0:
       val = 0.0
       for i in graph[k]:
         val += (pagerank1[i])/len(mydict[i])
       pagerank2[k] = (1 - 0.9)/N + 0.9 * val
     else:
       #pagerank2[k] += (1 - 0.1)/N 
       pagerank2[k] = 1.0/N
    flag = 0
    for page in pagerank1:
      if (abs(pagerank1[page] - pagerank2[page])) <= 0.00005:
        flag = 1
        continue
      else:
        flag = 0
        break
    if flag:
      break
    else:
      pagerank1 = pagerank2
      pagerank2 = {}
  sorted_result = sorted(pagerank1.iteritems(), key=operator.itemgetter(1))
  return reversed(sorted_result)

def main():
  tweetdata = []
  tweetdata = process_tweets()
 
  graph = construct_graph(tweetdata)

  N = len(tweetdata)
  pagerank = {}
  pagerank = compute_pagerank(graph, N)

  i = 0
  for item in pagerank:
    print item
    i += 1
    if i > 50:
      break

if __name__ == '__main__':
  main()
