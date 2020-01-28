# Kumquat
Performs efficient complex valued numerical integration of real varaibles in python. 

Essentially a wrapper for the C++ Boost numerical quadriture package

## Requirements

Kumquat requires Python 3.7 or later and C++ boost version 1.66.0 or later

## Installation

To install Kumquat simply clone the repository, navigate to its location and run 
```bash
$ python setup.py
```

Kumquat requires C++ boost to be installed. If the setup script is unable to locate the required header files, it will ask for a filepath to their location. The latest version of Boost can be found at [https://www.boost.org/].

## Integration Routines

### gauss_kronrod

Performs Gauss-Kronrod adaptive quadrature on a finite interval. 

#### Example:
```python
>>> from cmath import exp, pi

>>> import kumquat

>>> kumquat.gauss_kronrod(lambda x: exp(1j*x), 0.0, 2*pi)
((-2.5692059876656286e-16+0j), 1.1308638867425836e-15)
```

#### Returns
| Name | Type | Description|
|---|---|---|
| result | `complex` | The reuslt of the integration|
|error   | `float`   | An estemate in the error in the result. Optained as the magnitude difference between the integration using `points` and using `(points-1)/2` points in the integation. |

#### Parameters
| Name | Type | Description|
|---|---|---|
|`f`   |Callable| Function to be integrated. Must take a point in the integration range as a `float` in its first argument and return a `complex`. Additional arguments can be passed to `f` via the `args` and `kwargs` parameters|
|`a`  | `float` |Lower limit of integration|
|`b`  | `float`| Upper limit of integration. Must be strictly greater than `a`|
    
#### Optional Parameters
| Name | Type | Default | Description |
| -----|------|---------|-------------|
|`args`|    `tuple`| `None`| Additional positional arguments to be passed to `f`. The position in the integration region must still be the first argument of f.|
|`kwargs`| `dict`| `None` | Additional keyword arguments to be passed to `f`|
    
#### Keyword Parameters
| Name | Type | Default | Description |
| -----|------|---------|-------------|
|`full_output`| `bool`| `False`|If true returns a dict containing additional infomation about the integration performed, in addition to the result and error estemate. This dict contains an estimate of the L1 norm of `f`, a list of the abscissa used in the integration, and a list of the weights used in the integration.|
|`max_levels`| `int`| `15` |The maximum number of levels of adaptive quadrature to be used in the integration. Set to `0` for non-adaptive quadrature.|
|`tolarence`| `float`| square root of machine epsilon |The maximum relative error in the result. Should not be set too close to machine precision.|
|`points`| `int`, must be in `{15,31,41,51,61}`| `31` | Number of points being used in each level of Gaussian quadrature.|

