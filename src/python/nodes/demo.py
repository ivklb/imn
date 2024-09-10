import numpy as np
import typing
# import imn


def run(image: np.ndarray) -> tuple[np.ndarray]:
    return np.transpose(image)
    # dtype = 'uint8'
    # if image.dtype == np.uint8:
    #     pass
    # elif image.dtype == np.uint16:
    #     dtype = 'uint16'
    # elif image.dtype == np.uint32:
    #     dtype = 'uint32'
    # elif image.dtype == np.float32:
    #     dtype = 'float32'

    # rv = imn.create_array(tmp.shape, dtype)
    # rv[:] = tmp
    # return rv

