#!/usr/bin/python -tt

import re, cPickle
import gzip
import shlex
from os import listdir
from os.path import isfile, join

filename1 = "/home/deepthi/deepthi/test/IR/hw1/data1.pkl"
filename2 = "/home/deepthi/deepthi/test/IR/hw1/data2.pkl"

def swap(c, i, j):
  c = list(c)
  c[i], c[j] = c[j], c[i]
  return ''.join(c)

def permuterm(word):
  result = []
  k=0
  word += '$'
  length = len(word)
  while k < length:
    i = 1
    while i < length:
      word = swap(word, (length-i), (length-i-1))
      i += 1
    result.append(word)
    k += 1
  result.pop()
  return result

def permuterm_wc(word):
  result = permuterm(word)
  for item in result:
    if item[len(item)-1] == '*':
      return item

def parser():

  mypath = "/home/deepthi/deepthi/test/IR/hw1/books/"
  onlyfiles = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]

  filelist = [] 

  for file in onlyfiles:
    k = file.split('.')
    filelist.append(k[0])

  d = {}
  i=0
  num = 0

  for file in onlyfiles:
    myfile = open(join(mypath,file), "r")
    lines = myfile.readlines()
    for line in lines:
      l = re.split(r'[\(\)\{\}\@\$\#\%\^\&\+\=\<\>\_\!\?\[\]\-\,\.\\\'\"\:\;\|\r\b\t\n\s ]', line)
      for word in l:
        d.setdefault(word.lower(), []).append(filelist[i])
    i += 1

  mydict = {}

  for k in d.keys():
    myset = set(d[k])
    for item in myset:
      mydict.setdefault(k,[]).append(item)

  file = gzip.GzipFile(filename1, 'wb')
  cPickle.dump(mydict, file)
  file.close()


def phrase_query_parser():

  mypath = "/home/deepthi/deepthi/test/IR/hw1/books/"
  onlyfiles = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]

  d = {}
  i=0

  for file in onlyfiles:
    myfile = open(join(mypath,file), "r")
    lines = myfile.readlines()
    text = myfile.read()
    temp_dict = {}
    wordlist = {}
    position = 0
    for line in lines:
      l = re.split(r'[\(\)\{\}\@\$\#\%\^\&\+\=\<\>\_\!\?\[\]\-\,\.\\\'\"\:\;\|\r\b\t\n\s ]', line)
      position_list = []
      for word in l:
        position += 1
        k = file.split('.')
        wordlist.setdefault(word.lower(), []).append(position)

    for word in wordlist:
      d.setdefault(word.lower(), []).append(k[0])
      if word in wordlist:
        d.setdefault(word.lower(), []).append(wordlist[word])

  file = gzip.GzipFile(filename2, 'wb')
  cPickle.dump(d, file)
  file.close()

def docs_containing_query_words(query):
  file = gzip.GzipFile(filename1, 'rb')
  d = cPickle.load(file)
  result = []
  query_list = query.split(" ")
  if query_list[0] in d:
    result = d[query_list[0]]
  else:
    print "sorry no matches"
    return result
  for item in query_list:
    result = list(set(d[item]) & set(result))
  return result

def docs_containing_phrase_queries(query):
  result = []
  tempResult = []
  file = gzip.GzipFile(filename2, 'rb')
  d = cPickle.load(file)
  query_files = docs_containing_query_words(query)
  query_list = re.split(r'[ ]', query)
  mydict = {}
  newdict = {}

  for file in query_files:
    mydict = {}
    newdict = {}
    i = 0
    for query_word in query_list:
      if query_word.lower() in d:
        mylist = d[query_word.lower()]
      else:
        print "Sorry, no matches found!"
        return []
      myindex = mylist.index(file) + 1
      for item in mylist[myindex]:
         item = item - i
         newdict.setdefault(query_word.lower(), []).append(item)
      i += 1

    tempResult = newdict[query_list[0].lower()]
    for key in newdict.keys():
      tempResult = list(set(newdict[key.lower()]) & set(tempResult))
    if len(tempResult) != 0:
      result.append(file)

  return result

def docs_containing_wild_cards(query):
  file = gzip.GzipFile(filename1, 'rb')
  d = cPickle.load(file)
  result = []
  query_list = query.split(" ")
  ddict = {}
  for item in query_list:
    templist = []
    if '*' in item:
      word = permuterm_wc(item)
      word = word[:-1]
      for key in d.keys():
        for k in permuterm(key):
          if ''.join(k.lower()).find(''.join(word)) != -1:
            templist.append(key)
      templist = list(set(templist))
      if templist[0] in d:
        temp = d[templist[0]]
      else:
        print "Sorry, no matches found!"
        return []
      for i in templist:
        temp = list(set(d[i]) | set(temp))
      ddict[item] = temp

    else:
      ddict[item] = d[item]

  mypath = "/home/deepthi/deepthi/test/IR/hw1/books/"
  onlyfiles = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]

  filelist = []

  for file in onlyfiles:
    k = file.split('.')
    filelist.append(k[0])

  result = filelist
  for key in ddict.keys():
    result = list(set(result) & set(ddict[key]))

  return result

def process_query(query):

  phrasequerylist = []
  booleanquerylist = []
  wildcardquerylist = []

  query = query.lower()

  query_list = shlex.split(query) 
  for item in query_list:
    if ' ' in item:
      phrasequerylist.append(item)
    if '*' in item:
      wildcardquerylist.append(item)
    else:
      booleanquerylist.append(item)

  mypath = "/home/deepthi/deepthi/test/IR/hw1/books/"
  onlyfiles = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]

  filelist = []

  for file in onlyfiles:
    k = file.split('.')
    filelist.append(k[0])

  result1 = filelist
  result2 = filelist
  result3 = filelist
  result = []

  for word1 in phrasequerylist:
    result1 = list(set(docs_containing_phrase_queries(word1)) & set(result1))

  for word2 in wildcardquerylist:
    result2 = list(set(docs_containing_wild_cards(word2)) & set(result2))

  for word3 in booleanquerylist:  
    result3 = list(set(docs_containing_query_words(word3)) & set(result3))

  result = list(set(result1) & set(result2) & set(result3))

  if len(result) == 0:
    print "Sorry, no matches found!"

  files = []
  for word in result:
    files.append(word + ".txt")
  for file in files:
    print file,

def main():
  parser() 
  file1 = gzip.GzipFile(filename1, 'rb')
  d1 = cPickle.load(file1)
  file1 = open('index1', 'w')
  for k in d1.keys():
    print >> file1, k + ":" ,
    for item in d1[k]:
      print >> file1, item,
    print >> file1, " "

  phrase_query_parser()
  file2 = gzip.GzipFile(filename2, 'rb')
  d2 = cPickle.load(file2)
  file2 = open('index2', 'w')
  for k in d2.keys():
    print >> file2, k + ":" ,
    for item in d2[k]:
      print >> file2,  item,
    print >> file2, " "

  print 'Enter a query:'
  query = raw_input()
  
  while query != "quit":
    process_query(query)
    print 'Enter a query:'
    query = raw_input()


if __name__ == '__main__':
  main()
