import numpy as np


def run(image: np.ndarray) -> tuple[np.ndarray]:
    rv = np.transpose(image)
    return (rv,)
