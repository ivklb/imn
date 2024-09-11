import numpy as np


def run(image: np.ndarray) -> tuple[np.ndarray]:
    rv = np.transpose(image)
    rv = 255 - rv
    return (rv,)
