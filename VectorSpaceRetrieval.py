#!/usr/bin/python -tt

import json
import math
import operator
import numpy as np
import unicodedata
import collections
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
    usernames.append(mydict['name'])
  usernames = list(set(usernames))
  return usernames

def get_textdata(data):
  tweettext = []
  for item in data:
    tweettext.append(item['text'])
  return tweettext

def log2(x):
  return math.log10(x)/math.log10(2)

def tfidf(tweetdata):
  TotalNumOfTweets = len(tweetdata)
  newtext = []
  d = {}
  for item in tweetdata:
    l = re.split(r'[\(\)\{\}\@\$\#\%\^\&\+\=\<\>\!\?\[\]\-\,\.\\\'\"\:\;\|\r\b\t\n\s+\W+]', item['text'])
    for word in l:
      if word != '':
        d.setdefault(word.lower(), []).append(item['id'])
  tfidf = {}
  for k, v in d.iteritems():
    counter = collections.Counter(v)
    mydict = {}
    weight = 0.0
    for tweetid in counter:
      weight = (1 + log2(counter[tweetid])) * log2(TotalNumOfTweets/len(counter))
      mydict.setdefault(tweetid, weight)
    tfidf.setdefault(k, mydict)
  return tfidf

def CosineSimilarity(query, tf_idf, tweetdata):
  result = {}

  termsinquery = re.split(r'[\(\)\{\}\@\$\#\%\^\&\+\=\<\>\!\?\[\]\-\,\.\\\'\"\:\;\|\r\b\t\n\s+\W+]', query.lower()) 
  terms = collections.Counter(termsinquery)
  del terms['']

  i = 0
  for term in termsinquery:
    if term in tf_idf:
      i += 1
      continue
    else:
      break

  if i != len(termsinquery):
    print "Sorry, No results!"
    return {}

  mydict = {}
  for term in terms:
    mydict.setdefault(term, []).append((1.0 + log2(terms[term])) * log2(len(tweetdata)/len(tf_idf[term])))

  mod_q = 0.0
  for term in mydict:
    mod_q += math.pow(mydict[term][0], 2)
    mod_q = math.sqrt(mod_q)
  
  for item in tweetdata:
    tweetid = item['id']
    temp = 0.0
    mod_tweetid = 0.0
    termsintweet = re.split(r'[\(\)\{\}\@\$\#\%\^\&\+\=\<\>\!\?\[\]\-\,\.\\\'\"\:\;\|\r\b\t\n\s+\W+]', item['text'].lower())
    termsintweet[:] = (value for value in termsintweet if value != '')
    if set(termsintweet) & set(termsinquery) != set():
      for term in mydict:
        if item['id'] in tf_idf[term.lower()]:
          temp += mydict[term][0] * tf_idf[term.lower()][item['id']]
      for t in termsintweet:
        if item['id'] in tf_idf[t]:
          mod_tweetid += math.pow(tf_idf[t][item['id']], 2)

      mod_tweetid = math.sqrt(mod_tweetid)
      if mod_q == 0 or mod_tweetid == 0:
        result.setdefault(tweetid, []).append(0.0)
      else:
        temp = temp/(mod_q * mod_tweetid)
        result.setdefault(tweetid, []).append(temp)
  
  return result


def main():
  data = []
  data = process_tweets()

  tweettext = []
  tweettext = get_textdata(data)

  adict = {}
  bdict = {}
  for d in data:
    adict.setdefault(d['id'], d['user']['screen_name'])
    bdict.setdefault(d['id'], d['text'])

  tf_idf = tfidf(data)

  print 'Enter a query:'
  query = raw_input()
  while query != "quit":
    result = CosineSimilarity(query, tf_idf, data)
    sorted_result = sorted(result.iteritems(), key=operator.itemgetter(1))
    i = 0
    for item in reversed(sorted_result):
      if i > 50:
        break
      print item[0], bdict[item[0]]
      i += 1
    print 'Enter a query:'
    query = raw_input()
  

if __name__ == '__main__':
  main()
