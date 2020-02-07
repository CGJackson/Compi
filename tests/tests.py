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
        def dumby_routine(f,a,b,*args,**kwargs):
            raise NotImplementedError("A dumby routine has been called in place of a routine to be tested")
        self.routine_to_test = dumby_routine
        self.default_range = ()
        self.positional_args = 0
        self.func = lambda x: 1j
        self.tolerance = 7 #number of dp

    # Test basic funcitonality

    def test_return_not_None(self):
        def test_function(x):
            return 1j * (x-5j)**-2

        result = self.routine_to_test(test_function,*self.default_range)

        self.assertIsNotNone(result)

    def test_return_complex_float_pair(self):

        result = self.routine_to_test(self.func,*self.default_range)

        self.assertIsInstance(result,tuple)
        self.assertEqual(2,len(result))
        self.assertIsInstance(result[0],complex)
        self.assertIsInstance(result[1],float)

    def test_runs_with_lambda_function(self):
        test_function = lambda x: 1j * (x-5j)**-2
        
        result = self.routine_to_test(test_function,*self.default_range)

        self.assertIsNotNone(result)

    # Test reference counting

    def test_integrand_reference_count_does_not_change(self):
        '''
        Tests that the reference count of the function to be integrated does not change
        due to being passed to the integration routine
        '''

        initial_ref_count = sys.getrefcount(self.func)
        _ = self.routine_to_test(self.func, *self.default_range)
        self.assertEqual(initial_ref_count, sys.getrefcount(self.func))

    def test_bounds_reference_count_does_not_change(self):

        # Note unpythonic use of iterating over an index to avoid creating references in loop variables
        # which must then be kept track of

        intial_bounds_ref_counts = [sys.getrefcount(self.default_range[i]) for i in range(len(self.default_range))]

        _ = self.routine_to_test(self.func, *self.default_range)
        for i,initial_ref_count in enumerate(intial_bounds_ref_counts):
            self.assertEqual(initial_ref_count, sys.getrefcount(self.default_range[i]))

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
                *self.default_range
                )

    def test_TypeError_if_bounds_not_float(self):
        '''
        checks that the routine raises a ValueError if the bounds on
        the integration cannot be converted to a float
        '''
        perfectly_fine_function = lambda x: 0j

        for i, _ in enumerate(self.default_range):
            test_range = tuple(f if j != i else "Not a float" for j,f in enumerate(self.default_range))

            self.assertRaises(TypeError,
                            self.routine_to_test,
                            perfectly_fine_function,
                            *test_range)


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
                          *self.default_range)

    # Test extra args passed correctly

    def test_runs_for_integrand_with_1_extra_arg(self):
        test_function = lambda x, y: 1j * (x-5j)**-2

        result = self.routine_to_test(test_function,*self.default_range,('a',))
        self.assertIsNotNone(result)

    def test_runs_for_integrand_with_2_extra_args(self):
        test_function = lambda x, y,z: 1j *(x-5j)**-2

        result = self.routine_to_test(test_function,*self.default_range,('a',5))
        self.assertIsNotNone(result)

    def test_extra_args_change_result(self):
        test_function = lambda x,y: complex(y) *(1.0/(1+abs(0.1*x)))**2 # written this way to ensure no double overflow 

        result1,*_ = self.routine_to_test(test_function,*self.default_range,(1j,))
        result2,*_ = self.routine_to_test(test_function,*self.default_range,(2j,))

        self.assertNotAlmostEqual(result1,result2)

    def test_TypeError_if_extra_args_expected_but_not_given(self):
        def test_func(x,y):
            return 1j * (x-5j)**-2

        self.assertRaises(TypeError,self.routine_to_test,test_func,*self.default_range)

    def test_TypeError_if_no_extra_args_expected_but_some_given(self):
        def test_func(x):
            return 1j * (x-5j)**-2

        self.assertRaises(TypeError,self.routine_to_test,test_func,*self.default_range,(1,2))
        
    def test_TypeError_if_wrong_number_of_extra_args_given(self):
        def test_func(x,y):
            return 1j * (x-5j)**-2

        self.assertRaises(TypeError,self.routine_to_test,test_func,*self.default_range,(1,2))

    def test_extra_arg_tuple_unchanged(self):
        test_function = lambda x,y,z : 1j * (x-5j)**-2

        arg_tuple = ('a', [1,2])
        initial_arg_tuple = copy.deepcopy(arg_tuple)

        _ = self.routine_to_test(test_function,*self.default_range,arg_tuple)

        self.assertEqual(arg_tuple, initial_arg_tuple)
        
    def test_extra_arg_tuple_reference_count(self):
        test_function = lambda x,y,z : 1j * (x-5j)**-2

        arg_tuple = ('a', 'b')

        arg_tuple_ref_count = sys.getrefcount(arg_tuple)

        _ = self.routine_to_test(test_function,*self.default_range,arg_tuple)

        self.assertEqual(arg_tuple_ref_count, sys.getrefcount(arg_tuple))

    def test_extra_args_reference_count(self):
        test_function = lambda x,y,z : 1j * (x-5j)**-2

        arg1,arg2 = ('a', 0.43243)

        arg1_ref_count = sys.getrefcount(arg1)
        arg2_ref_count = sys.getrefcount(arg2)

        _ = self.routine_to_test(test_function,*self.default_range,(arg1,arg2))

        self.assertEqual(arg1_ref_count, sys.getrefcount(arg1))
        self.assertEqual(arg2_ref_count, sys.getrefcount(arg2))

    # Test Kwargs passed correctly
    def test_runs_for_integrand_with_1_kwarg(self):
        def test_function(x,y='kw'):
            return 1j * (x-5j)**-2

        result = self.routine_to_test(test_function,*self.default_range,(),{'y':9})
        self.assertIsNotNone(result)

    def test_runs_for_integrand_with_2_kwargs(self):
        def test_function(x,y='kw',z=2):
            return 1j * (x-5j)**-2

        result = self.routine_to_test(test_function,*self.default_range,(),{'y':9,'z':'f'})
        self.assertIsNotNone(result)

    def test_kwargs_change_result(self):
        def test_function(x,kw=1j): 
            return complex(kw) *(1.0/(1+abs(0.1*x)))**2
        
        result1,*_ = self.routine_to_test(test_function,*self.default_range,())
        result2,*_ = self.routine_to_test(test_function,*self.default_range,(),{'kw':2j})

        self.assertNotAlmostEqual(result1,result2)

    def test_TypeError_for_unexpected_kwargs(self):
        def test_func(x):
            return 1j * (x-5j)**-2

        self.assertRaises(TypeError,self.routine_to_test,test_func,*self.default_range,(),{'kw':'spam'})

    def test_kwarg_dict_unchanged(self):
        def test_func(x,kw=4):
            return 1j * (x-5j)**-2

        kw_dict = {'kw':5}
        initial_kw_dict = copy.deepcopy(kw_dict)

        _ = self.routine_to_test(test_func,*self.default_range,(),kw_dict)

        self.assertEqual(kw_dict,initial_kw_dict)

    def test_kwarg_dict_reference_count_unchenged(self):
        def test_func(x,kw=3):
            return 1j * (x-5j)**-2

        kw_dict = {'kw':5}

        initial_ref_count = sys.getrefcount(kw_dict)

        _ = self.routine_to_test(test_func,*self.default_range,(),kw_dict)

        self.assertEqual(initial_ref_count, sys.getrefcount(kw_dict))

    def test_kwarg_args_reference_count_unchanged(self):
        def test_func(x,kw=3):
            return 1j * (x-5j)**-2

        kw_arg = [2,4]
        kw_dict = {'kw':kw_arg}

        initial_ref_count = sys.getrefcount(kw_arg)

        _ = self.routine_to_test(test_func,*self.default_range,(),kw_dict)

        self.assertEqual(initial_ref_count, sys.getrefcount(kw_arg))

    def test_kwargs_reference_count_unchanged(self):
        def test_func(x,kw=3):
            return 1j * (x-5j)**-2
        kw_name = 'kw'
        kw_dict = {kw_name:5}

        initial_ref_count = sys.getrefcount(kw_name)

        _ = self.routine_to_test(test_func,*self.default_range,(),kw_dict)

        self.assertEqual(initial_ref_count, sys.getrefcount(kw_name))

    # Test integration routine parameters

    def _accept_ketword_test(self, keyword, val):
        self.assertIsNotNone(self.routine_to_test(self.func,*self.default_range,None,None,**{keyword:val}))


    def test_accept_tolerance_keyword(self):
        self._accept_ketword_test('tolerance', 0.1)

    def test_changing_tolerance_chages_result(self):
        def difficult_function(x):
            return ((0.501+0.00000001j -x)**(-1.5))*cmath.exp(1j*x)

        loose_result,*_ = self.routine_to_test(difficult_function,*self.default_range,tolerance=1.0)
        tight_result,*_ = self.routine_to_test(difficult_function,*self.default_range,tolerance=1e-10)

        self.assertNotAlmostEqual(loose_result,tight_result,places=self.tolerance)

    def test_accept_max_levels_keyword(self):
        self._accept_ketword_test('max_levels', 10)

    def test_max_levels_changes_result(self):
        '''
        Checks that the max_levels keyword is doing something by integrating a function which
        *should* require many levels to integrate accurately with different values of 
        max_levels and checking that they give a different result.

        Also checks that the result of an 'easy' function, that should not requite many levels
        to integrate, is not changed
        '''
        def difficult_function(x):
            '''
            A function that should require many levels to correctly numerically intergrate 
            '''
            ex = (-0.25*abs(x)+ 1j*x)
            t1 = cmath.exp(ex)
            t2 = 1/(0.501-x)
            return t1*t2 

        bad_result,*_ = self.routine_to_test(difficult_function,*self.default_range,max_levels = 0)
        good_result,*_ = self.routine_to_test(difficult_function,*self.default_range,max_levels = 10)

        self.assertNotAlmostEqual(bad_result,good_result, places=self.tolerance)

        def easy_function(x):
            return 1j * cmath.exp(-0.00001*abs(x))

        bad_result,*_ = self.routine_to_test(easy_function,*self.default_range,max_levels = 0)
        good_result,*_ = self.routine_to_test(easy_function,*self.default_range,max_levels = 10)
        self.assertAlmostEqual(good_result,bad_result,places=self.tolerance)

    def test_accept_full_output_keyword(self):
        self._accept_ketword_test('full_output',True)
        self._accept_ketword_test('full_output',False)

    def test_full_output_adds_dict_to_result(self):
        result = self.routine_to_test(self.func,*self.default_range, full_output=True)

        self.assertIsInstance(result,tuple)
        self.assertEqual(3,len(result))
        self.assertIsInstance(result[0],complex)
        self.assertIsInstance(result[1],float)
        self.assertIsInstance(result[2],dict)

class TestFiniteIntevalIntegration():
    
    # Note the purpose of these tests is not to test the unerlying
    # Integration library, it is to test the python wrapper. Consequently
    # we have not tried to find particularly difficult functions
    # for the quadrature routine to compute. 

    def test_exp_integral(self):
        '''
        tests that the integral of exp(i x) ~ 0 over 1 period
        '''
        result, *_ = self.routine_to_test(lambda x: cmath.exp(1j*x), 0, 2*pi)
        self.assertAlmostEqual(result.real,0.0,places=self.tolerance)
        self.assertAlmostEqual(result.imag,0.0,places=self.tolerance)

    def test_quadratic_integral(self):
        '''
        checks that the integal of a simple quadratic function is correct
        '''
        result, *_ = self.routine_to_test(lambda x: 3*(x-1j)**2, 0.0,1.0)
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

        result, *_ = self.routine_to_test(complicated_func, 0, 5)
        self.assertGreater(result.real,0.0)
        self.assertGreater(result.imag,0.0)

class TestInfiniteIntegration():

    def test_gaussian_integral(self):
        '''
        checks the gaussian itegral
        '''
        def gaussian(x):
            '''
            A gaussian function, written to avoid overflow errors at large arguments
            '''
            if x > 0:
                return cmath.exp(-(0.5+0.5j)*x)**x 
            else:
                return cmath.exp((0.5+0.5j)*x)**-x

        result, *_ = self.routine_to_test(gaussian)
        squared_result = result**2 # Squared result used to avoid ambiguity with multivaluedness of complex square root
        expected_squared_result = 2 * pi / (1+1j) 
        self.assertAlmostEqual(squared_result.real,expected_squared_result.real,places=self.tolerance)
        self.assertAlmostEqual(squared_result.imag,expected_squared_result.imag,places=self.tolerance)

    def test_lorenzian_integral(self):
        '''
        Checks the integral of a lorenzian with a complex pole vanishes
        '''
        result, *_ = self.routine_to_test(lambda x: (x-5j)**-2)
        self.assertAlmostEqual(result.real,0.0,places=self.tolerance)
        self.assertAlmostEqual(result.imag,0.0,places=self.tolerance)

    def test_positivity_of_complicated_integral(self):
        '''
        Integrates a function with positive real and imaginary parts.
        Checks that the result of the integral also has positive real and
        imaginary parts
        '''

        def complicated_func(x):
            return cmath.exp(-abs(x)) + 1j*cmath.exp(-5*abs(x))

        result, *_ = self.routine_to_test(complicated_func)
        self.assertGreater(result.real,0.0)
        self.assertGreater(result.imag,0.0)

    def test_VauleError_if_integrand_does_not_tend_to_zero(self):

        def does_not_tend_to_zero(x):
            return 1j

        self.assertRaises(ValueError,self.routine_to_test,does_not_tend_to_zero)
        self.assertRaises(ValueError,self.routine_to_test,lambda x: cmath.exp(1j*x))
    
class TestSemiInfiniteIntegration():
    def test_power_law(self):
        def power_law(x):
            return 1j*(1.0/(x+1))**2

        result, *_ = self.routine_to_test(power_law,0.0)

        self.assertAlmostEqual(result.real,0.0, places=self.tolerance)
        self.assertAlmostEqual(result.imag,1.0, places=self.tolerance)


    def test_exp(self):
        result, *_ = self.routine_to_test(lambda x: 1j*cmath.exp(-x),0.0)
        self.assertAlmostEqual(result.real,0.0,places=self.tolerance)
        self.assertAlmostEqual(result.imag,1.0,places=self.tolerance)

    def test_positivity_of_complicated_integral(self):
        '''
        Integrates a function with positive real and imaginary parts.
        Checks that the result of the integral also has positive real and
        imaginary parts
        '''

        def complicated_func(x):
            return (x+1)**-2 + 1j*cmath.exp(-5*x)

        result, *_ = self.routine_to_test(complicated_func,*self.default_range)
        self.assertGreater(result.real,0.0)
        self.assertGreater(result.imag,0.0)

class TestGaussKronrod(unittest.TestCase,
                       TestIntegrationRoutine,
                       TestFiniteIntevalIntegration):
    
    def setUp(self):
        TestIntegrationRoutine.setUp(self)
        self.routine_to_test = kumquat.gauss_kronrod
        self.default_range = (0.0,1.0)


    def test_accept_ponts_parameter(self):
        self._accept_ketword_test('points',15)

    def test_invalid_number_of_points_raises_ValueError(self):
        def func(x):
            return 1j

        self.assertRaises(ValueError, self.routine_to_test,func,*self.default_range,points=17)

    def test_points_changes_result(self):
        def difficult_function(x):
            return (0.501+0.00000001j -x)**(-1.5)

        bad_result,*_= self.routine_to_test(difficult_function,*self.default_range,max_levels=1, points=15)
        good_result,*_ = self.routine_to_test(difficult_function,*self.default_range,max_levels=1, points=61)

        self.assertNotAlmostEqual(bad_result,good_result,places=self.tolerance)

    def test_full_output_contains_L1_norm_abscissa_and_weights(self):
        def func(x):
            return 1j

        _,_,diagnostics = self.routine_to_test(func,*self.default_range,full_output=True)

        self.assertSetEqual({"L1 norm", "abscissa", "weights"}, set(diagnostics.keys()))
        self.assertIsInstance(diagnostics["L1 norm"], float)
        self.assertIsInstance(diagnostics["abscissa"][0], float)
        self.assertIsInstance(diagnostics["weights"][0], float)

class TestTanhSinh(unittest.TestCase,
                TestIntegrationRoutine,
                TestFiniteIntevalIntegration):
    def setUp(self):
        TestIntegrationRoutine.setUp(self)
        self.routine_to_test = kumquat.tanh_sinh
        self.default_range = (-1.0,1.0)

    def test_full_output_contains_L1_norm_levels(self):
        _,_,diagnostics = self.routine_to_test(self.func,*self.default_range,full_output=True)

        self.assertSetEqual({"L1 norm", "levels"}, set(diagnostics.keys()))
        self.assertIsInstance(diagnostics["L1 norm"], float)
        self.assertIsInstance(diagnostics["levels"], int)

class TestSinhSinh(unittest.TestCase,
                  TestInfiniteIntegration,
                  TestIntegrationRoutine):
    def setUp(self):
        TestIntegrationRoutine.setUp(self)
        self.routine_to_test = kumquat.sinh_sinh
        self.default_range = ()
        self.func = lambda x: (x-5j)**(-2)

    def test_full_output_contains_L1_norm_levels(self):
        _,_,diagnostics = self.routine_to_test(self.func,*self.default_range,full_output=True)

        self.assertSetEqual({"L1 norm", "levels"}, set(diagnostics.keys()))
        self.assertIsInstance(diagnostics["L1 norm"], float)
        self.assertIsInstance(diagnostics["levels"], int)

class TestExpSinh(unittest.TestCase,
                  TestSemiInfiniteIntegration,
                  TestIntegrationRoutine):
    def setUp(self):
        TestIntegrationRoutine.setUp(self)
        self.routine_to_test = kumquat.exp_sinh
        self.default_range = (0.0,)
        self.func = lambda x: cmath.exp(-x)

    def test_full_output_contains_L1_norm_levels(self):
        _,_,diagnostics = self.routine_to_test(self.func,*self.default_range,full_output=True)

        self.assertSetEqual({"L1 norm", "levels"}, set(diagnostics.keys()))
        self.assertIsInstance(diagnostics["L1 norm"], float)
        self.assertIsInstance(diagnostics["levels"], int)

    def test_accept_interval_infinity_keyword(self):
        self._accept_ketword_test("interval_infinity",+1)
        self.assertIsNotNone(self.routine_to_test(lambda x: 1j*cmath.exp(x),*self.default_range,None,None,**{"interval_infinity":-1}))

    def test_interval_infinity_changes_result(self):
        def asymmetric_function(x):
            return (1.0/(x-5+1j))**2 + (1.0/(x-2j))**3

        result1,*_ = self.routine_to_test(asymmetric_function, *self.default_range)
        result2,*_ = self.routine_to_test(asymmetric_function, *self.default_range, interval_infinity=-1)

        self.assertNotAlmostEqual(result1.real,result2.real,places=self.tolerance)
        self.assertNotAlmostEqual(result1.imag,result2.imag,places=self.tolerance)

    def test_interval_infinity_changes_sign_of_odd_intergral(self):
        '''
        Check that changing interval infinity is probably doing what we expect by putting in an integral where 
        the result on the 2 semi-infinite ranges are simply related
        '''
        def odd_function(x):
            return 1j*x*cmath.exp(-abs(x))

        pos_result,*_ = self.routine_to_test(odd_function,0.0,interval_infinity=+1)
        neg_result,*_ = self.routine_to_test(odd_function,0.0,interval_infinity=-1)

        self.assertAlmostEqual(pos_result.real, -neg_result.real, places=self.tolerance)
        self.assertAlmostEqual(pos_result.imag, -neg_result.imag, places=self.tolerance)

if __name__ == '__main__':
    unittest.main()
