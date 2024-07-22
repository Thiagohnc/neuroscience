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


def read_params(input_folder):
    _params = dict()
    with open(f'{input_folder}/params', 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            if line == '' or line[0] == '#':
                continue
            line = line.split('=')
            _params[line[0]] = line[1]
    return _params
