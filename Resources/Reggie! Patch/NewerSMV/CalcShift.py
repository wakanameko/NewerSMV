# -*- coding: utf-8 -*-

baseData = 0x01234567

def calcShift(a):
    distance = (baseData & 0xF) << 4
    print(distance)

target = input("type int values here.:")
calcShift(target)