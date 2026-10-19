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


def read_params(path, fullpath=False):
    _params = dict()
    if not fullpath:
        path = f'{path}/params'
    with open(path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            if line == '' or line[0] == '#':
                continue
            line = line.split('=')
            _params[line[0]] = line[1]
    return _params

def read_spike_average(path, fullpath=False):
    avg = []
    if not fullpath:
        path = f'{path}/spike_average'
    with open(path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            if line == '':
                continue
            avg.append(float(line))
    return avg

def read_adj_w(path, fullpath=False):
    adj = []
    if not fullpath:
        path = f'{path}/adjacency_weights'
    with open(path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            if line == '':
                continue
            adj.append([float(a) for a in line.split(' ')])
    return adj


def read_pearson(input_folder, filename="pearson"):
    D = []
    with open(f'{input_folder}/{filename}') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split(' ')
            D.append([])
            for val in line:
                D[-1].append(float(val))
    return D