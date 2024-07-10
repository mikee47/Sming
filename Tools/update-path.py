
import os
import sys

def main():
    if sys.platform == 'win32':
        sep = ';'
        max_length = 1020
    else:
        sep = ':'
        max_length = 4090

    orig_path = os.environ['PATH']
    path_list = []
    for item in orig_path.split(sep):
        item = item.strip()
        if item == '':
            continue
        if item in path_list:
            continue
        if 'WinGet\\Packages' in item:
            continue
        path_list.append(item)

    for value in sys.argv[1:]:
        if value in path_list:
            continue
        path_list.insert(0, value)

    # for item in path_list:
    #     print(f'  {item}')
    new_path = sep.join(path_list)

    if len(new_path) > max_length:
        raise RuntimeError(f'Path too long: was {len(orig_path)} chars, now {len(new_path)}.')

    print(new_path)

if __name__ == '__main__':
    main()
