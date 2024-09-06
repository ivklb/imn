import numpy as np
import importlib
import typing


def _type_to_str(t):
    if t == bool:
        return "bool"
    elif t == int:
        return "int"
    elif t == float:
        return "float"
    elif t == str:
        return "str"
    elif t == np.ndarray:
        return "ndarray"
    else:
        return str(t)


def inspect(module_name: str, function_name: str) -> tuple[dict[str, str], tuple]:
    """
    example output:
    ({'b': 'int', 'a': 'np.ndarray'}, ['int', 'np.ndarray'])
    """
    module = importlib.import_module(module_name)
    type_hints = typing.get_type_hints(getattr(module, function_name))
    rv_hints = None
    if "return" in type_hints:
        rv_hints = type_hints.pop("return")
        rv_hints = [_type_to_str(t) for t in rv_hints.__args__]

    args_hints = {k: _type_to_str(v) for k, v in type_hints.items()}
    print(args_hints, rv_hints)
    # TODO: use list
    # foo.__code__.co_varnames
    return args_hints, rv_hints


if __name__ == "__main__":
    print(inspect("inspect_util", "inspect"))
