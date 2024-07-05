start_indexes = dict()

def read_input(filename, var_type, input_folder, tmax=None):
    input_data = []
    with open(f'../{input_folder}/{filename}', 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split(' ')
            if tmax is not None:
                line = line[:tmax]
            input_data.append([])
            for spk in line:
                input_data[len(input_data) - 1].append(var_type(spk))
    return input_data


def read_input_line(filename, line_to_read, var_type, input_folder):
    input_data = []

    if not filename in start_indexes.keys():
        start_indexes[filename] = list_line_start_indexes(filename, input_folder)

    with open(f'../{input_folder}/{filename}', 'r') as file:
        file.seek(start_indexes[filename][line_to_read])
        return [var_type(val) for val in file.readline().strip().split(' ')]


def list_line_start_indexes(filename,  input_folder):
    start_idx = [0]
    with open(f'../{input_folder}/{filename}', 'r') as file:
        for line in file:
            start_idx.append(start_idx[-1] + len(line))
    return start_idx[:-1]

def read_params(input_folder):
    _params = dict()
    with open(f'../{input_folder}/params', 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split('=')
            _params[line[0]] = line[1]
    return _params
