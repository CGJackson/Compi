import unittest
import kumquat
import cmath, math
pi = cmath.pi

class TestIntegrationRoutine():
    '''
    Tests functionality common to all integration routines 
    '''

    def setUp(self):
        self.tolerance = 7 #number of dp

    # Test basic funcitonality

    def test_return_not_None(self):
        def test_function(x):
            return 1j

        result = self.test_routine(test_function,0.0,1.0)

        self.assertIsNotNone(result)

    def test_return_complex_float_pair(self):
        def test_function(x):
            return 1j

        result = self.test_routine(test_function,0.0,1.0)

        self.assertIsInstance(result,tuple)
        self.assertIsInstance(result[0],complex)
        self.assertIsInstance(result[1],float)

    def test_runs_with_lambda_function(self):
        test_function = lambda x: 1j
        
        result = self.test_routine(test_function,0.0,1.0)

        self.assertIsNotNone(result)

    #Tests correctly raising errors

    def test_function_raising_error(self):
        '''
        Tests that if the integrand function raises an exception, that 
        exception is sucessfully passed to the Python intepreter
        '''

        class TestFunctionRaisingException(Exception):
            '''
            A unique exception that cannot have been raised by anything
            other than the integrand function
            '''
            pass

        def exception_raising_function(x):
            raise TestFunctionRaisingException("Oh No!")

        self.assertRaises(TestFunctionRaisingException,
                self.test_routine,
                exception_raising_function,
                0.0,1.0
                )

    def test_TypeError_if_bounds_not_float(self):
        '''
        checks that the routine raises a ValueError if the bounds on
        the integration cannot be converted to a float
        '''
        perfectly_fine_function = lambda x: 0

        self.assertRaises(TypeError,
                        self.test_routine,
                        perfectly_fine_function,
                        "Not a Float",
                        1.0)

        self.assertRaises(TypeError,
                        self.test_routine,
                        perfectly_fine_function,
                        0.0,
                        "Not a Float")

    def test_ValueError_if_function_does_not_return_complex(self):
        '''
        Checks that the routine raises a ValueError if the function passed
        as an integrand returns a value that cannot be converted to a
        complex
        '''

        does_not_return_complex = lambda x: "Not a complex"

        self.assertRaises(ValueError,
                          self.test_routine,
                          does_not_return_complex,
                          0.0,1.0 )

class TestFiniteIntevalIntegration():
    
    # Note the purpose of these tests is not to test the unerlying
    # Integration library, it is to test the python wrapper. Consequently
    # we have not tried to find particularly difficult functions
    # for the quadrature routine to compute. 

    def test_exp_integral(self):
        '''
        tests that the integral of exp(i x) ~ 0 over 1 period
        '''
        result, err = self.test_routine(lambda x: cmath.exp(1j*x), 0, 2*pi)
        self.assertAlmostEqual(result.real,0.0,places=self.tolerance)
        self.assertAlmostEqual(result.imag,0.0,places=self.tolerance)

    def test_quadratic_integral(self):
        '''
        checks that the integal of a simple quadratic function is correct
        '''
        result, err = self.test_routine(lambda x: 3*(x-1j)**2, 0.0,1.0)
        expected_result = ( (1-1j)**3 ) - ( (-1j)**3 )
        self.assertAlmostEqual(result.real,expected_result.real,places=self.tolerance)
        self.assertAlmostEqual(result.imag,expected_result.imag,places=self.tolerance)

    def test_positivity_of_complicated_integral(self):
        '''
        Integrates a function with positive real and imaginary parts.
        Checks that the result of the integral also has positive real and
        imaginary parts
        '''
        def complicated_func(x):
            return math.exp(x) + 1j* math.exp(x**2)

        result, err = self.test_routine(complicated_func, 0, 5)
        self.assertGreater(result.real,0.0)
        self.assertGreater(result.imag,0.0)


class TestGaussKronrod(unittest.TestCase,
                       TestIntegrationRoutine,
                       TestFiniteIntevalIntegration):
    
    def setUp(self):
        TestIntegrationRoutine.setUp(self)
        self.test_routine = kumquat.gauss_kronrod




if __name__ == '__main__':
    unittest.main()
