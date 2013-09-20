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

def get_textdata(data):
  tweettext = []
  for item in data:
    tweettext.append(item['text'])
  return tweettext

def log2(x):
  return math.log10(x)/math.log10(2)

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

def tfidf(tweetdata):
  TotalNumOfTweets = len(tweetdata)
  newtext = []
  d = {}
  #i = 0
  for item in tweetdata:
  #for item in tweettext:
    l = re.split(r'[\(\)\{\}\@\$\#\%\^\&\+\=\<\>\!\?\[\]\-\,\.\\\'\"\:\;\|\r\b\t\n\s+\W+]', item['text'])
    for word in l:
      if word != '':
        #d.setdefault(word.lower(), []).append(i)
        d.setdefault(word.lower(), []).append(item['id'])
    #i += 1
  tfidf = {}
  for k, v in d.iteritems():
    counter = collections.Counter(v)
    mydict = {}
    weight = 0.0
    for tweetid in counter:
      weight = (1 + log2(counter[tweetid])) * log2(TotalNumOfTweets/len(counter))
      #mydict.setdefault(tweetid, []).append(weight)
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

def compute_bestorder(mydict, pagerank, adict, bdict, cdict):
  tempdict = {}
  result = {}
  for values in pagerank:
    tempdict.setdefault(values[0], values[1])

  i = 0  
  for item in mydict:
    val = 0.0
    if adict[item] in tempdict and i < 50:
      i += 1
      val = mydict[item][0] * 2.0 + tempdict[adict[item]]
      result.setdefault(adict[item], []).append(val)
      result.setdefault(adict[item], []).append(bdict[item])
  
  sorted_result = sorted(result.iteritems(), key=operator.itemgetter(1))
  return reversed(sorted_result)

def main():
  data = []
  data = process_tweets()

  tweettext = []
  tweettext = get_textdata(data)

  tf_idf = tfidf(data)
  graph = construct_graph(data)

  adict = {}
  bdict = {}
  cdict = {}
  for d in data:
    adict.setdefault(d['id'], d['user']['screen_name'])
    bdict.setdefault(d['id'], d['text'])
    cdict.setdefault(d['user']['screen_name'], d['id'])
  
  N = len(data)
  pagerank = ()
  pagerank = compute_pagerank(graph, N)

  print 'Enter a query:'
  query = raw_input()
  while query != "quit":
    myresult = {}
    myresult = CosineSimilarity(query, tf_idf, data)
    sorted_result = sorted(myresult.iteritems(), key=operator.itemgetter(1))
    mydict = {}
    for item in reversed(sorted_result):
      mydict.setdefault(item[0], item[1])
   
    pagerank = ()
    pagerank = compute_pagerank(graph, N)
    result = {}
    result = compute_bestorder(mydict, pagerank, adict, bdict, cdict)  
    for res in result:
      print res
    print 'Enter a query:'
    query = raw_input()


if __name__ == '__main__':
  main()

