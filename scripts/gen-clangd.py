#!/usr/bin/env python3
import os

def getTokens():
    for f in ['compiler_options.txt', 'compiler_options_extra.txt']:
        if not os.path.isfile(f):
            continue
        with open(f, 'r') as ops_file:
            for line in ops_file.readlines():
                for word in line.rstrip('\n').split(' '):
                    yield word
                yield '\n'

def main():
    with open('.clangd', 'w') as f:
        f.write('CompileFlags:\n')
        f.write('  Add:\n')
        line = ''
        for t in getTokens():
            if t == '\n':
                f.write('    - ')
                f.write(line)
                f.write('\n')
                line = ''
            else:
                if line == '-I' and t.startswith('.'):
                    line += os.path.abspath(t)
                else:
                    line += t

if __name__ == '__main__':
    main()
