import unittest
import kumquat
import sys,copy
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

        result = self.routine_to_test(test_function,0.0,1.0)

        self.assertIsNotNone(result)

    def test_return_complex_float_pair(self):
        def test_function(x):
            return 1j

        result = self.routine_to_test(test_function,0.0,1.0)

        self.assertIsInstance(result,tuple)
        self.assertIsInstance(result[0],complex)
        self.assertIsInstance(result[1],float)

    def test_runs_with_lambda_function(self):
        test_function = lambda x: 1j
        
        result = self.routine_to_test(test_function,0.0,1.0)

        self.assertIsNotNone(result)

    # Test reference counting

    def test_integrand_reference_count_does_not_change(self):
        '''
        Tests that the reference count of the function to be integrated does not change
        due to being passed to the integration routine
        '''
        test_func = lambda x: 1j

        initial_ref_count = sys.getrefcount(test_func)
        _ = self.routine_to_test(test_func, 0.0,1.0)
        self.assertEqual(initial_ref_count, sys.getrefcount(test_func))

    def test_bounds_reference_count_does_not_change(self):
        test_func = lambda x: 1j

        lower_bound,upper_bound = 0.0,1.0

        lower_bound_initial_ref_count = sys.getrefcount(lower_bound)
        upper_bound_initial_ref_count = sys.getrefcount(upper_bound)
        _ = self.routine_to_test(test_func, 0.0,1.0)
        self.assertEqual(lower_bound_initial_ref_count, sys.getrefcount(lower_bound))
        self.assertEqual(upper_bound_initial_ref_count, sys.getrefcount(upper_bound))

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
                self.routine_to_test,
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
                        self.routine_to_test,
                        perfectly_fine_function,
                        "Not a Float",
                        1.0)

        self.assertRaises(TypeError,
                        self.routine_to_test,
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
                          self.routine_to_test,
                          does_not_return_complex,
                          0.0,1.0 )

    # Test extra args passed correctly

    def test_runs_for_integrand_with_1_extra_arg(self):
        test_function = lambda x, y: 1j

        result = self.routine_to_test(test_function,0.0,1.0,('a',))
        self.assertIsNotNone(result)

    def test_runs_for_integrand_with_2_extra_args(self):
        test_function = lambda x, y,z: 1j

        result = self.routine_to_test(test_function,0.0,1.0,('a',5))
        self.assertIsNotNone(result)

    def test_extra_args_change_result(self):
        test_function = lambda x,y: complex(y)

        result1,_ = self.routine_to_test(test_function,0.0,1.0,(1j,))
        result2,_ = self.routine_to_test(test_function,0.0,1.0,(2j,))

        self.assertNotAlmostEqual(result1,result2)

    def test_TypeError_if_extra_args_expected_but_not_given(self):
        def test_func(x,y):
            return 1j

        self.assertRaises(TypeError,self.routine_to_test,0.0,1.0)

    def test_TypeError_if_no_extra_args_expected_but_some_given(self):
        def test_func(x):
            return 1j

        self.assertRaises(TypeError,self.routine_to_test,0.0,1.0,(1,2))
        
    def test_TypeError_if_wrong_number_of_extra_args_given(self):
        def test_func(x,y):
            return 1j

        self.assertRaises(TypeError,self.routine_to_test,0.0,1.0,(1,2))

    def test_extra_arg_tuple_unchanged(self):
        test_function = lambda x,y,z : 1j

        arg_tuple = ('a', [1,2])
        initial_arg_tuple = copy.deepcopy(arg_tuple)

        _ = self.routine_to_test(test_function,0.0,1.0,arg_tuple)

        self.assertEqual(arg_tuple, initial_arg_tuple)
        
    def test_extra_arg_tuple_reference_count(self):
        test_function = lambda x,y,z : 1j

        arg_tuple = ('a', 'b')

        arg_tuple_ref_count = sys.getrefcount(arg_tuple)

        _ = self.routine_to_test(test_function,0.0,1.0,arg_tuple)

        self.assertEqual(arg_tuple_ref_count, sys.getrefcount(arg_tuple))

    def test_extra_args_reference_count(self):
        test_function = lambda x,y,z : 1j

        arg1,arg2 = ('a', 0.43243)

        arg1_ref_count = sys.getrefcount(arg1)
        arg2_ref_count = sys.getrefcount(arg2)

        _ = self.routine_to_test(test_function,0.0,1.0,(arg1,arg2))

        self.assertEqual(arg1_ref_count, sys.getrefcount(arg1))
        self.assertEqual(arg2_ref_count, sys.getrefcount(arg2))

class TestFiniteIntevalIntegration():
    
    # Note the purpose of these tests is not to test the unerlying
    # Integration library, it is to test the python wrapper. Consequently
    # we have not tried to find particularly difficult functions
    # for the quadrature routine to compute. 

    def test_exp_integral(self):
        '''
        tests that the integral of exp(i x) ~ 0 over 1 period
        '''
        result, err = self.routine_to_test(lambda x: cmath.exp(1j*x), 0, 2*pi)
        self.assertAlmostEqual(result.real,0.0,places=self.tolerance)
        self.assertAlmostEqual(result.imag,0.0,places=self.tolerance)

    def test_quadratic_integral(self):
        '''
        checks that the integal of a simple quadratic function is correct
        '''
        result, err = self.routine_to_test(lambda x: 3*(x-1j)**2, 0.0,1.0)
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

        result, err = self.routine_to_test(complicated_func, 0, 5)
        self.assertGreater(result.real,0.0)
        self.assertGreater(result.imag,0.0)


class TestGaussKronrod(unittest.TestCase,
                       TestIntegrationRoutine,
                       TestFiniteIntevalIntegration):
    
    def setUp(self):
        TestIntegrationRoutine.setUp(self)
        self.routine_to_test = kumquat.gauss_kronrod




if __name__ == '__main__':
    unittest.main()
