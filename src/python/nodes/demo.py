import numpy as np


def run(image: np.ndarray) -> tuple[np.ndarray]:
    rv = np.transpose(image)
    rv = np.copy(rv, order='C')
    return (rv,)
