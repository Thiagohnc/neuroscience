import re


def format_int(str_int):
    answer = ''
    as_int = int(str_int)
    while as_int >= 1000:
        answer += 'K'
        if as_int % 1000 == 0:
            as_int //= 1000
        else:
            as_int /= 1000
    return str(as_int) + answer

def format_decimal(str_decimal):
    remove_trailing_zeroes = re.sub(r'([^0])0*$', r'\1', str_decimal)
    remove_point_if_no_decimal = re.sub(r'\.$', '', remove_trailing_zeroes)
    return remove_point_if_no_decimal


def decimal_to_porc(str_decimal):
    return format_decimal(str(100 * float(str_decimal))) + '%'
