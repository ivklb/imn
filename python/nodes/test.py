import numpy as np
import typing


def run(a: int, b: np.ndarray) -> tuple[int, np.ndarray]:
    """
    This is a test function
    """
    return [a + 1, b + 1]

def run2() :
    pass

# {}
print(typing.get_type_hints(run2))


a = typing.get_type_hints(run)

# {'a': <class 'int'>, 'b': <class 'numpy.ndarray'>, 'return': tuple[int, numpy.ndarray]}
print(a)

# (<class 'int'>, <class 'int'>)
print(a["return"].__args__)
print(a["return"].__args__[0] == int)  # True
print(a["return"].__args__[1] == np.ndarray)  # True
