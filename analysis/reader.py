def read_input(filename, var_type):
    input_data = []
    with open(f'../out/{filename}', 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split(' ')
            input_data.append([])
            for spk in line:
                input_data[len(input_data) - 1].append(var_type(spk))
    return input_data


def read_params():
    _params = dict()
    with open('../out/params', 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split('=')
            _params[line[0]] = line[1]
    return _params
