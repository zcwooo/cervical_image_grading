"""
File:dictest.py
Date: 2020/10/21 10:45
Auther:Affreden
"""
empydic={}
name="ais"
for i in range(10):
    try:
        empydic[name] += 1
    except:
        empydic[name]=1
dic2={"aec":2}
empydic.update(dic2)
print(empydic)