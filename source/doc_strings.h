#ifndef KUMQUAT_DOC_STRING_HEADER_GUARD
#define KUMQUAT_DOC_STRING_HEADER_GUARD

/* Doc strings must be C constant strings. It is therefore simplest to define them as macros */

/* Module docstring */
#define KUMQUAT_DOCS "Provides routine to perform efficient complex valued numeric integration\n\nContains:\n\ttrapezoidal: Performs trapeziodal quadrature over a finite interval\n\tgauss_kronrod: Performs Gauss-Kronrod quadrature over a finite interval\n\ttanh_sinh: Performs tanh-sinh quadrature over a finite, semi-infinite or infinite interval\n\tsinh_sinh: Performs sinh-sinh quadrature over an infinite interval\n\texp_sinh: Performs exp-sinh quadrature over a semi-infinite interval"


/* Function docstrings */
#define GAUSS_KRONROD_DOCS "Performs Gauss-Kronrod quadrature, returning a complex result and a real error estimate\n\nParameters:\n\tf: Callable. Function to be integrated. Must take a point in the integration range as a float in its first argument and return a complex. Additional arguments can be passed to f via the args and kwargs parameters\n\ta: float. Lower limit of integration\n\tb: float. Upper limit of integration. Must be strictly greater than a\n\nOptional Parameters:\n\targs: tuple. Additional positional arguments to be passed to f. The position in the integration region must still be the first argument of f. default None.\n\tkwargs: dict. Additional keyword arguments to be passed to f. Default None.\n\nKeyword Parameters:\n\tfull_output: bool. If true returns a dict containing additional infomation about the integration performed, in addition to the result and error estemate. This dict contains an estimate of the L1 norm of f, a list of the abscissa used in the integration, and a list of the weights used in the integration. Default False.\n\tmax_levels: int. The maximum number of levels of adaptive quadrature to be used in the integration. Set to 0 for non-adaptive quadrature. default 15\n\ttolarence: float. The maximum relative error in the result. Should not be set too close to machine precision, Default sqrt of machine precision\n\tpoints: int. Must be chosen from {15,31,41,51,61}. Number of points being used in each level of Gaussian quadrature."

#define TANH_SINH_DOCS "Performs tanh-sinh quadrature, returning a complex result and a real error estimate\n\nParameters:\n\tf: Callable. Function to be integrated. Must take a point in the integration range as a float in its first argument and return a complex. Additional arguments can be passed to f via the args and kwargs parameters\n\ta: float. Lower limit of integration. May be -inf.\n\tb: float. Upper limit of integration. Must be strictly greater than a. May be +inf\n\nOptional Parameters:\n\targs: tuple. Additional positional arguments to be passed to f. The position in the integration region must still be the first argument of f. default None.\n\tkwargs: dict. Additional keyword arguments to be passed to f. Default None.\n\nKeyword Parameters:\n\tfull_output: bool. If true returns a dict containing additional infomation about the integration performed, in addition to the result and error estemate. This dict contains an estimate of the L1 norm of f and the number of levels of adaptive quadrature used to achieve the required tolarence. Default False.\n\tmax_levels: int. The maximum number of levels of adaptive quadrature to be used in the integration. Set to 0 for non-adaptive quadrature. default 15\n\ttolarence: float. The maximum relative error in the result. Should not be set too close to machine precision, Default sqrt of machine precision."

#define SINH_SINH_DOCS "Performs sinh-sinh quadrature, returning a complex result and a real error estimate\n\nParameters:\n\tf: Callable. Function to be integrated. Must take a float in its first argument and return a complex. Additional arguments can be passed to f via the args and kwargs parameters\n\nOptional Parameters:\n\targs: tuple. Additional positional arguments to be passed to f. The position in the integration region must still be the first argument of f. default None.\n\tkwargs: dict. Additional keyword arguments to be passed to f. Default None.\n\nKeyword Parameters:\n\tfull_output: bool. If true returns a dict containing additional infomation about the integration performed, in addition to the result and error estemate. This dict contains an estimate of the L1 norm of f and the number of levels of adaptive quadrature used to achieve the required tolarence. Default False.\n\tmax_levels: int. The maximum number of levels of adaptive quadrature to be used in the integration. Set to 0 for non-adaptive quadrature. default 15\n\ttolarence: float. The maximum relative error in the result. Should not be set too close to machine precision, Default sqrt of machine precision."

#define EXP_SINH_DOCS "Performs exp-sinh quadrature, returning a complex result and a real error estimate\n\nParameters:\n\tf: Callable. Function to be integrated. Must take a point in the integration range as a float in its first argument and return a complex. Additional arguments can be passed to f via the args and kwargs parameters\n\tb: float. Boundry of the range of integration. Whether it is the upper or lower boundry depends on the sign of interval_infinity\n\nOptional Parameters:\n\targs: tuple. Additional positional arguments to be passed to f. The position in the integration region must still be the first argument of f. default None.\n\tkwargs: dict. Additional keyword arguments to be passed to f. Default None.\n\tinterval_infinity: float. Determines whether the range of integration is taken as b to +infinity or -infinity to b. If interval_infinity > 0 the range of integration is taken as going from b to +infinity. If interval_infinity < 0 the range -infinity to b is taken. interval_infinity == 0 raises a ValueError. Default 1.0\n\nKeyword Parameters:\n\tfull_output: bool. If true returns a dict containing additional infomation about the integration performed, in addition to the result and error estemate. This dict contains an estimate of the L1 norm of f and the number of levels of adaptive quadrature used to achieve the required tolarence. Default False.\n\tmax_levels: int. The maximum number of levels of adaptive quadrature to be used in the integration. Set to 0 for non-adaptive quadrature. default 15\n\ttolarence: float. The maximum relative error in the result. Should not be set too close to machine precision, Default sqrt of machine precision."

#define TRAPEZOIDAL_DOCS "Performs trapezoidal quadrature, returning a complex result and a real error estimate\n\nParameters:\n\tf: Callable. Function to be integrated. Must take a point in the integration range as a float in its first argument and return a complex. Additional arguments can be passed to f via the args and kwargs parameters\n\ta: float. Lower limit of integration\n\tb: float. Upper limit of integration. Must be strictly greater than a\n\nOptional Parameters:\n\targs: tuple. Additional positional arguments to be passed to f. The position in the integration region must still be the first argument of f. default None.\n\tkwargs: dict. Additional keyword arguments to be passed to f. Default None.\n\nKeyword Parameters:\n\tfull_output: bool. If true returns a dict containing additional infomation about the integration performed, in addition to the result and error estemate. This dict contains an estimate of the L1 norm of f. Default False.\n\tmax_levels: int. The maximum number of levels of adaptive quadrature to be used in the integration. Set to 0 for non-adaptive quadrature. default 12\n\ttolarence: float. The maximum relative error in the result. Should not be set too close to machine precision, Default sqrt of machine precision."

#endif