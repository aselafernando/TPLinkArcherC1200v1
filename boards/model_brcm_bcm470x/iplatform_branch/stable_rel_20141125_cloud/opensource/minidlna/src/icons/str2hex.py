#!/usr/bin/python

__author__ = 'zhuxianfeng@tp-link.net'

import sys

def str2hex(path):
    hexarr = ''
    lines = open(path).read()
    line = ''
    i = 0

    hexarr += "/* " + path + "*/\n"
    hexarr += "unsigned char[] = \n"
    for c in lines:
        if i == 0:
            hexarr += '    "'
        elif i % 24 == 0:
            hexarr += '"\n    "'
        hexarr += '\\x{:02x}'.format(ord(c)) 
        i += 1
    if (i - 1) % 24 < 23:
        hexarr += '"'
    hexarr += ';\n'

    return hexarr

def main():
    if len(sys.argv) < 2:
        return
    for path in sys.argv[1:]:
        print(str2hex(path))

if __name__ == '__main__':
    main()

