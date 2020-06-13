u«Zµìmþ™ZŠvÚ±î¸#!/usr/bin/python3

import sys
import urllib.request

def limit(s, n):
    pos = len(s)
    if (pos > n):
        pos = n
        while (not s[pos].isspace()): pos -= 1
        while (s[pos-1].isspace()): pos -= 1
    return pos

def right_justify(s, n):
    pos = limit(s, n)
    tmp_s = s[0:pos]
    if (pos <= len(s)): s = s[pos:].strip()
    print (tmp_s.rjust(n))
    if (len(s) > 0): right_justify(s, n)

url = 'http://opensource.petra.ac.id/~hnpalit/wiki-python.txt'
out = '/tmp/std.out'
orig_stdout = sys.stdout
fout = open(out, 'w')
sys.stdout = fout
txt = urllib.request.urlopen(url).read()
for s_line in txt.splitlines():
    s_line = s_line.strip().decode('utf-8')
    right_justify(s_line, 70)
sys.stdout = orig_stdout
fout.close()
