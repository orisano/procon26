#coding:utf-8

import sys

def main(filepath):
    print(len(filter(lambda x: x.strip(), open(filepath).readlines())))

if __name__ == "__main__":
    main(sys.argv[1])
