#!/usr/bin/python
#-*- coding:utf-8 -*-

def trans( a ) :
    if a == "司" :
        return 0   
    if a == "军" :
        return 1   
    if a == "师" :
        return 2   
    if a == "旅" :
        return 3   
    if a == "团" :
        return 4   
    if a == "营" :
        return 5   
    if a == "连" :
        return 6   
    if a == "排" :
        return 7   
    if a == "兵" :
        return 8   
    if a == "雷" :
        return 9   
    if a == "炸" :
        return 10  
    if a == "旗" :
        return 11   
    return -1

def main() :
    T = int(raw_input())
    print "[%d][25] = { " % T
    for t in range(T) :
        aa = []
        for i in range(6) :
            line = raw_input()
            aa.append( map(trans,line.split()) )
        print "{",
        for i in range(5,-1,-1) :
            for j in aa[i] :
                print j, ",",
        print "},"
        raw_input()
    print "}"

main()
